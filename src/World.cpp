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
  pointsize(20.0f),
  renderthreshold(800.0f),
  renderresolution(10),
  renderoption(1),
  rain(false),
  drawwall(false),
  gravity(true),
  springsize(500000),
  particlesPoolSize(3000),
  m_3d(false)
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

    // DEFAULT PARTICLE PROPERTIES
    water=ParticleProperties();
    //water=ParticleProperties(true, 0.6f,0.8f,0.4,0.8f,0.01f,0.004,0.3,10.0f,0.5f,0.27f,0.07f,false);
    //water=ParticleProperties(false,0.0175,0.3472,0.0004,0.3,0.007336,0.0038962,0.3,2.368,0.1f,0.5,0.8f,true);
    poo=ParticleProperties(false,0.3f,0.2f,0.004f,0.3f,0.01f,0.004f,0.3f,10.0f,0.8f,0.52f,0.25f,false);
    random=ParticleProperties();
    random.randomize();
    todraw=&water; // This is the liquid to draw (tap or mouse)

    // create start two blocks of particles
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*0.1f,3.0f-j*0.1f,-2.0f),todraw));
        //particles.back().setIsObject();
      }
    }

/*
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
        particles.push_back(Particle(Vec3(3.0f+i*0.2f,3.0f-j*0.2f)));
    }
    for(auto& i : particles)
      i.addVelocity(Vec3(((float)(rand() % 100 - 50)*0.001f),((float)(rand() % 10000 - 50))*0.001f));
*/


    m_previousmousex=-10;
    m_previousmousey=-10;

    m_camerarotatey=0.0f;
    m_camerarotatex=0.0f;

    Particle::Spring defaultspring;
    defaultspring.alive=false;
    springs.resize(springsize,defaultspring);
    firstFreeSpring=0;


    // FUN PARTICLE TYPES



    //Any other glTex* stuff here

    m_isInit = true;
}

/**
 * @brief World::resize needs to set up the camera paramaters (i.e. projection matrix) and the viewport
 * @param w Width of window
 * @param h Height of window
 */
void World::resize(int w, int h) {
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

  hashParticles();

  //-------------MARCHING SQUARES NONSENSE-----------------------------

  renderwidth=gridwidth*renderresolution;
  renderheight=gridheight*renderresolution;

  renderGrid.clear();
  renderGrid.resize(renderheight+1);
  for(auto& i : renderGrid)
  {
    i.resize(renderwidth+1,0.0f);
  }
}

/**
 * @brief World::draw draws the World to the current GL context. Called a lot - make this fast!
 */
