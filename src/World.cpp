#include "include/World.h"

// Use this to keep track of the times
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>

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

  float squaresize = 5.0f;
  // initialize map
  int mapwidth=ceil((float)w/squaresize);
  int mapheight=ceil((float)h/squaresize);
  float onepixel=(halfheight*2)/h; // one pixel is halfheight/h vertex coordinates;
  map.resize(mapheight);
  for(int i=0; i<mapheight; i++)
  {
    map[i].resize(mapwidth);
  }
  printf("(w:%d,h:%d)",mapwidth,mapheight);
  // fill it up baby
  for(int i=0; i<(int)particles.size(); ++i)
  {
    int x = floor((particles[i].getPosition()[0]+halfwidth)/(squaresize*onepixel));
    int y = floor((particles[i].getPosition()[1]+halfheight)/(squaresize*onepixel));
    printf("(%d,%d)",x,y);
    map[y][x].push_back(&particles[i]);
  }

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
    static int particlecount = 100;
    if(everyother%5==0){
      for(int i = 0; i<5; ++i)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*1.0f,5.0f)));
        particles[particlecount].setVelocity(Vec3(((float)(rand() % 20 - 10))*0.0001f,((float)(rand() % 20 - 10))*0.0001f));
        particlecount++;
      }
    }
    */

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




    // readjust springs
    for(int h=0; h<mapheight; ++h)
    {
      for(int w=0; w<mapwidth; ++w)
      {
        for(int i=0; i<map[h][w].size(); ++i)
        {
          for(int j=0; j<map[h][w].size();++j)
          {
            Vec3 rij=(map[h][w][j]->getPosition()-map[h][w][i]->getPosition());
            float rijmag = rij.length();
            float q = rijmag/0.5f;
            if(q<1)
            {
              bool quit = false;
              Spring *thisspring;
              for(int a = 0; a<(int)springs.size() && !quit; ++a)
              {
                if(((springs[a].indexi==(map[h][w][i])) && (springs[a].indexj==(map[h][w][j]))) ||
                   ((springs[a].indexi==(map[h][w][j])) && (springs[a].indexj==(map[h][w][i]))))
                  thisspring=&springs[a];
              }
              if(!quit)
              {
                Spring newspring;
                newspring.indexi = map[h][w][i];
                newspring.indexj = map[h][w][j];
                newspring.L = interactionradius; // maybe change this to sum of radius of two particles
                springs.push_back(newspring);
              }
              thisspring = &springs.back();
              GLfloat L = thisspring->L;
              GLfloat d= L*map[h][w][i]->gam();
              GLfloat alpha = map[h][w][i]->alp();

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
      //printf("(%f,%f)",(float)D[0],(float)D[1]);
    }

    //*/
    // boundaries
    for(int i =0; i<(int)particles.size();++i)
    {
      if(particles[i].getPosition()[1]-0.5f<-halfheight)
      {
        // erase springs
        for(int j=0; j<(int)springs.size();++j)
        {
          if(springs[j].indexi==&particles[i] || springs[j].indexj==&particles[i])
            springs.erase(springs.begin()+j);
        }
        // erase particle
        particles.erase(particles.begin()+i);
      }

      if(std::abs(particles[i].getPosition()[0])>halfwidth)
      {
        particles[i].setVelocity(Vec3(-(particles[i].getVelocity()[0])*0.2f,particles[i].getVelocity()[1]));
        if(particles[i].getPosition()[0]<0)
        {
          particles[i].setPosition(Vec3(-halfwidth+0.01f,particles[i].getPosition()[1]));
        }
        else
        {
          particles[i].setPosition(Vec3(halfwidth-0.01f,particles[i].getPosition()[1]));
        }
      }
      particles[i].updatePosition(m_elapsedTime);
      particles[i].clearForces();
    }
}

