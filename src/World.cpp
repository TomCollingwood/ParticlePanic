#include "include/World.h"

// Use this to keep track of the times
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

/**
 * @brief World::World
 */
World::World() : m_isInit(false),m_startTime(0.0),m_elapsedTime(0.0)
{
}

/**
 * @brief World::~World destructorize this thing. Make sure to eliminate dynamically allocated memory.
 */
World::~World() {
}

/**
 * @brief World::init initialises the GL World, enabling features that are needed
 */
void World::init() {
    // Sanity check - if we've already initialised this class we shouldn't be here
    if (m_isInit) return;

    // Disable texturing
    glDisable(GL_TEXTURE_2D);

    // Enable counter clockwise face ordering
    glFrontFace(GL_CCW); // front face - determines normal in order you specify vertices

    //glEnable(GL_LIGHTING);
    glDisable(GL_LIGHTING); // no longer need normals or lights
    glEnable(GL_NORMALIZE);

    // Make our points lovely and smooth
    glEnable( GL_POINT_SMOOTH );
    glEnable( GL_MULTISAMPLE_ARB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SIZE);
    glPointSize(10.f);

    // Set the background colour
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set our start time by using the gettimeofday function (accurate to 10 nanosecs)
    struct timeval tim;
    gettimeofday(&tim, NULL);
    m_startTime = tim.tv_sec+(tim.tv_usec * 1e-6);

    srand (time(NULL));

    // can only get away with this as they are the first particles (otherwise insertParticle)
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*0.2f,3.0f-j*0.2f)));
      }
    }
    for(int a=0; a<10*10; ++a)
    {

      particles[a].setVelocity(Vec3(((float)(rand() % 100 - 50)),((float)(rand() % 10000 - 50))));
    }


    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*0.2f,-1.0f-j*0.2f)));
      }
    }
    for(int a=0; a<10*10; ++a)
    {

      particles[a].setVelocity(Vec3(((float)(rand() % 100 - 50))*0.001f,4.0f+((float)(rand() % 100 - 50))*0.001f));
    }

    interactionradius=0.5f;

    m_isInit = true;
}

/**
 * @brief World::resize needs to set up the camera paramaters (i.e. projection matrix) and the viewport
 * @param w Width of window
 * @param h Height of window
 */
void World::resize(int w, int h) {
  if (!m_isInit) return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float i = 5;
  float ara = float(w)/float(h);
  glOrtho(-i*ara,i*ara,-i,i,0.1, 10.0);
  halfheight=i;
  halfwidth=i*ara;

  glViewport(0,0,w,h);

  glMatrixMode(GL_MODELVIEW);

  // initialize grid
  squaresize = interactionradius; //each square is size of interaction radius
  gridwidth=ceil((halfwidth*2)/squaresize);
  gridheight=ceil((halfheight*2)/squaresize);
  grid.resize(gridheight*gridwidth);
  //initialize cells conatining
  cellsContainingParticles.clear();
  cellsContainingParticles.resize(gridheight*gridwidth,false);
}

/**
 * @brief World::draw draws the World to the current GL context. Called a lot - make this fast!
 */
void World::draw() {
    if (!m_isInit) return;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    for(int i =0; i<(int)particles.size(); ++i){
      particles[i].drawParticle();
    }
}


/**
 * @brief World::update updates the World based on a timer. Used for animation.
 */
