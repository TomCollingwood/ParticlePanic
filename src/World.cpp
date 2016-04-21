#include "include/World.h"

// Use this to keep track of the times
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

//#include <GLUT/glut.h>

//#include <SDL2_image>

/**
 * @brief World::World
 */
World::World() :
  m_isInit(false),
  m_startTime(0.0),
  m_elapsedTime(0.0),
  interactionradius(1.0f),
  squaresize(1.0f),
  m_timestep(1.0f),
  pointsize(10.0f),
  mainrenderthreshold(30.0f),  //90
  renderresolution(3),
  renderoption(1),
  rain(false),
  drawwall(false),
  gravity(true),
  springsize(500000),
  particlesPoolSize(3000),
  m_3d(false),
  m_boundaryMultiplier(1.0f),
  m_boundaryType(0)
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
  glEnable(GL_TEXTURE_2D);

  // Enable counter clockwise face ordering
  glFrontFace(GL_CCW); // front face - determines normal in order you specify vertices

  //glEnable(GL_LIGHTING);
  glEnable(GL_LIGHTING); // no longer need normals or lights
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT_MODEL_AMBIENT);

  GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

  // Make our points lovely and smooth
  glEnable( GL_POINT_SMOOTH );
  glEnable( GL_MULTISAMPLE_ARB);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POINT_SIZE);
  glEnable(GL_COLOR_MATERIAL);
  glPointSize(pointsize);

  // Set the background colour
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // Set our start time by using the gettimeofday function (accurate to 10 nanosecs)
  struct timeval tim;
  gettimeofday(&tim, NULL);
  m_startTime = tim.tv_sec+(tim.tv_usec * 1e-6);
  srand (time(NULL));

  particles.clear();
  springs.clear();

  Particle defaultparticle;
  defaultparticle.setAlive(false);
  particles.resize(particlesPoolSize,defaultparticle);
  firstFreeParticle=0;
  lastTakenParticle=-1;
  howManyAliveParticles=0;

  Particle::Spring defaultspring;
  defaultspring.alive=false;
  springs.resize(springsize,defaultspring);
  firstFreeSpring=0;
  lastTakenSpring=-1;

  // DEFAULT PARTICLE PROPERTIES
  m_particleTypes.push_back(ParticleProperties()); //water
  m_particleTypes.push_back(ParticleProperties(true,0.3f,0.2f,0.004f,0.01f,0.01f,0.004f,0.3f,10.0f,0.8f,0.52f,0.25f,false)); //slime
  m_particleTypes.push_back(ParticleProperties(false,0.3f,0.2f,0.004f,0.01f,0.01f,0.004f,0.3f,10.0f,0.8f,0.52f,0.25f,false)); //blobby
  m_particleTypes.push_back(ParticleProperties()); //random

  //water=ParticleProperties(true, 0.6f,0.8f,0.4,0.8f,0.01f,0.004,0.3,10.0f,0.5f,0.27f,0.07f,false);
  //water=ParticleProperties(false,0.0175,0.3472,0.0004,0.3,0.007336,0.0038962,0.3,2.368,0.1f,0.5,0.8f,true);

  //random=ParticleProperties();
  //random.randomize();
  m_todraw=0; // This is the liquid to draw (tap or mouse)

  m_previousmousex=-10;
  m_previousmousey=-10;

  m_camerarotatey=0.0f;
  m_camerarotatex=0.0f;

  // create start two blocks of particles

  /*
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
      {
        Particle newparticle = Particle(Vec3(-3.0f+i*0.1f,3.0f-j*0.1f,-2.0f),&m_particleTypes[m_todraw]);
        newparticle.setIsObject();
        insertParticle(newparticle);
      }
    }
    */



  /*
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
        particles.push_back(Particle(Vec3(3.0f+i*0.2f,3.0f-j*0.2f)));
    }
    for(auto& i : particles)
      i.addVelocity(Vec3(((float)(rand() % 100 - 50)*0.001f),((float)(rand() % 10000 - 50))*0.001f));
*/

  m_isInit = true;
}

/**
 * @brief World::resize needs to set up the camera paramaters (i.e. projection matrix) and the viewport
 * @param w Width of window
 * @param h Height of window
 */

void World::resizeWorld(int w, int h)
{
  pixelheight=h;
  pixelwidth=w;

  float i = 5;
  float ara = float(w)/float(h);

  halfheight=i;
  halfwidth=i*ara;

  gridwidth=ceil((halfwidth*2)/squaresize);
  gridheight=ceil((halfheight*2)/squaresize);
  griddepth=gridwidth;

  cellsContainingParticles.clear();
  grid.clear();

  if(!m_3d)
  {
    grid.resize(gridheight*gridwidth);
    cellsContainingParticles.resize(gridheight*gridwidth,false);
  }
  else
  {
    grid.resize(gridheight*gridwidth*griddepth);
    cellsContainingParticles.resize(gridheight*gridwidth*griddepth,false);
  }

  renderwidth=gridwidth*renderresolution;
  renderheight=gridheight*renderresolution;

  // GHOST PARTICLES

  //particles.push_back(Particle(Vec3(0.0f,0.0f,-2.0f),m_todraw));
  //particles.back().setWall(true);

  /*

  int density = 70;
  float gap = (halfwidth*2)/(float)density;
  int prevm_todraw=m_todraw;
  int m_todraw=0;
  for(int i = 0; i<density+1; ++i)
  {
    for(int j=0; j<3; ++j)
    {
      Particle newparticle = Particle(Vec3(-halfwidth+i*gap,-halfheight+0.5f-j*gap,-2.0f),&m_particleTypes[m_todraw]);
      newparticle.setWall(true);
      insertParticle(newparticle);
    }
  }

  density = 50;
  gap = (halfheight*2)/(float)density;
  for(int i = 0; i<density+1; ++i)
  {
    for(int j=0; j<3; ++j)
    {
      Particle newparticle = Particle(Vec3(-halfwidth-j*gap,-halfheight+i*gap,-2.0f),&m_particleTypes[m_todraw]);
      newparticle.setWall(true);
      insertParticle(newparticle);
    }
  }

  density = 80;
  gap = (halfheight*2)/(float)density;
  for(int i = 0; i<density+1; ++i)
  {
    for(int j=0; j<3; ++j)
    {
      Particle newparticle = Particle(Vec3(halfwidth+j*gap,-halfheight+i*gap,-2.0f),&m_particleTypes[m_todraw]);
      newparticle.setWall(true);
      insertParticle(newparticle);
    }
  }
  m_todraw=prevm_todraw;
  // */
  hashParticles();

}

