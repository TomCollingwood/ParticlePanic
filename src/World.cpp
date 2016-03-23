#include "include/World.h"

// Use this to keep track of the times
#include <sys/time.h>
#include <stdlib.h>

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

//    particles.push_back(Particle());
//    particles.push_back(Particle(Vec3(3.0f,0.0f)));
//    particles[1].setVelocity(Vec3(0.1f,0.0f));
//    particles.push_back(Particle(Vec3(3.0f,5.0f)));
//    particles[2].setVelocity(Vec3(-0.1f,0.0f));

    srand (time(NULL));
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*0.25f,3.0f-j*0.25f)));
      }
    }
    for(int a=0; a<10*10; ++a)
    {
      particles[a].setVelocity(Vec3(((float)(rand() % 100 - 50))*0.001f,((float)(rand() % 100 - 50))*0.001f));
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

    static int everyother = 0;
    everyother++;
    static int particlecount = 100;
    if(everyother%5==0){
      for(int i = 0; i<10; ++i)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*0.4f,5.0f)));
        particles[particlecount].setVelocity(Vec3(((float)(rand() % 100 - 50))*0.001f,((float)(rand() % 100 - 50))*0.001f));
        particlecount++;
      }
    }

    for(int i =0; i<(int)particles.size(); ++i)
    {
      particles[i].applyGravity();
      particles[i].updateVelocity();
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
          if(u>0)
          {
            float sig = particles[i].sig();
            float bet = particles[i].bet();
            Vec3 impulse = rij*((1-q)*(sig*u + bet*u*u));
            particles[i].addVelocity(-impulse/2.0f);
            particles[j].addVelocity(impulse/2.0f);
          }
        }
      }
    }

    // readjust springs
    for(int i =0; i<(int)particles.size(); ++i)
    {
      for(int j =i+1; j<(int)particles.size(); ++j)
      {
        Vec3 rij=(particles[j].getPosition()-particles[i].getPosition());
        float rijmag = rij.length();
        float q = rijmag/0.5f;
        if(q<1)
        {
          bool quit = false;
          int springindex=0; // index of relevant spring in particles[i].particlesprings
          for(int a = 0; a<particles[i].size() && !quit; ++a)
          {
            if(getSpring(a)->indexi==i)
            {
              if(indexj==j) springindex=a; quit=true;
            }
            else
            {
              if(indexi==j) springindex=a; quit=true;
            }
          }
          if(!quit)
          {
            struct Spring newspring;
            newspring.indexi = i;
            newspring.indexj = j;
            newspring.L = interactionradius; // maybe change this to sum of radius of two particles
            springs.push_back(newspring);
            particles[i].addSpring(&newspring);
            particles[j].addSpring(&newspring);
            springindex=particles[i].springNumber()-1;
          }
          float L = particles[i].getSpring(springindex)->L;
          float d=gamma*(particles[i].getSpring(springindex)->L);
          float alpha = particles[i].alph();

          if(rijmag>L+d)
          {
            particles[i].getSpring(springindex)->L=L+m_elapsedTime*alpha*(rijmag-L-d);
          }
          else if(rijmag<L-d)
          {
            particles[i].getSpring(springindex)->L=L-m_elapsedTime*alpha*(L-d-rijmag);
          }
        }
      }
    }

    // delete springs if over rest length?
    for(int i=0; i<(int)springs.size(); ++i)
    {
      if(springs[i].L>interactionradius)
      {
        particles[springs[i].indexi].deleteSpring(springs[j].indexj);
        particles[springs[i].indexj].deleteSpring(springs[i].indexi);
        springs.erase(i);
      }
    }


      // OLD METHOD WITHOUT CLEVER MATHS (INSIDE A FOR LOOP
//      particles[i].applyGravity();
//      static int number =0;
//      for(int j=i+1; j<(int)particles.size(); ++j){
//        if (particles[i].collision(particles[j]))
//        {
//          number++;
//          //printf("CRASH(%d)",number);
//          Vec3 newi = Vec3(particles[i].getPosition()-particles[j].getPosition());
//          Vec3 newj = Vec3(particles[j].getPosition()-particles[i].getPosition());
//          //printf("1: %f, %f | 2: %f, %f |",particles[i].getPosition()[0],particles[i].getPosition()[1],particles[j].getPosition()[0],particles[j].getPosition()[1]);
//          newi.normalize();
//          newj.normalize();
//          newi*=particles[i].getVelocity().length()*0.1f;
//          newj*=particles[j].getVelocity().length()*0.1f;
//          particles[i].addForce(newi);
//          particles[j].addForce(newj);
//          printf("(%f)",newi[0]);
//        }
//      }
//      particles[i].updateVelocity();


    // boundaries
    for(int i =0; i<(int)particles.size();++i)
    {

      if(particles[i].getPosition()[1]-0.5f<-halfheight)
      {
        particles[i].setVelocity(Vec3(particles[i].getVelocity()[0],-(particles[i].getVelocity()[1])*0.2f));
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
      particles[i].updatePosition();
      particles[i].clearForces();
    }
}