void World::draw() {
    if (!m_isInit) return;

    glMatrixMode(GL_MODELVIEW);

    if(m_3d)
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
      for(auto& i : particles){
        i.drawParticle();
      }
    }

    else if(renderoption==2)
    {
      for(auto& i : particles)
      {
        Vec3 heightwidth = getGridColumnRow(i.getGridPosition())*renderresolution;
        for(int x = -1*renderresolution; x<=2*renderresolution; ++x)
        {
          for(int y = -1*renderresolution; y<=2*renderresolution ; ++y)
          {
            int currentcolumn=heightwidth[0]+x;
            int currentrow=heightwidth[1]+y;

            if(currentcolumn<renderwidth && currentcolumn>0 &&
               currentrow<renderheight && currentrow>0)
            {
              float currentx = squaresize/renderresolution*(float)currentcolumn - halfwidth;
              float currenty = squaresize/renderresolution*(float)currentrow - halfheight;

              float metaballx = currentx-i.getPosition()[0];
              float metabally = currenty-i.getPosition()[1];

              float metaballfloat = 2.0f/(metaballx*metaballx + metabally*metabally);

              renderGrid[currentrow][currentcolumn]+=metaballfloat;
            }
          }
        }
      }

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

            if(boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //1111
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p4x,p4y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
            }
            else if(!boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //0001
            {
              glBegin(GL_TRIANGLES);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p6x,p6y,-2.0f);
              glVertex3f(p7x,p7y,-2.0f);
              glVertex3f(p4x,p4y,-2.0f);
              glEnd();
            }
            else if(!boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //0010
            {
              glBegin(GL_TRIANGLES);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p8x,p8y,-2.0f);
              glVertex3f(p7x,p7y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
            }
            else if(!boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //0011
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p8x,p8y,-2.0f);
              glVertex3f(p6x,p6y,-2.0f);
              glVertex3f(p4x,p4y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
            }
            else if(!boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //0100
            {
              glBegin(GL_TRIANGLES);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p5x,p5y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p6x,p6y,-2.0f);
              glEnd();
            }
            else if(!boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //0101
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p5x,p5y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p4x,p4y,-2.0f);
              glVertex3f(p7x,p7y,-2.0f);
              glEnd();
            }
            else if(!boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //0110
            {
              glBegin(GL_TRIANGLES);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p5x,p5y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p6x,p6y,-2.0f);
              glVertex3f(p8x,p8y,-2.0f);
              glVertex3f(p7x,p7y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
            }
            else if(!boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //0111
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p4x,p4y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
              glBegin(GL_TRIANGLES);
              glColor3f(0.0f,0.0f,0.0f);
              glVertex3f(p1x,p1y,-1.5f);
              glVertex3f(p5x,p5y,-1.5f);
              glVertex3f(p8x,p8y,-1.5f);
              glEnd();
            }
            else if(boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //1000
            {
              glBegin(GL_TRIANGLES);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-1.5f);
              glVertex3f(p5x,p5y,-1.5f);
              glVertex3f(p8x,p8y,-1.5f);
              glEnd();
            }
            else if(boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //1001
            {
              glBegin(GL_TRIANGLES);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-1.5f);
              glVertex3f(p5x,p5y,-1.5f);
              glVertex3f(p8x,p8y,-1.5f);
              glVertex3f(p7x,p7y,-1.5f);
              glVertex3f(p6x,p6y,-1.5f);
              glVertex3f(p4x,p4y,-1.5f);
              glEnd();
            }
            else if(boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //1010
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-2.0f);
              glVertex3f(p5x,p5y,-2.0f);
              glVertex3f(p7x,p7y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
            }
            else if(boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //1011
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p4x,p4y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
              glBegin(GL_TRIANGLES);
              glColor3f(0.0f,0.0f,0.0f);
              glVertex3f(p2x,p2y,-1.5f);
              glVertex3f(p6x,p6y,-1.5f);
              glVertex3f(p5x,p5y,-1.5f);
              glEnd();
            }
            else if(boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //1100
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p6x,p6y,-2.0f);
              glVertex3f(p8x,p8y,-2.0f);
              glEnd();
            }
            else if(boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //1101
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p4x,p4y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
              glBegin(GL_TRIANGLES);
              glColor3f(0.0f,0.0f,0.0f);
              glVertex3f(p3x,p3y,-1.5f);
              glVertex3f(p8x,p8y,-1.5f);
              glVertex3f(p7x,p7y,-1.5f);
              glEnd();
            }
            else if(boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //1110
            {
              glBegin(GL_QUADS);
              glColor3f(0.5f,1.0f,0.831f);
              glVertex3f(p1x,p1y,-2.0f);
              glVertex3f(p2x,p2y,-2.0f);
              glVertex3f(p4x,p4y,-2.0f);
              glVertex3f(p3x,p3y,-2.0f);
              glEnd();
              glBegin(GL_TRIANGLES);
              glColor3f(0.0f,0.0f,0.0f);
              glVertex3f(p6x,p6y,-1.5f);
              glVertex3f(p4x,p4y,-1.5f);
              glVertex3f(p7x,p7y,-1.5f);
              glEnd();
            }
          }
        }

      }

      for(auto& i : renderGrid)
      {
        i.assign(renderwidth+1,0.0f);
      }
    }

    if(m_3d) glPopMatrix();
}


/**
 * @brief World::update updates the World based on a timer. Used for animation.
 */