void World::resizeWindow(int w, int h) {
  howmanytimesrandomized=0;
  if (!m_isInit) return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  pixelheight=h;
  pixelwidth=w;

  float i = 5;
  float ara = float(w)/float(h);

  glOrtho(-i*ara,i*ara,-i,i,0.1, 5000.0);

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

  glMatrixMode(GL_MODELVIEW);

  bool current_3d=m_3d;
  if(current_3d)
  {
    glPushMatrix();
    glTranslatef(0.0f,2.0f,-10.0f);
    glTranslatef(0.0f, 0.0f, -2.0f); // move back to focus of gluLookAt
    glRotatef(m_camerarotatex,0.0f,1.0f,0.0f); //  rotate around center
    glRotatef(m_camerarotatey,1.0f,0.0f,0.0f); //  rotate around center
    glTranslatef(0.0f, 0.0f, 2.0f); //move object to center
  }

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  if(renderoption==1){
    for(int i=0; i<lastTakenParticle+1; ++i){
      if(particles[i].isAlive())
        particles[i].drawParticle();
    }
  }


  else if(renderoption==2)
  {


    if(!m_3d)
    {
      for(auto& i : m_particleTypes)
      {
        glDisable(GL_LIGHTING);
        std::vector<std::vector<float>> waterRenderGrid = renderGrid(&i);
        drawMarchingSquares(waterRenderGrid,i,false);
        drawMarchingSquares(waterRenderGrid,i,true);
        glEnable(GL_LIGHTING);
      }
    }
    else
    {
      for(auto& i : m_particleTypes)
      {
        std::vector<std::vector<std::vector<float>>> waterRender3dGrid = render3dGrid(&i);
        waterRender3dGrid[0][0][0]=100;
        drawMarchingCubes(waterRender3dGrid,i);

        /*
        for(auto& i : waterRender3dGrid)
        {
          for(auto& j : i)
          {
            for(auto& k : j)
            {
              if(k>0) std::cout<<k<<std::endl;
            }
          }
        }

        int render3dheight=waterRender3dGrid.size();
        int render3dwidth=waterRender3dGrid[0].size();
        int render3ddepth=waterRender3dGrid[0][0].size();

        for(int h=0; h<render3dheight-1; ++h)
        {
          for(int w=0; w<render3dwidth-1; ++w)
          {
            for(int d=0; d<render3ddepth-1; ++d)
            {
              //std::vector<float> gridvalue;
              if(waterRender3dGrid[h][w][d]>0) std::cout<<"here"<<waterRender3dGrid[h][w][d]<<std::endl;
            }
          }
        }

        */
        //drawMarchingCubes(waterRender3dGrid,i,true);
      }
    }


  }

  if(current_3d) glPopMatrix();
}


/**
 * @brief World::update updates the World based on a timer. Used for animation.
 */
