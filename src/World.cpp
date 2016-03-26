#include "include/World.h"

// Use this to keep track of the times
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

/**
 * @brief World::World
 */
World::World() :
  m_isInit(false),
  m_startTime(0.0),
  m_elapsedTime(0.0),
  interactionradius(0.3f),
  squaresize(0.3f),
  m_timestep(0.0001)
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
    glPointSize(13.f);

    // Set the background colour
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set our start time by using the gettimeofday function (accurate to 10 nanosecs)
    struct timeval tim;
    gettimeofday(&tim, NULL);
    m_startTime = tim.tv_sec+(tim.tv_usec * 1e-6);
    srand (time(NULL));

    // create start two blocks of particles
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
        particles.push_back(Particle(Vec3(-3.0f+i*0.3f,3.0f-j*0.3f)));
    }
    for(int a=0; a<10*10; ++a)
    {
      //particles[a].setVelocity(Vec3(-8.0f,0.0f));
      //particles[a].addVelocity(Vec3(0.0f,(float)(rand() % 100 - 50)*0.01f));
      //particles[a].setVelocity(Vec3(((float)(rand() % 100 - 50)),((float)(rand() % 10000 - 50))));
    }

    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
        particles.push_back(Particle(Vec3(3.0f+i*0.2f,3.0f-j*0.2f)));
    }
    for(int a=100; a<10*10*2; ++a)
      //particles[a].setVelocity(Vec3(8.0f,0.0f));

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
  gridwidth=ceil((halfwidth*2)/squaresize);
  gridheight=ceil((halfheight*2)/squaresize);
  grid.resize(gridheight*gridwidth);
  //initialize cells conatining
  cellsContainingParticles.clear();
  cellsContainingParticles.resize(gridheight*gridwidth,false);
  hashParticles();
  std::cout<<gridwidth<<std::endl;
}

/**
 * @brief World::draw draws the World to the current GL context. Called a lot - make this fast!
 */