void World::update() {
    if (!m_isInit) return;

    // Some stuff we need to perform timings
    struct timeval tim;

    // Retrieve the current time in nanoseconds (accurate to 10ns)
    gettimeofday(&tim, NULL);
    double now =tim.tv_sec+(tim.tv_usec * 1e-6);

    // Increment the rotation based on the time elapsed since we started running
    m_elapsedTime = m_startTime - now;

    //make it rain
    /*
    static int everyother = 0;
    everyother++;
    if(everyother%5==0){
      for(int i = 0; i<4; ++i)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*0.2f,5.0f)));
        particles.back().setVelocity(Vec3(((float)(rand() % 20 - 10))*0.0001f,((float)(rand() % 20 - 10))*0.0001f));
      }
    }

    //*/

    // apply gravity to velocity
    for(int i =0; i<(int)particles.size(); ++i)
    {
      particles[i].applyGravity();
      particles[i].updateVelocity(m_elapsedTime);
    }

    // viscosity algorithm
    for(int i =0; i<(int)particles.size(); ++i)
    {
      for(int j =i+1; j<(int)particles.size(); ++j)
      {
        Vec3 rij=(particles[j].getPosition()-particles[i].getPosition());
        float q = rij.length()/interactionradius;
        if(q<1)
        {
          rij.normalize();
          float u = (particles[i].getVelocity()-particles[j].getVelocity()).dot(rij);
          //printf("(%f)",u);
          if(u>0)
          {
            float sig = particles[i].sig();
            float bet = particles[i].bet();
            Vec3 impulse = rij*((1-q)*(sig*u + bet*u*u))*0.01f*m_elapsedTime;
            particles[i].addVelocity(-impulse/2.0f);
            particles[j].addVelocity(impulse/2.0f);
          }
        }
      }
    }

    // update previous and current position
    for(int i =0; i<(int)particles.size(); ++i)
    {
      particles[i].updatePrevPosition();
      particles[i].updatePosition(m_elapsedTime);
    }
    cellsContainingParticles.clear();
    cellsContainingParticles.resize(gridwidth*gridheight,false);
    hashParticles();

    //readjust the springs
    for(int k=0; k<gridheight*gridwidth; ++k)
    {
      if(cellsContainingParticles[k])
      {
        std::vector<Particle *> surroundingParticles = getSurroundingParticles(k);
        for(int i=0; i<(int)surroundingParticles.size(); ++i)
        {
          for(int j=i+1; j<(int)surroundingParticles.size();++j)
          {
            Vec3 rij=(surroundingParticles[j]->getPosition()-surroundingParticles[i]->getPosition());
            float rijmag = rij.length();
            float q = rijmag/interactionradius;
            if(q<1)
            {
              bool quit = false;
              Spring *thisspring;
              for(int a = 0; a<(int)springs.size() && !quit; ++a)
              {
                if(((springs[a].indexi==(surroundingParticles[i])) && (springs[a].indexj==(surroundingParticles[j]))) ||
                   ((springs[a].indexi==(surroundingParticles[j])) && (springs[a].indexj==(surroundingParticles[i]))))
                {
                  thisspring=&springs[a];
                  //std::cout<<"FUCK"<<std::endl;
                  quit=true;
                  break;
                }
              }

              if(!quit)
              {
                Spring newspring;
                newspring.indexi=surroundingParticles[i];
                newspring.indexj=surroundingParticles[j];
                newspring.L = interactionradius; // maybe change this to sum of radius of two particles
                springs.push_back(newspring);
              }
              thisspring = &springs.back();
              GLfloat L = thisspring->L;
              GLfloat d= L*surroundingParticles[i]->gam();
              GLfloat alpha = surroundingParticles[i]->alp();

              if(rijmag>L+d)
              {
                thisspring->L=L+0.001f*m_elapsedTime*alpha*(rijmag-L-d);
              }
              else if(rijmag<L-d)
              {
                thisspring->L=L-0.001f*m_elapsedTime*alpha*(L-d-rijmag);
              }
            }
          }
        }
      }
    }

    std::cout<<"s"<<(int)springs.size()<<"p"<<(int)particles.size()<<std::endl;

    // delete springs if over rest length?
    for(int i=0; i<(int)springs.size(); ++i)
    {
      if(springs[i].L>interactionradius)
        springs.erase(springs.begin()+i);
    }

    //spring displacements
    for(int i=0; i<(int)springs.size(); ++i)
    {
      Vec3 rij = (springs[i].indexi)->getPosition() - (springs[i].indexj)->getPosition();
      float rijmag = rij.length();
      rij.normalize();
      Vec3 D = rij*m_elapsedTime*m_elapsedTime*0.1f*(1-(springs[i].L/interactionradius))*(springs[i].L-rijmag);
      springs[i].indexi->addPosition(-D/2);
      springs[i].indexj->addPosition(D/2);
    }


    //----------------------------------BOUNDARIES --------------------------------------------
    for (std::deque<Particle>::iterator it=particles.begin(); it!=particles.end();)
    {
      if(std::abs(it->getPosition()[0])>halfwidth)
      {
        for(std::deque<Spring>::iterator it2=springs.begin(); it2!=springs.end();)
        {
          if(it2->indexi==&*(it) || it2->indexj==&*(it))
            it2=springs.erase(it2);
          else
            ++it2;
        }
        it=particles.erase(it);
      }
      else
        ++it;

      if(it->getPosition()[1]-0.5f<-halfheight)
      {
        it->setPosition(Vec3(it->getPosition()[0]))

      }


    }
    //----------------------------------CLEANUP ------------------------------------------------
    cellsContainingParticles.clear();
}

void World::hashParticles()
{
  grid.clear();
  grid.resize(gridheight*gridwidth);
  int grid_cell;
  for(auto i : particles)
  {
    grid_cell=floor((i.getPosition()[0]+halfwidth)/squaresize)+floor((i.getPosition()[1]+halfheight)/squaresize)*gridwidth;
    cellsContainingParticles[grid_cell]=true;
    grid[grid_cell].push_back(&i);
  }
}

std::vector<Particle *> World::getSurroundingParticles(int thiscell) const
{
  int numSurrounding=1;
  std::vector<Particle *> surroundingParticles;
  for(int i = -numSurrounding; i <= numSurrounding; ++i)
  {
    for(int j = -numSurrounding; j <= numSurrounding; ++j)
    {
      int grid_cell = thiscell+ i + j*gridwidth;
      if(grid_cell<gridwidth*gridheight || grid_cell>=0)
        surroundingParticles.insert(surroundingParticles.begin(),grid[grid_cell].begin(),grid[grid_cell].end());
    }
  }
  return surroundingParticles;
}