void World::update(bool *updateinprogress) {
  if (!m_isInit) return;
  *updateinprogress = true;

  // Some stuff we need to perform timings
  struct timeval tim;

  // Retrieve the current time in nanoseconds (accurate to 10ns)
  //gettimeofday(&tim, NULL);
  //double now =tim.tv_sec+(tim.tv_usec * 1e-6);

  // Increment the rotation based on the time elapsed since we started running
  //m_elapsedTime = m_startTime - now;

  //make it rain

  // 2d/3d different

  static int everyother = 0;
  everyother++;

  if(rain)
  {
    if(everyother%2==0){
      if(!m_3d)
      {
        for(int i = 0; i<10; ++i)
        {
          Particle newParticle = Particle(Vec3(-3.0f+i*0.15f,halfheight/2+0.5f,-2.0f),&m_particleTypes[m_todraw]);
          newParticle.addVelocity(Vec3(0.0f,-0.05f,0.0f));
          insertParticle(newParticle);
        }
      }
      else
      {
        for(int j = 0; j< 5; ++j)
        {
          for(int i = 0; i<5; ++i)
          {
            Particle newParticle =Particle(Vec3(i*0.3f,halfheight/5,-2.0+j*0.3f),&m_particleTypes[m_todraw]);
            newParticle.addVelocity(Vec3(0.0f,0.0f,0.0f));
            insertParticle(newParticle);
          }
        }
      }
    }
  }

  // ------------------------------GRAVITY --------------------------------------------
  if(gravity)
  {
    Vec3 gravityvel = Vec3(0.0f,-0.008*m_timestep,0.0f);
    //gravityvel.rotateAroundXAxisf(-m_camerarotatey*(M_PI/180.0f));
    // PARALLEL
    for(int i=0; i<lastTakenParticle+1; ++i)
    {
      if(particles[i].isAlive()) particles[i].addVelocity(gravityvel);
    }
  }

  // ------------------------------VISCOSITY--------------------------------------------
  // TODO : Implement spatial hash for loop here


  int choo = 0;

  //#pragma omp parallel for ordered schedule(dynamic)
  for(auto k = 0; k<(int)grid.size(); ++k)
  {
    int ploo = 0;
    for(auto& i : grid[k])
    {
      if(!(i->getWall()))
      {
        std::vector<Particle *> surroundingParticles = getSurroundingParticles(choo,1,false);
        int cloo = 0;
        for(auto& j : surroundingParticles)
        {
          if(cloo>ploo && !(j->getWall()))
          {
            Vec3 rij=(j->getPosition()-i->getPosition()); //THFP
            //std::cout<<rij[2]<<std::endl;
            float q = rij.length()/interactionradius;
            if(q<1 && q!=0)
            {
              rij.normalize();
              float u = (i->getVelocity()-j->getVelocity()).dot(rij); //THFP
              if(u>0)
              {
                ParticleProperties *thisproperties = i->getProperties(); // fiddle with this!!!
                float sig = thisproperties->getSigma();
                float bet = thisproperties->getBeta();
                Vec3 impulse = rij*((1-q)*(sig*u + bet*u*u))*m_timestep;
                i->addVelocity(-impulse/2.0f);
                j->addVelocity(impulse/2.0f);
              }
            }

          }
          cloo++;
        }
        ploo++;

      }
    }
    choo++;
  }
  // */

  //------------------------------------------POSITION----------------------------------------

  for(int i=0; i<lastTakenParticle+1; ++i)
  {
    if(particles[i].isAlive())
    {
      particles[i].updatePrevPosition();
      if(!(particles[i].getDrag())&&!(particles[i].getWall()))
        particles[i].updatePosition(m_timestep);
    }
  }
  hashParticles();

  //--------------------------------------SPRING ALGORITMNS-----------------------------------------------


  //#pragma omp parallel for ordered
  for(int k=0; k<gridheight*gridwidth; ++k)
  {
    if(cellsContainingParticles[k])
    {
      std::vector<Particle *> surroundingParticles = getSurroundingParticles(k,3,false);

      for(auto& i : grid[k])
      {
        if(i->getProperties()->getSpring() && (!i->isObject() || (i->isObject() && !i->isInit()) ) && !i->getWall())
        {
          for(auto& j : surroundingParticles)
          {
            if(j->getProperties()==i->getProperties()) // They only cling when same type
            {
              Vec3 rij=(j->getPosition()-i->getPosition());
              float rijmag = rij.length();
              float q = rijmag/interactionradius;

              if(q<1 && q!=0)
              {
                // FINDING / CREATING THE SPRING
                bool quiter = false;
                int thisspring;

                for(auto& spring : i->particleSprings)
                {
                  if(((springs[spring].indexi==i->getIndex()) && (springs[spring].indexj==j->getIndex())) ||
                     ((springs[spring].indexi==j->getIndex()) && (springs[spring].indexj==i->getIndex())))
                  {
                    springs[spring].alive=true;
                    thisspring=spring;
                    quiter=true; // FOUND EXISTING SPRING
                    break;
                  }
                }

                if(!quiter)
                {
                  Particle::Spring newspring;
                  newspring.indexi=i->getIndex();
                  newspring.indexj=j->getIndex();
                  newspring.count=everyother-1;
                  newspring.alive=true;
                  newspring.L = interactionradius; // maybe change this to sum of radius of two particles

                  thisspring = insertSpring(newspring);

                  i->particleSprings.push_back(thisspring);
                  j->particleSprings.push_back(thisspring);
                }

                // MAKING SURE EACH SPRING IS ONLY UPDATED ONCE PER FRAME
                if(springs[thisspring].count!=everyother)
                {
                  GLfloat L = springs[thisspring].L;
                  GLfloat d= L*i->getProperties()->getGamma();
                  GLfloat alpha = i->getProperties()->getAlpha();

                  if(rijmag>L+d)
                  {
                    springs[thisspring].L=L+m_timestep*alpha*(rijmag-L-d);
                  }
                  else if(rijmag<L-d)
                  {
                    springs[thisspring].L=L-m_timestep*alpha*(L-d-rijmag);
                  }
                  springs[thisspring].count++;
                }
              }
            }
          }
          i->setInit();
        }
      }
    }
  }

  // delete springs if over rest length?


  //spring displacements

  int count=0;
  for(auto& i : springs)
  {
    if(i.alive){
      Vec3 rij = (particles[i.indexj].getPosition() - particles[i.indexi].getPosition());
      float rijmag = rij.length();

      if(rijmag>interactionradius && !particles[i.indexi].isObject())
      {
        deleteSpring(count);
      }

      else{
        rij.normalize();
        Vec3 D = rij*m_timestep*m_timestep*particles[i.indexi].getProperties()->getKspring()*(1-(i.L/interactionradius))*(i.L-rijmag);
        particles[i.indexi].addPosition(-D/2);
        particles[i.indexj].addPosition(D/2);
      }

    }
    count++;
  }
  defragSprings();
  // */

  //----------------------------------DOUBLEDENSITY------------------------------------------


  count =0;

  //#pragma omp parallel for ordered
  for(int k = 0; k<(int)grid.size(); ++k)
  {
    std::vector<Particle *> neighbours=getSurroundingParticles(count,1,false);

    for(auto& i : grid[k])
    {
      float density =0;
      float neardensity=0;
      for(auto& j : neighbours)
      {
        Vec3 rij = j->getPosition()-i->getPosition(); // THFP
        float rijmag = rij.length();
        float q = rijmag/interactionradius;
        if(q<1 && q!=0) // q==0 when same particle
        {
          density+=(1.0f-q)*(1.0f-q);
          neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);
        }
      }


      if(m_boundaryType==1)
      {
        // BOTTOM
        float distance = halfheight + i->getPosition()[1];
        float q = distance/(m_boundaryMultiplier*interactionradius);
        if(q<1 && q!=0) // q==0 when same particle
        {
          density+=(1.0f-q)*(1.0f-q);
          neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);
        }
        // RIGHT
        distance = halfwidth - i->getPosition()[0];
        q = distance/(m_boundaryMultiplier*interactionradius);
        if(q<1 && q!=0) // q==0 when same particle
        {
          density+=(1.0f-q)*(1.0f-q);
          neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);
        }

        // LEFT
        distance = i->getPosition()[0] + halfwidth ;
        q = distance/(m_boundaryMultiplier*interactionradius);
        if(q<1 && q!=0) // q==0 when same particle
        {
          density+=(1.0f-q)*(1.0f-q);
          neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);
        }
      }



      float p0 = i->getProperties()->getP0();             // PROPERTIES <--------------------------------------
      float k = i->getProperties()->getK();
      float knear = i->getProperties()->getKnear();

      float P = k*(density -p0);
      float Pnear = knear * neardensity;
      Vec3 dx = Vec3();
      for(auto& j : neighbours)
      {
        Vec3 rij = j->getPosition()-i->getPosition(); // THFP
        float rijmag = rij.length();
        float q = rijmag/interactionradius;
        if(q<1 && q!=0)
        {
          rij.normalize();
          Vec3 D = rij*(m_timestep*m_timestep*(P*(1.0f-q))+Pnear*(1.0f-q)*(1.0f-q));
          if(!(j->getWall())) j->addPosition(D/2);
          dx-=(D/2);
        }
      }
      if(!(i->getWall())) i->addPosition(dx);
    }
    count++;
  }
  // */

  //----------------------------------MAKE NEW VELOCITY-------------------------------------

  for(auto& list : grid)
  {
    for(auto& i : list)
    {
      i->setVelocity((i->getPosition()-i->getPrevPosition())/m_timestep);
    }
  }

  //----------------------------------BOUNDARIES --------------------------------------------

  // 2d/3d different

  float smallen = 0.4f;
  if(!m_3d) smallen=1.0f;


  for(int i=0; i<lastTakenParticle+1; ++i)
  {
    if(particles[i].isAlive())
    {
      // OpenMP <<---
      // MTSi parallelism for loop as prrallel as you can on an intel.

      if(m_boundaryType==0)
      {
        //------------------------------------BOTTOM------------------------------
        if(particles[i].getPosition()[1]-0.5f<-halfheight)
        {
          particles[i].setPosition(Vec3(particles[i].getPosition()[0],-halfheight+0.5f,particles[i].getPosition()[2]));
          particles[i].setVelocity(Vec3(0.5f*particles[i].getVelocity()[0],-0.5f*particles[i].getVelocity()[1],0.0f));
        }
        //------------------------------------TOP------------------------------

        if(particles[i].getPosition()[1]+1.5f>halfheight)
        {
          particles[i].setPosition(Vec3(particles[i].getPosition()[0],halfheight-1.5f,particles[i].getPosition()[2]));
          particles[i].addVelocity(Vec3(0.0f,-0.8f*particles[i].getVelocity()[1],0.0f));
        }

        //------------------------------------RIGHT------------------------------
        if(particles[i].getPosition()[0]>(halfwidth-0.5f)*smallen)
        {
          particles[i].setPosition(Vec3(smallen*(halfwidth-0.5f),particles[i].getPosition()[1],particles[i].getPosition()[2]));
          particles[i].addVelocity(Vec3(-0.8f*particles[i].getVelocity()[0],0.0f));
        }
        //------------------------------------LEFT------------------------------
        if(particles[i].getPosition()[0]<(-halfwidth+0.5f)*smallen)
        {
          particles[i].setPosition(Vec3(smallen*(-halfwidth+0.5f),particles[i].getPosition()[1],particles[i].getPosition()[2]));
          particles[i].addVelocity(Vec3(-0.8f*particles[i].getVelocity()[0],0.0f));
        }
        if(particles[i].getPosition()[2]<-2-(halfwidth+0.5f)*smallen)
        {
          particles[i].setPosition(Vec3(particles[i].getPosition()[0],particles[i].getPosition()[1],-2-(halfwidth+0.5)*smallen));
          particles[i].addVelocity(Vec3(0.0f,0.0f,-0.8f*particles[i].getVelocity()[2]));
        }
        if(particles[i].getPosition()[2]>-2+(halfwidth-0.5f)*smallen)
        {
          particles[i].setPosition(Vec3(particles[i].getPosition()[0],particles[i].getPosition()[1],-2+(halfwidth-0.5f)*smallen));
          particles[i].addVelocity(Vec3(0.0f,0.0f,-0.8f*particles[i].getVelocity()[2]));
        }
      }

      if(m_boundaryType==1)
      {

        float distance = - particles[i].getPosition()[1] + halfheight - 0.5f;
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          particles[i].addVelocity(Vec3(0.0f,-force,0.0f));
        }

        distance = halfheight + particles[i].getPosition()[1];
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          particles[i].addVelocity(Vec3(0.0f,force,0.0f));
        }

        distance = particles[i].getPosition()[0] + halfwidth*smallen;
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          particles[i].addVelocity(Vec3(force,0.0f,0.0f));
        }

        distance = halfwidth*smallen - particles[i].getPosition()[0];
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          particles[i].addVelocity(Vec3(-force,0.0f,0.0f));
        }

        distance = particles[i].getPosition()[2] - (-2-halfwidth*smallen);
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          particles[i].addVelocity(Vec3(0.0f,0.0f,force));
        }

        distance = (-2+halfwidth*smallen) - particles[i].getPosition()[2] ;
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          particles[i].addVelocity(Vec3(0.0f,0.0f,-force));
        }


      }

    }
  }
  // */
  //----------------------------------CLEANUP ------------------------------------------------

  if(everyother%30==0)
  {
    std::cout<<"Numebr:"<<howManyAliveParticles<<std::endl;
  }

  int howmany=howManyAliveParticles;
  if(howmany==0) howmany=1;
  if(((float)lastTakenParticle-(float)firstFreeParticle)/((float)howmany) >0.5)
  {
    std::cout<<"fraction: "<<((float)lastTakenParticle-(float)firstFreeParticle)/((float)howmany)<<std::endl;
    std::cout<<"lasttaken: "<<lastTakenParticle<<"  firstfree: "<<firstFreeParticle<<std::endl;
  }

  /*
    gettimeofday(&tim, NULL);
    static double now = tim.tv_sec+(tim.tv_usec * 1e-6);
    static double now2 = tim.tv_sec+(tim.tv_usec * 1e-6);

    now2=now;
    now=tim.tv_sec+(tim.tv_usec * 1e-6);

    double calcTimeStep = now-now2;
    //if(calcTimeStep==0) calcTimeStep=0.03;

    static float total=0;
    static int howmanys=0;
    static double max = calcTimeStep;
    if(calcTimeStep>max) max=calcTimeStep;
    if(everyother%300==0)
    {
      total+=max;
      howmanys++;
      std::cout<<total/(float)howmanys<<":"<<howmanys<<std::endl;
      max=0;
      particles.clear();
    }
    // */


  *updateinprogress = false;
}