void World::draw() {
    if (!m_isInit) return;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    for(auto& i : particles){
      i.drawParticle();
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
    static int everyother = 0;
    everyother++;
    int divisor = (int)(0.00005f/m_timestep);
    if(divisor==0) divisor=1;
    if(everyother%5==0){
      for(int i = 0; i<4; ++i)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*0.4f,5.0f)));
        particles.back().setVelocity(Vec3(((float)(rand() % 20 - 10))*0.0001f,-2.0f));
      }
    }
    //*/

    // ------------------------------GRAVITY --------------------------------------------
    for(auto& i : particles)
    {
      i.applyGravity(m_timestep);
    }

    // ------------------------------VISCOSITY--------------------------------------------
    // TODO : Implement spatial hash for loop here


    int choo = 0;

    for(auto& k : grid)
    {
      int ploo = 0;
      for(auto& i : k)
      {
        std::vector<Particle *> surroundingParticles = getSurroundingParticles(ploo);
        int cloo = 0;
        for(auto& j : surroundingParticles)
        {
          if(cloo>ploo)
          {

            Vec3 rij=(j->getPosition()-i->getPosition());
            float q = rij.length()/interactionradius;
            if(q<1 && q!=0)
            {
              rij.normalize();
              float u = (i->getVelocity()-j->getVelocity()).dot(rij);
              //printf("(%f)",u);
              if(u>0)
              {
                float sig = i->sig();
                float bet = i->bet();
                Vec3 impulse = rij*((1-q)*(sig*u + bet*u*u))*m_timestep;
                i->addVelocity(-impulse/2.0f);
                j->addVelocity(impulse/2.0f);
              }
            }

          }
          cloo++;
        }
      }
      ploo++;
    }
    // */

    //------------------------------------------POSITION----------------------------------------

    for(auto& i : particles)
    {
      i.updatePrevPosition();
      i.updatePosition(m_timestep);
    }
    hashParticles();

    //--------------------------------------SPRINGS-----------------------------------------------


    for(int k=0; k<gridheight*gridwidth; ++k)
    {
      if(cellsContainingParticles[k])
      {
        //std::cout<<"not now"<<std::endl;
        std::vector<Particle *> surroundingParticles = getSurroundingParticles(k);
        // std::cout<<(int)surroundingParticles.size()<<std::endl;

        //std::cout<<"START";
        for(auto& i : grid[k])
        {
          //std::cout<<"("<<surroundingParticles[i]->getPosition()[0]<<")"<<std::endl;
          for(auto& j : surroundingParticles)
          {
            Vec3 rij=(j->getPosition()-i->getPosition());
            float rijmag = rij.length();
            float q = rijmag/interactionradius;

            if(q<1 && q!=0)
            {
              bool quiter = false;
              Particle::Spring *thisspring;

              /*
              for(auto& spring : springs)
              {
                if(((spring.indexi==i) && (spring.indexj==j)) ||
                   ((spring.indexi==j) && (spring.indexj==i)))
                {
                  thisspring=&spring;
                  quiter=true;
                  //std::cout<<"FUCK"<<std::endl;
                  break;
                }

              }
              // */

              if(!quiter)
              {
                Particle::Spring newspring;
                newspring.indexi=i;
                newspring.indexj=j;
                newspring.L = interactionradius; // maybe change this to sum of radius of two particles

                //springs.push_back(newspring);

                //thisspring = &(springs.back());
                thisspring = &newspring; // delete this
              }
              GLfloat L = thisspring->L;
              GLfloat d= L*i->gam();
              GLfloat alpha = i->alp();

              if(rijmag>L+d)
              {
                thisspring->L=L+m_timestep*alpha*(rijmag-L-d);
              }
              else if(rijmag<L-d)
              {
                thisspring->L=L-m_timestep*alpha*(L-d-rijmag);
              }
            }
            // */
          }
        }
      }
    }

    //std::cout<<"s"<<(int)springs.size()<<"p"<<(int)particles.size()<<std::endl;
    // delete springs if over rest length?


    choo=0;
    for(auto& i : springs)
    {
      if(i.L>interactionradius)
      {
        std::list<Particle::Spring>::iterator it;
        it = springs.begin();
        advance(it,choo);
        springs.erase(it);
      }
      else{choo++;}
    }

    // */
    //spring displacements


    for(auto& i : springs)
    {
      Vec3 rij = (*(i.indexj)).getPosition() - (*(i.indexi)).getPosition();
      float rijmag = rij.length();

      rij.normalize();
      Vec3 D = rij*m_timestep*0.3f*(1-(i.L/interactionradius))*(i.L-rijmag);
      //if((1-(springs[i].L/interactionradius))!=0) std::cout<<(1-(springs[i].L/interactionradius))<<"<---"<<std::endl;
      i.indexi->addPosition(-D/2);
      i.indexj->addPosition(D/2);
    }
    // */

    //----------------------------------DOUBLEDENSITY------------------------------------------


    int count =0;

    for(auto& list : grid)
    {
      std::vector<Particle *> neighbours=getSurroundingParticles(count);
      for(auto& i : list)
      {
        float density =0;
        float neardensity=0;
        for(auto& j : neighbours)
        {
          Vec3 rij = j->getPosition()-i->getPosition();
          float rijmag = rij.length();
          float q = rijmag/interactionradius;
          if(q<1 && q!=0)
          {
            density+=(1.0f-q)*(1.0f-q);
            neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);

          }
        }
        float p0 = 10.0f;
        float k = 0.004f;
        float knear = 0.05f;
        float P = k*(density -p0);
        float Pnear = knear * neardensity;
        Vec3 dx = Vec3();
        for(auto& j : neighbours)
        {
          Vec3 rij = j->getPosition()-i->getPosition();
          float rijmag = rij.length();
          float q = rijmag/interactionradius;
          if(q<1 && q!=0)
          {
            rij.normalize();
            Vec3 D = rij*(m_timestep*m_timestep*(P*(1.0f-q))+Pnear*(1.0f-q)*(1.0f-q));
            j->addPosition(D/2);
            dx-=(D/2);
          }
        }
        i->addPosition(dx);
      }
      count++;
    }

    //----------------------------------MAKE NEW VELOCITY-------------------------------------


    for(auto& list : grid)
    {
      for(auto& i : list)
      {
        i->setVelocity((i->getPosition()-i->getPrevPosition())/m_timestep);
      }
    }


    //----------------------------------BOUNDARIES --------------------------------------------
    count = 0;
    for (auto& it : particles)
    {
      if(std::abs(it.getPosition()[0])>halfwidth )//|| it.getPosition()[1]-0.5f<-halfheight)
      {
        //here we iterate though particle springs TODO
        int count2 =0;
        for(auto& it2 : springs)
        {
          if(it2.indexi==&it || it2.indexj==&it)
          {
            std::list<Particle::Spring>::iterator it = springs.begin();
            advance(it, count2);
            springs.erase(it);
          }
          else
          {
            ++count2;
            //std::cout<<"ooohy"<<std::endl;
          }
        }
        std::list<Particle>::iterator it = particles.begin();
        advance(it, count);
        particles.erase(it);
      }
      else
        ++count;
      // */


      if(it.getPosition()[1]-0.5f<-halfheight)
      {
        it.setPosition(Vec3(it.getPosition()[0],-halfheight+0.5f));
        it.addVelocity(Vec3(0.0f,-0.5f*it.getVelocity()[1]));
      }

      if(it.getPosition()[0]>halfwidth-0.5f)
      {
        it.setPosition(Vec3(halfwidth-0.5f,it.getPosition()[1]));
        it.addVelocity(Vec3(-0.5f*it.getVelocity()[0],0.0f));
      }

      if(it.getPosition()[0]<-halfwidth+0.5f)
      {
        it.setPosition(Vec3(-halfwidth+0.5f,it.getPosition()[1]));
        it.addVelocity(Vec3(-0.5f*it.getVelocity()[0],0.0f));
      }
      //*/
    }
    //----------------------------------CLEANUP ------------------------------------------------

    gettimeofday(&tim, NULL);
    double now2 =tim.tv_sec+(tim.tv_usec * 1e-6);
    double difference = now2-now;
    static double max= 0;
    if(difference>max) max=difference;
    std::cout<<max<<std::endl;
}

void World::hashParticles()
{
  cellsContainingParticles.assign(gridwidth*gridheight,false);
  std::vector<Particle *> newvector;
  grid.assign(gridheight*gridwidth,newvector);
  int grid_cell;
  for(auto& i : particles)
  {
    grid_cell=floor((i.getPosition()[0]+halfwidth)/squaresize)+floor((i.getPosition()[1]+halfheight)/squaresize)*gridwidth;
    if(grid_cell>=0 && grid_cell<gridheight*gridwidth)
    {
      cellsContainingParticles[grid_cell]=true;
      //std::cout<<"THIS ONE!"<<(&i)->getPosition()[1]<<std::endl;
      grid[grid_cell].push_back(&i);
    }
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
      if(grid_cell<(gridwidth*gridheight) && grid_cell>=0)
      {
        for(auto& p : grid[grid_cell])
        {
          surroundingParticles.push_back(p);
        }
      }
    }
  }
  return surroundingParticles;
}

