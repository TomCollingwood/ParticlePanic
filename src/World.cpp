#include "include/World.h"

// Use this to keep track of the times
#include <sys/time.h>

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
    glPointSize(20.f);

    // Set the background colour
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set our start time by using the gettimeofday function (accurate to 10 nanosecs)
    struct timeval tim;
    gettimeofday(&tim, NULL);
    m_startTime = tim.tv_sec+(tim.tv_usec * 1e-6);

    particles.push_back(Particle());
    particles.push_back(Particle(Vec3(0.0f,5.0f)));

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

    for(int i =0; i<(int)particles.size(); ++i){
      particles[i].applyGravity();
      particles[i].updateVelocity();
      if(particles[i].getPosition()[1]-0.5f<-halfheight)
      {
        if(particles[i].getVelocity()[1]<-0.0005f)
          particles[i].setVelocity(-(particles[i].getVelocity())*0.8f);
        else particles[i].setVelocity(Vec3(0.0f,0.0f));
      }

      /*
      for(int j=i+1; j<(int)particles.size(); ++j){
        if (particles[i].collision(particles[j]))
        {
          Vec3 newi = Vec3(particles[i].getPosition()-particles[j].getPosition()).perpendicular();
          Vec3 newj = Vec3(particles[j].getPosition()-particles[i].getPosition()).perpendicular();
          particles[i].addForce(newi);
          particles[j].addForce(newj);
        }
      }
      //*/

      particles[i].updatePosition();
      particles[i].clearForces();
    }
}