//---------------------------------HASH FUNCTIONS--------------------------------------------------------

void World::hashParticles()
{
  int gridSize;
  if(!m_3d) gridSize = gridwidth*gridheight;
  else gridSize = gridwidth*gridheight*griddepth;

  cellsContainingParticles.assign(gridSize,false);
  std::vector<Particle *> newvector;
  grid.assign(gridSize,newvector);
  int grid_cell;
  for(int i=0; i<lastTakenParticle+1; ++i)
  {
    if(particles[i].isAlive())
    {
      float positionx = particles[i].getPosition()[0];
      float positiony = particles[i].getPosition()[1];
      float positionz = particles[i].getPosition()[2];

      if(positionx<-halfwidth) positionx=halfwidth;
      else if (positionx>halfwidth) positionx=halfwidth;
      if(positiony<-halfheight) positiony=halfheight;
      else if (positiony>halfheight) positiony=halfheight;
      if(positionz<-2-halfwidth) positionz=-2-halfwidth;
      else if (positionz>-2+halfwidth) positionz=-2+halfwidth;


      grid_cell=
          floor((positionx+halfwidth)/squaresize)+
          floor((positiony+halfheight)/squaresize)*gridwidth;


      if(m_3d) grid_cell+=floor((positionz+halfwidth+2)/squaresize)*gridwidth*gridheight;

      particles[i].setGridPosition(grid_cell);

      if(grid_cell>=0 && grid_cell<gridSize)
      {
        cellsContainingParticles[grid_cell]=true;
        grid[grid_cell].push_back(&particles[i]);
      }
    }
  }
}

std::vector<Particle *> World::getSurroundingParticles(int thiscell, int numsur, bool dragselect) const
{
  int numSurrounding=1;
  std::vector<Particle *> surroundingParticles;
  if(!m_3d)
  {
    for(int i = -numSurrounding; i <= numSurrounding; ++i)
    {
      for(int j = -numSurrounding; j <= numSurrounding; ++j)
      {
        int grid_cell = thiscell+ i + j*gridwidth;
        if(grid_cell<(gridwidth*gridheight) && grid_cell>=0)
        {
          for(auto& p : grid[grid_cell])
          {
            if(dragselect && !p->getWall() || !dragselect) surroundingParticles.push_back(p);
          }
        }
      }
    }
  }
  else
  {
    for(int i = -numSurrounding; i <= numSurrounding; ++i)
    {
      for(int j = -numSurrounding; j <= numSurrounding; ++j)
      {
        for(int k = -numSurrounding; k <= numSurrounding; ++k)
        {
          int grid_cell = thiscell+ i + j*gridwidth + k*gridwidth*gridheight;

          if(grid_cell<(gridwidth*gridheight*griddepth) && grid_cell>=0)
          {
            for(auto& p : grid[grid_cell])
            {
              if(dragselect && !p->getWall() || !dragselect) surroundingParticles.push_back(p);
            }
          }
        }
      }
    }
  }

  return surroundingParticles;
}

//---------------------------------GRID FUNCTIONS--------------------------------------------------------


Vec3 World::getGridColumnRow(int k)
{
  int row = floor(k/gridwidth);
  int column = k - row*gridwidth;
  return Vec3(column,row);
}

//--------------------------------INPUT FUNCTIONS---------------------------------------------------

void World::mouseDraw(int x, int y)
{
  float objectdensity=0.1f;
  if(drawwall) objectdensity=0.05f;

  float currentx = ((float)x/(float)pixelwidth)*(halfwidth*2) - halfwidth;
  float currenty = -((float)y/(float)pixelheight)*(halfheight*2) + halfheight;

  float correctedx = floor(currentx/objectdensity + 0.5f);
  correctedx*=objectdensity;

  float correctedy = floor(currenty/objectdensity + 0.5f);
  correctedy*=objectdensity;

  bool drawparticle=true;
  int grid_cell=floor((correctedx+halfwidth)/squaresize)+floor((correctedy+halfheight)/squaresize)*gridwidth;
  for(auto& i : grid[grid_cell])
  {
    if(i->getPosition()[0]==correctedx && i->getPosition()[1]==correctedy)
    {
      drawparticle=false;
      break;
    }
  }

  if(drawparticle)
  {
    Particle newparticle;
    if(drawwall)
    {
      int oldm_todraw=m_todraw;
      m_todraw=0;
      newparticle= Particle(Vec3(correctedx,correctedy,-2.0f),&m_particleTypes[m_todraw]);
      newparticle.setWall(true);
      m_todraw=oldm_todraw;
    }
    else
    {
      newparticle= Particle(Vec3(correctedx,correctedy,-2.0f),&m_particleTypes[m_todraw]);
    }
    insertParticle(newparticle);
    hashParticles();
  }
}