void World::update() {
    if (!m_isInit) return;

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
      if(everyother%4==0){
        if(!m_3d)
        {
          for(int i = 0; i<5; ++i)
          {
            particles.push_back(Particle(Vec3(-3.0f+i*0.2f,halfheight-2.5,-2.0f),todraw));
            particles.back().addVelocity(Vec3(0.0f,-50.0f,0.0f));
          }
        }
        else
        {
          for(int j = 0; j< 5; ++j)
          {
            for(int i = 0; i<5; ++i)
            {
              particles.push_back(Particle(Vec3(-3.0f+i*0.2f,halfheight-2.5,-3.0f+j*0.2f),todraw));
              particles.back().addVelocity(Vec3(0.0f,-50.0f,0.0f));
            }
          }
        }
      }
    }

    // ------------------------------GRAVITY --------------------------------------------
    if(gravity)
    {
      Vec3 gravityvel = Vec3(0.0f,-0.008*m_timestep,0.0f);
      gravityvel.rotateAroundXAxisf(-m_camerarotatey*(M_PI/180.0f));

      for(auto& i : particles)
      {
        i.addVelocity(gravityvel);
      }
    }

    // ------------------------------VISCOSITY--------------------------------------------
    // TODO : Implement spatial hash for loop here


    int choo = 0;

    for(auto& k : grid)
    {
      int ploo = 0;
      for(auto& i : k)
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

    for(auto& i : particles)
    {
      i.updatePrevPosition();
      if(!(i.getDrag())&&!(i.getWall())) i.updatePosition(m_timestep);
    }
    hashParticles();

    //--------------------------------------SPRING ALGORITMNS-----------------------------------------------

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
                    if(((springs[spring].indexi==i) && (springs[spring].indexj==j)) ||
                       ((springs[spring].indexi==j) && (springs[spring].indexj==i)))
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
                    newspring.indexi=i;
                    newspring.indexj=j;
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
        Vec3 rij = (*(i.indexj)).getPosition() - (*(i.indexi)).getPosition();
        float rijmag = rij.length();

        if(rijmag>interactionradius && !i.indexi->isObject())
        {
          deleteSpring(count);
        }

        else{
          rij.normalize();
          Vec3 D = rij*m_timestep*m_timestep*i.indexi->getProperties()->getKspring()*(1-(i.L/interactionradius))*(i.L-rijmag);
          i.indexi->addPosition(-D/2);
          i.indexj->addPosition(D/2);
        }

      }
      count++;
    }
    // */

    //----------------------------------DOUBLEDENSITY------------------------------------------

    count =0;
    for(auto& list : grid)
    {
      std::vector<Particle *> neighbours=getSurroundingParticles(count,1,false);
      for(auto& i : list)
      {
        float density =0;
        float neardensity=0;
        for(auto& j : neighbours)
        {
          Vec3 rij = j->getPosition()-i->getPosition(); // THFP
          float rijmag = rij.length();
          float q = rijmag/interactionradius;
          if(q<1 && q!=0)
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

    for (auto& it : particles)
    {

      if(it.getPosition()[1]-0.5f<-halfheight)
      {
        it.setPosition(Vec3(it.getPosition()[0],-halfheight+0.5f,it.getPosition()[2]));
        it.setVelocity(Vec3(0.5f*it.getVelocity()[0],-0.5f*it.getVelocity()[1],0.0f));
      }

      if(it.getPosition()[1]+1.5f>halfheight)
      {
        it.setPosition(Vec3(it.getPosition()[0],halfheight-1.5f,it.getPosition()[2]));
        it.addVelocity(Vec3(0.0f,-0.8f*it.getVelocity()[1],0.0f));
      }

      if(it.getPosition()[0]>(halfwidth-0.5f)*smallen)
      {
        it.setPosition(Vec3(smallen*(halfwidth-0.5f),it.getPosition()[1],it.getPosition()[2]));
        it.addVelocity(Vec3(-0.8f*it.getVelocity()[0],0.0f));
      }

      if(it.getPosition()[0]<(-halfwidth+0.5f)*smallen)
      {
        it.setPosition(Vec3(smallen*(-halfwidth+0.5f),it.getPosition()[1],it.getPosition()[2]));
        it.addVelocity(Vec3(-0.8f*it.getVelocity()[0],0.0f));
      }

      if(it.getPosition()[2]<-2-(halfwidth+0.5f)*smallen)
      {
        it.setPosition(Vec3(it.getPosition()[0],it.getPosition()[1],-2-(halfwidth+0.5)*smallen));
        it.addVelocity(Vec3(0.0f,0.0f,-0.8f*it.getVelocity()[2]));
      }

      if(it.getPosition()[2]>-2+(halfwidth-0.5f)*smallen)
      {
        it.setPosition(Vec3(it.getPosition()[0],it.getPosition()[1],-2+(halfwidth-0.5f)*smallen));
        it.addVelocity(Vec3(0.0f,0.0f,-0.8f*it.getVelocity()[2]));
      }
    }
    //----------------------------------CLEANUP ------------------------------------------------

    if(everyother%30==0)
    {
      std::cout<<"Numebr:"<<particles.size()<<std::endl;
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
  for(auto& i : particles)
  {
    grid_cell=
        floor((i.getPosition()[0]+halfwidth)/squaresize)+
        floor((i.getPosition()[1]+halfheight)/squaresize)*gridwidth;

    if(m_3d) grid_cell+=floor((i.getPosition()[2]+halfwidth)/squaresize)*gridwidth*gridheight;

    i.setGridPosition(grid_cell);

    if(grid_cell>=0 && grid_cell<gridSize)
    {
      cellsContainingParticles[grid_cell]=true;
      grid[grid_cell].push_back(&i);
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

Vec3 World::getRenderGridColumnRow(int k)
{
  int row = floor(k/((gridwidth+1)*2));
  int column = k - row*((gridwidth+1)*2);
  return Vec3(column, row);
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
      particles.push_back(Particle(Vec3(correctedx,correctedy,-2.0f),&water));
      if(drawwall) particles.back().setWall(true);
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

void World::handleKeys(char i)
{
  if(i=='i') // inflow
  {
    if(rain) rain=false;
    else rain=true;
  }
  else if(i=='g')
  {
    if(gravity) gravity=false;
    else gravity=true;
  }
  else if(i=='0')
  {
    drawWith(0);
  }
  else if(i=='1')
  {
    drawWith(1);
  }

  if(!m_3d)
  {
     if (i=='w')
    {
      if(drawwall) drawwall=false;
      else drawwall=true;
    }
    else if(i=='r')
    {
      if(renderoption==1) renderoption=2;
      else renderoption=1;
    }

    else if(i=='p')
    {
      clearWorld();
      m_3d = true;
      resize(pixelwidth,pixelheight);
      if(renderoption==2) renderoption=1;
      m_camerarotatex=0.0f;
      m_camerarotatey=0.0f;
    }
  }
  else if(i=='o')
  {
    clearWorld();
    m_3d = false;
    resize(pixelwidth,pixelheight);
  }

}

void World::mouseErase(int x, int y)
{
  float worldx = ((float)x/(float)pixelwidth)*(halfwidth*2) - halfwidth;
  float worldy = -((float)y/(float)pixelheight)*(halfheight*2) + halfheight;
  int grid_cell=floor((worldx+halfwidth)/squaresize)+floor((worldy+halfheight)/squaresize)*gridwidth;
  if (cellsContainingParticles[grid_cell])
  {
    bool thereisanobject=false;
    for(auto& i : grid[grid_cell])
    {
      if(!(i->isObject())) i->setDead();
      thereisanobject=true;
    }
    std::list<Particle>::iterator i = particles.begin();
    while (i != particles.end())
    {
      if (i->isDead())
      {
        particles.erase(i++);  // alternatively, i = items.erase(i);
      }
      else
      {
        ++i;
      }
    }
    hashParticles();
  }
  m_previousmousex=x;
  m_previousmousey=y;
}

//-------------------------SPRING FUNCTIONS----------------------------------------

int World::insertSpring(Particle::Spring spring)
{
  for(int i = firstFreeSpring; i< springsize; ++i)
  {
    if(!(springs[i].alive))
    {
      springs[i] = spring;
      firstFreeSpring=i+1;
      return i;
    }
  }
}

void World::deleteSpring(int s)
{
  springs[s].alive=false;
  if(s+1<firstFreeSpring) firstFreeSpring=s+1;
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
  particles.clear();
  hashParticles();

  Particle::Spring defaultspring;
  defaultspring.alive=false;
  springs.clear();
  springs.resize(springsize,defaultspring);
  firstFreeSpring=0;
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
    todraw=&water;
  }
  else if(type==1)
  {
    random.randomize();
    todraw=&random;
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