void World::mouseDrag(int x, int y)
{
  std::cout<<x<<std::endl;
  if(m_previousmousex>0 && m_previousmousey>0)
  {
    float toaddx = (x-m_previousmousex)*((halfwidth*2)/(float)pixelwidth);
    float toaddy = (y-m_previousmousey)*((halfwidth*2)/(float)pixelwidth);

    for(auto& i : draggedParticles)
    {
      i->addPosition(Vec3(toaddx,-toaddy,0.0f));
      getbackhere(&(*i));
    }
    hashParticles();
  }
  m_previousmousex=x;
  m_previousmousey=y;
}

void World::selectDraggedParticles(int x, int y)
{
  float worldx = ((float)x/(float)pixelwidth)*(halfwidth*2) - halfwidth;
  float worldy = -((float)y/(float)pixelheight)*(halfheight*2) + halfheight;
  int grid_cell=floor((worldx+halfwidth)/squaresize)+floor((worldy+halfheight)/squaresize)*gridwidth;
  draggedParticles = getSurroundingParticles(grid_cell,2,true);

  int count = 0;
  for(auto& i : draggedParticles)
  {
    i->setDrag(true);
  }
  m_previousmousex=x;
  m_previousmousey=y;
}

void World::getbackhere(Particle * p)
{
  if(p->getPosition()[0]>halfwidth-0.5f) p->getPosition()[0]=halfwidth-0.5f;
  else if(p->getPosition()[0]<-halfwidth+0.5f) p->getPosition()[0]=-halfwidth+0.5f;
  if(p->getPosition()[1]>halfheight-0.5f) p->getPosition()[1]=halfheight-0.5f;
  else if(p->getPosition()[1]<-halfheight+0.5f) p->getPosition()[1]=-halfheight+0.5f;
}

void World::mouseDragEnd(int x, int y)
{
  Vec3 newVelocity = Vec3(x-m_previousmousex,m_previousmousey-y);

  for(auto& i : draggedParticles)
  {
    i->setDrag(false);
    i->addVelocity(newVelocity*0.05f);
  }
  draggedParticles.clear();
  m_previousmousex=-10;
  m_previousmousey=-10;
}

void World::handleKeys(char _input)
{
  switch(_input)
  {
  case '0' :
    drawWith(0);
    break;
  case '1' :
    drawWith(1);
    break;
  case 'w' :
    if(!m_3d)
    {
      if(drawwall) drawwall=false;
      else drawwall=true;
    }
    break;
  case 'r':
    if(renderoption==1) renderoption=2;
    else renderoption=1;
    break;
  case 'p':
    if(!m_3d)
    {
      resizeWindow(pixelwidth,pixelheight);
      if(renderoption==2) renderoption=1;
      m_camerarotatex=0.0f;
      m_camerarotatey=0.0f;
    }
    break;

  case 'o' :
    resizeWindow(pixelwidth,pixelheight);
    break;
  }
}

void World::mouseErase(int x, int y)
{
  hashParticles();
  float worldx = ((float)x/(float)pixelwidth)*(halfwidth*2) - halfwidth;
  float worldy = -((float)y/(float)pixelheight)*(halfheight*2) + halfheight;
  int grid_cell=floor((worldx+halfwidth)/squaresize)+floor((worldy+halfheight)/squaresize)*gridwidth;
  if (cellsContainingParticles[grid_cell])
  {
    bool thereisanobject=false;
    for(auto& i : grid[grid_cell])
    {
      if(!(i->isObject())) deleteParticle(i->getIndex());
      thereisanobject=true;
    }

  }
  m_previousmousex=x;
  m_previousmousey=y;
  defragParticles();
  defragSprings();
}

//------------------------PARTICLES FUNCTIONS-------------------------------------

void World::insertParticle(Particle particle)
{
  particles[firstFreeParticle]=particle;
  particles[firstFreeParticle].setIndex(firstFreeParticle);
  if(lastTakenParticle<firstFreeParticle)
  {
    ++lastTakenParticle;
    ++firstFreeParticle;
  }
  else{
    while(particles[firstFreeParticle].isAlive()==true)
    {
      ++firstFreeParticle;
    }
  }
  ++howManyAliveParticles;
}

void World::deleteParticle(int p)
{
  particles[p].setAlive(false);
  for(auto& i : particles[p].particleSprings)
  {
    deleteSpring(i);
  }

  if(lastTakenParticle==p)
  {
    while(particles[lastTakenParticle].isAlive()==false && lastTakenParticle>-1)
    {
      --lastTakenParticle;
    }
  }
  if(firstFreeParticle>p) firstFreeParticle=p;
  --howManyAliveParticles;
}

void World::defragParticles()
{
  for(int i=lastTakenParticle; i>firstFreeParticle; --i)
  {
    if(particles[i].isAlive())
    {
      for(auto& j : particles[i].particleSprings)
      {
        if(springs[j].indexi==i) springs[j].indexi=firstFreeParticle;
        else if(springs[j].indexj==i) springs[j].indexj=firstFreeParticle;
      }
      insertParticle(particles[i]);
      deleteParticle(i);
    }
  }
}

//-------------------------SPRING FUNCTIONS----------------------------------------

int World::insertSpring(Particle::Spring spring)
{
  int result = firstFreeSpring;
  springs[firstFreeSpring]=spring;
  if(lastTakenSpring<firstFreeSpring)
  {
    ++lastTakenSpring;
    ++firstFreeSpring;
  }
  else{
    while(springs[firstFreeSpring].alive)
    {
      ++firstFreeSpring;
    }
  }
  return result;
}

void World::deleteSpring(int s)
{
  springs[s].alive=false;
  particles[springs[s].indexi].updateSpringIndex(s,-1);
  particles[springs[s].indexj].updateSpringIndex(s,-1);
  if(lastTakenSpring==s)
  {
    while(springs[lastTakenSpring].alive==false && lastTakenSpring>-1)
    {
      --lastTakenSpring;
    }
  }
  if(firstFreeSpring>s) firstFreeSpring=s;
}

void World::defragSprings()
{
  for(int i=lastTakenSpring; i>firstFreeSpring; --i)
  {
    if(springs[i].alive)
    {
      particles[springs[i].indexi].updateSpringIndex(i,firstFreeSpring);
      particles[springs[i].indexj].updateSpringIndex(i,firstFreeSpring);
      insertSpring(springs[i]);
      deleteSpring(i);
    }
  }
}
//-------------------------GETTERS------------------------------

float World::getHalfHeight() const
{
  return halfheight;
}

float World::getHalfWidth() const
{
  return halfwidth;
}

void World::toggleRain()
{
  if(rain) rain=false;
  else rain=true;
}

void World::clearWorld()
{
  if(lastTakenParticle<0) lastTakenParticle=0;
  for(int i=0; i<lastTakenParticle+1; ++i)
  {
    deleteParticle(i);
  }

  hashParticles();

  //if(lastTakenSpring<0) lastTakenSpring=0;
  for(int i=0; i<lastTakenSpring+1; ++i)
  {
    deleteSpring(i);
  }
}

void World::toggleGravity()
{
  if(gravity) gravity=false;
  else gravity=true;
}

void World::drawWith(int type)
{
  if(type==0)
  {
    m_todraw=0;
  }
  else if(type==1)
  {
    //random.randomize();
    m_todraw=1;
    howmanytimesrandomized++;
  }
}

//--------------------------3D STUFF ------------------------------------------------

void World::mouseMove(const int &x, const int &y, bool leftclick, bool rightclick) {
  if(m_3d)
  {
    // only called when clicked
    float dx = (float)(x - m_previousmousex);
    float dy = (float)(y - m_previousmousey);

    if(leftclick)
    {
      m_camerarotatex+=dx*0.1f;
      m_camerarotatey+=dy*0.1f;
      std::cout<<m_camerarotatey<<std::endl;
    }

    else if(rightclick)
    {
      m_camerazoom+=dx*0.1f;
      //std::cout<<m_camerazoom<<std::endl;
    }

    m_previousmousex=x;
    m_previousmousey=y;
  }
}

std::vector<std::vector<float>> World::renderGrid(ParticleProperties *p)
{
  std::vector<std::vector<float>> rendergrid;
  rendergrid.clear();
  rendergrid.resize(renderheight+1);
  for(auto& i : rendergrid)
  {
    i.resize(renderwidth+1,0.0f);
  }

  float rendersquare=squaresize/renderresolution;
  for(int i=0; i<lastTakenParticle+1; ++i)
  {
    if(particles[i].isAlive()&&(particles[i].getProperties()==p))
    {
      Vec3 heightwidth = getGridColumnRow(particles[i].getGridPosition())*renderresolution;
      for(int x = -2*renderresolution; x<=4*renderresolution; ++x)
      {
        for(int y = -2*renderresolution; y<=4*renderresolution ; ++y)
        {
          int currentcolumn=heightwidth[0]+x;
          int currentrow=heightwidth[1]+y;

          if(currentcolumn<renderwidth && currentcolumn>0 &&
             currentrow<renderheight && currentrow>0)
          {
            float currentx = rendersquare*(float)currentcolumn - halfwidth;
            float currenty = rendersquare*(float)currentrow - halfheight;

            float metaballx = currentx-particles[i].getPosition()[0];
            float metabally = currenty-particles[i].getPosition()[1];

            float metaballfloat = (interactionradius*interactionradius)/(metaballx*metaballx + metabally*metabally);

            rendergrid[currentrow][currentcolumn]+=metaballfloat;
          }
        }
      }
    }
  }
  return rendergrid;
}

void World::drawMarchingSquares(std::vector<std::vector<float>> renderGrid, ParticleProperties p, bool inner)
{
  float red = p.getRed();
  float green = p.getGreen();
  float blue = p.getBlue();

  //std::cout<<"BLUE"<<blue<<std::endl;

  float renderthreshold = mainrenderthreshold;
  if(inner)
  {
    renderthreshold=0.7f*renderthreshold;
    red+=0.4;
    green+=0.4;
    blue+=0.4;
  }

  float rendersquare=squaresize/renderresolution;

  for(int currentrow=0; currentrow<renderheight; ++currentrow)
  {
    for(int currentcolumn=0; currentcolumn<renderwidth; ++currentcolumn)
    {

      //1---5---2
      //|       |
      //8       6
      //|       |
      //3---7---4


      std::vector<bool> boolpoints;
      boolpoints.push_back(renderGrid[currentrow][currentcolumn]>renderthreshold);
      boolpoints.push_back(renderGrid[currentrow][currentcolumn+1]>renderthreshold);
      boolpoints.push_back(renderGrid[currentrow+1][currentcolumn]>renderthreshold);
      boolpoints.push_back(renderGrid[currentrow+1][currentcolumn+1]>renderthreshold);

      bool empty=false;

      if(!boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&!boolpoints[3])
      {
        empty=true;
      }

      if(!empty)
      {
        float p1x = (squaresize/renderresolution)*(float)currentcolumn - halfwidth;
        float p1y = (squaresize/renderresolution)*(float)currentrow - halfheight;

        float p2x = (squaresize/renderresolution)*((float)currentcolumn+1.0f) - halfwidth;
        float p2y = p1y;

        float p3x = p1x;
        float p3y = (squaresize/renderresolution)*((float)currentrow+1.0f) - halfheight;

        float p4x = p2x;
        float p4y = p3y;

        float p5x = (p1x+p2x)/2.0f;
        float p5y = p1y;

        float p6x = p2x;
        float p6y = (p2y+p4y)/2.0f;

        float p7x = p5x;
        float p7y = p3y;

        float p8x = p1x;
        float p8y = p6y;

        if(boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //1111 TICK
        {
          glBegin(GL_QUADS);
          glColor3f(red,green,blue);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);
          glEnd();
        }
        else if(!boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //0001 TICK
        {
          p6y=p2y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow][currentcolumn+1]));
          p7x=p3x+rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glEnd();
        }
        else if(!boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //0010 TICK
        {
          p8y=p1y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow][currentcolumn]));
          p7x=p4x-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow+1][currentcolumn+1]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);
          glEnd();
        }
        else if(!boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //0011 TICK
        {
          p8y=p1y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow][currentcolumn]));
          p6y=p2y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow][currentcolumn+1]));
          glBegin(GL_QUADS);
          glColor3f(red,green,blue);
          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);
          glEnd();
        }
        else if(!boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //0100 TICK
        {
          p6y=p4y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn+1]));
          p5x=p1x+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow][currentcolumn]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glEnd();
        }
        else if(!boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //0101 TICK
        {
          p5x=p1x+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow][currentcolumn]));
          p7x=p3x+rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn]));

          glBegin(GL_QUADS);
          glColor3f(red,green,blue);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glEnd();
        }
        else if(!boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //0110 COULD CHANGE TO SEE
        {
          p5x=p1x+(p2x-p1x)*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow][currentcolumn]));
          p6y=p4y+(p2y-p4y)*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn+1]));
          p7x=p4x+(p3x-p4x)*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow+1][currentcolumn+1]));
          p8y=p1y+(p3y-p1y)*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow][currentcolumn]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          //glColor3f(1.0f,0.0f,0.0f);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);

          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p8x,p8y,-2.0f);

          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);

          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);

          glEnd();
        }
        else if(!boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //0111 TICK
        {
          p5x=p1x+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow][currentcolumn]));
          p8y=p1y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow][currentcolumn]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);
          glEnd();
        }
        else if(boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //1000 TICK
        {
          p5x=p2x-rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow][currentcolumn+1]));
          p8y=p3y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow+1][currentcolumn]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p8x,p8y,-2.0f);
          glEnd();
        }
        else if(boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //1001 COULD CHANGE TO SEE
        {
          p5x=p2x+(p1x-p2x)*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow][currentcolumn+1]));
          p6y=p2y+(p4y-p2y)*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow][currentcolumn+1]));
          p7x=p3x+(p4x-p3x)*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn]));
          p8y=p3y+(p1y-p3y)*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow+1][currentcolumn]));


          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);


          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p8x,p8y,-2.0f);

          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);

          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);

          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);

          glEnd();
        }
        else if(boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //1010 TICK
        {
          p5x=p2x-rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow][currentcolumn+1]));
          p7x=p4x-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow+1][currentcolumn+1]));

          glBegin(GL_QUADS);
          glColor3f(red,green,blue);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);
          glEnd();
        }
        else if(boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //1011 TICK
        {
          p5x=p2x-rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow][currentcolumn+1]));
          p6y=p2y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow][currentcolumn+1]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);
          glVertex3f(p5x,p5y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glEnd();
        }
        else if(boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //1100 TICK
        {
          p6y=p4y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn+1]));
          p8y=p3y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow+1][currentcolumn]));

          glBegin(GL_QUADS);
          glColor3f(red,green,blue);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p8x,p8y,-2.0f);
          glEnd();
        }
        else if(boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //1101 TICK
        {
          p7x=p3x+rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn]));
          p8y=p3y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow+1][currentcolumn]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glVertex3f(p8x,p8y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glVertex3f(p4x,p4y,-2.0f);
          glEnd();
        }
        else if(boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //1110
        {
          p7x=p4x-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow+1][currentcolumn+1]));
          p6y=p4y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn+1]));

          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p2x,p2y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p6x,p6y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glVertex3f(p1x,p1y,-2.0f);
          glVertex3f(p7x,p7y,-2.0f);
          glVertex3f(p3x,p3y,-2.0f);

          glEnd();
        }
      }
    }
  }
}

void World::set3D(bool b)
{
  m_3d=b;
}

void World::setToDraw(int _todraw)
{
  if(_todraw<m_particleTypes.size()) m_todraw=_todraw;
}

void World::setRandomType(int _randomSeed)
{
  m_particleTypes[3].randomize(_randomSeed);
  m_particleTypes[3].printVariables();
}

void World::drawMarchingCubes(std::vector<std::vector<std::vector<float>>> renderGrid, ParticleProperties p)
{

  //std::cout<<"HELLO THEREBABY"<<std::endl;
  float red = p.getRed();
  float green = p.getGreen();
  float blue = p.getBlue();

  int edgeTable[256]={
  0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
  0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
  0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
  0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
  0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
  0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
  0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
  0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
  0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
  0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
  0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
  0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
  0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
  0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
  0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
  0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
  0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
  0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
  0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
  0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
  0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
  0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
  0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
  0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
  0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
  0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
  0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
  0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
  0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
  0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
  0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
  0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };
  int triTable[256][16] =
  {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
  {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
  {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
  {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
  {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
  {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
  {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
  {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
  {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
  {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
  {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
  {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
  {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
  {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
  {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
  {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
  {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
  {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
  {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
  {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
  {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
  {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
  {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
  {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
  {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
  {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
  {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
  {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
  {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
  {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
  {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
  {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
  {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
  {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
  {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
  {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
  {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
  {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
  {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
  {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
  {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
  {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
  {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
  {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
  {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
  {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
  {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
  {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
  {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
  {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
  {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
  {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
  {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
  {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
  {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
  {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
  {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
  {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
  {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
  {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
  {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
  {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
  {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
  {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
  {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
  {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
  {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
  {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
  {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
  {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
  {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
  {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
  {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
  {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
  {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
  {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
  {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
  {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
  {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
  {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
  {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
  {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
  {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
  {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
  {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
  {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
  {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
  {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
  {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
  {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
  {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
  {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
  {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
  {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
  {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
  {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
  {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
  {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
  {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
  {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
  {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
  {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
  {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
  {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
  {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
  {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
  {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
  {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
  {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
  {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
  {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
  {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
  {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
  {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
  {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
  {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
  {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
  {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
  {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
  {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
  {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
  {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
  {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
  {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
  {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
  {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
  {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
  {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
  {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
  {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
  {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
  {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
  {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
  {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
  {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
  {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
  {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
  {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
  {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
  {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
  {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
  {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
  {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
  {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
  {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
  {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
  {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
  {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
  {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
  {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
  {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
  {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
  {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
  {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
  {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
  {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
  {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
  {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
  {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
  {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
  {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
  {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
  {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
  {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
  {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
  {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
  {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
  {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
  {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
  {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
  {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
  {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
  {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
  {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
  {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
  {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
  {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
  {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
  {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
  {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

  int render3dheight=renderGrid.size();
  int render3dwidth=renderGrid[0].size();
  int render3ddepth=renderGrid[0][0].size();
  float isolevel=mainrenderthreshold;

  for(int h=0; h<render3dheight-1; ++h)
  {
    for(int w=0; w<render3dwidth-1; ++w)
    {
      for(int d=0; d<render3ddepth-1; ++d)
      {
        std::vector<float> gridvalue;
        if(renderGrid[h][w][d]>0) std::cout<<"DEFO"<<renderGrid[h][w][d]<<std::endl;
        gridvalue.push_back(renderGrid[h+1][w][d+1]);   //0
        gridvalue.push_back(renderGrid[h+1][w+1][d+1]); //1
        gridvalue.push_back(renderGrid[h+1][w+1][d]);   //2
        gridvalue.push_back(renderGrid[h+1][w][d]);     //3
        gridvalue.push_back(renderGrid[h][w][d+1]);     //4
        gridvalue.push_back(renderGrid[h][w+1][d+1]);   //5
        gridvalue.push_back(renderGrid[h][w+1][d]);     //6
        gridvalue.push_back(renderGrid[h][w][d]);       //7

        for(int poo =0; poo<8 ; ++poo)
        {
          if(gridvalue[poo]>0) std::cout<<gridvalue[poo]<<std::endl;
        }

        float rendersquare=squaresize/renderresolution;
        float hWorld = halfheight - h * rendersquare ;
        float wWorld = - halfwidth + w * rendersquare;
        float dWorld = - 2 - halfwidth + d * rendersquare;

        std::vector<Vec3> gridposition;
        gridposition.push_back(Vec3(hWorld+squaresize,wWorld,dWorld+squaresize));   //0
        gridposition.push_back(Vec3(hWorld+squaresize,wWorld+squaresize,dWorld+squaresize)); //1
        gridposition.push_back(Vec3(hWorld+squaresize,wWorld+squaresize,dWorld));   //2
        gridposition.push_back(Vec3(hWorld+squaresize,wWorld,dWorld));     //3
        gridposition.push_back(Vec3(hWorld,wWorld,dWorld+squaresize));     //4
        gridposition.push_back(Vec3(hWorld,wWorld+squaresize,dWorld+squaresize));   //5
        gridposition.push_back(Vec3(hWorld,wWorld+squaresize,dWorld));     //6
        gridposition.push_back(Vec3(hWorld,wWorld,dWorld));       //7

        Vec3 vertlist[12];

        int cubeindex = 0;
        if (gridvalue[0] < isolevel) cubeindex |= 1;
        if (gridvalue[1] < isolevel) cubeindex |= 2;
        if (gridvalue[2] < isolevel) cubeindex |= 4;
        if (gridvalue[3] < isolevel) cubeindex |= 8;
        if (gridvalue[4] < isolevel) cubeindex |= 16;
        if (gridvalue[5] < isolevel) cubeindex |= 32;
        if (gridvalue[6] < isolevel) cubeindex |= 64;
        if (gridvalue[7] < isolevel) cubeindex |= 128;

        /* Cube is entirely in/out of the surface */
        if (edgeTable[cubeindex] == 0)
        {
          return;
        }

        /* Find the vertices where the surface intersects the cube */
        if (edgeTable[cubeindex] & 1)
          vertlist[0] =
              VertexInterp(gridposition[0],gridposition[1],gridvalue[0],gridvalue[1]);
        if (edgeTable[cubeindex] & 2)
          vertlist[1] =
              VertexInterp(gridposition[1],gridposition[2],gridvalue[1],gridvalue[2]);
        if (edgeTable[cubeindex] & 4)
          vertlist[2] =
              VertexInterp(gridposition[2],gridposition[3],gridvalue[2],gridvalue[3]);
        if (edgeTable[cubeindex] & 8)
          vertlist[3] =
              VertexInterp(gridposition[3],gridposition[0],gridvalue[3],gridvalue[0]);
        if (edgeTable[cubeindex] & 16)
          vertlist[4] =
              VertexInterp(gridposition[4],gridposition[5],gridvalue[4],gridvalue[5]);
        if (edgeTable[cubeindex] & 32)
          vertlist[5] =
              VertexInterp(gridposition[5],gridposition[6],gridvalue[5],gridvalue[6]);
        if (edgeTable[cubeindex] & 64)
          vertlist[6] =
              VertexInterp(gridposition[6],gridposition[7],gridvalue[6],gridvalue[7]);
        if (edgeTable[cubeindex] & 128)
          vertlist[7] =
              VertexInterp(gridposition[7],gridposition[4],gridvalue[7],gridvalue[4]);
        if (edgeTable[cubeindex] & 256)
          vertlist[8] =
              VertexInterp(gridposition[0],gridposition[4],gridvalue[0],gridvalue[4]);
        if (edgeTable[cubeindex] & 512)
          vertlist[9] =
              VertexInterp(gridposition[1],gridposition[5],gridvalue[1],gridvalue[5]);
        if (edgeTable[cubeindex] & 1024)
          vertlist[10] =
              VertexInterp(gridposition[2],gridposition[6],gridvalue[2],gridvalue[6]);
        if (edgeTable[cubeindex] & 2048)
          vertlist[11] =
              VertexInterp(gridposition[3],gridposition[7],gridvalue[3],gridvalue[7]);

        for (int i=0;triTable[cubeindex][i]!=-1;i+=3)
        {
          glBegin(GL_TRIANGLES);
          glColor3f(red,green,blue);
          glVertex3f(vertlist[triTable[cubeindex][i  ]][0],vertlist[triTable[cubeindex][i  ]][1],vertlist[triTable[cubeindex][i  ]][2]);
          glVertex3f(vertlist[triTable[cubeindex][i+1]][0],vertlist[triTable[cubeindex][i+1]][1],vertlist[triTable[cubeindex][i+1]][2]);
          glVertex3f(vertlist[triTable[cubeindex][i+2]][0],vertlist[triTable[cubeindex][i+2]][1],vertlist[triTable[cubeindex][i+2]][2]);
          glEnd();
          //std::cout<<"I'M HERE"<<std::endl;
        }
      }
    }
  }
}

// Aren't Grammars the way to go? i dunno

Vec3 World::VertexInterp(Vec3 p1, Vec3 p2, float valp1, float valp2)
{
    
  float isolevel = mainrenderthreshold;

  double mu;
  Vec3 p = Vec3();

   if (std::abs(isolevel-valp1) < 0.00001)
      return p1;
   if (std::abs(isolevel-valp2) < 0.00001)
      return p2 ;
   if (std::abs(valp1-valp2) < 0.00001)
      return p1 ;
   mu = (isolevel - valp1) / (valp2 - valp1);
   p[0] = p1[0] + mu * (p2[0] - p1[0]);
   p[1] = p1[1] + mu * (p2[1] - p1[1]);
   p[2] = p1[2] + mu * (p2[2] - p1[2]);

   return p;
}

std::vector<std::vector<std::vector<float>>> World::render3dGrid(ParticleProperties *p)
{
  std::vector<std::vector<std::vector<float>>> rendergrid;
  rendergrid.clear();
  rendergrid.resize(renderheight+1);
  for(auto& i : rendergrid)
  {
    i.resize(renderwidth+1);
    for(auto& j : i)
    {
      j.resize(renderwidth+1,0.0f);
    }
  }

  float rendersquare=squaresize/renderresolution;

  for(int i=0; i<lastTakenParticle+1; ++i)
  {
    if(particles[i].isAlive()&&(particles[i].getProperties()==p))
    {
      Vec3 heightwidthdepth = getGridColumnRowDepth(particles[i].getGridPosition())*renderresolution; // 3Dify this

      for(int x = -2*renderresolution; x<=4*renderresolution; ++x)
      {
        for(int y = -2*renderresolution; y<=4*renderresolution ; ++y)
        {
          for(int z = -2*renderresolution; z<=4*renderresolution ; ++z)
          {

            //std::cout<<"hello"<<std::endl;
            int currentcolumn=heightwidthdepth[0]+x;
            int currentrow=heightwidthdepth[1]+y;
            int currentdepth=heightwidthdepth[2]+z;

            //std::cout<<currentcolumn<<","<<currentrow<<","<<currentdepth<<std::endl;

            //std::cout<<"column"<<currentcolumn<<","<<renderwidth<<std::endl;
            //std::cout<<"row"<<currentrow<<","<<renderheight<<std::endl;
            //std::cout<<"depth"<<currentdepth<<","<<renderwidth<<std::endl;

            if(currentcolumn<renderwidth && currentcolumn>0 &&
               currentrow<renderheight && currentrow>0 &&
               currentdepth<renderwidth && currentdepth>0)
            {
              float currentx = rendersquare*(float)currentcolumn - halfwidth;
              float currenty = rendersquare*(float)currentrow - halfheight;
              float currentz = rendersquare*(float)currentdepth - 2 - halfwidth;

              float metaballx = currentx-particles[i].getPosition()[0];
              float metabally = currenty-particles[i].getPosition()[1];
              float metaballz = currentz-particles[i].getPosition()[2];

              float metaballfloat = 10/(metaballx*metaballx + metabally*metabally + metaballz*metaballz);
              //std::cout<<metaballfloat<<std::endl;
              rendergrid[currentrow][currentcolumn][currentdepth]+=metaballfloat;
            }
          }
        }
      }
    }
  }

  /*

  for(auto& i : rendergrid)
  {
    for(auto& j : i)
    {
      for(auto& k : j)
      {
        if(k>0) std::cout<<k<<std::endl;
      }
    }
  }
  // */

  return rendergrid;
}

Vec3 World::getGridColumnRowDepth(int k) // CHECK THIS
{
    int z = floor(k/(gridwidth*gridheight));
    int y = floor((k - z*gridwidth*gridheight)/gridwidth);
    int x = k - y*gridwidth - z*gridwidth*gridheight;

    return Vec3(x,y,z);
}
