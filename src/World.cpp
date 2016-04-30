#include "include/World.h"

///
///  @file Commands.cpp
///  @brief Command objects to be executed inside timerCallback in main.



/**
 * @brief World::World
 */
World::World() :
  m_isInit(false),
  m_startTime(0.0),
  m_elapsedTime(0.0),
  interactionradius(1.0f),  //1
  squaresize(1.0f), //1
  m_timestep(1.0f),
  pointsize(10.0f), //10
  m_mainrender2dthreshold(90.0f),  //90
  m_mainrender3dthreshold(90.0f),
  renderresolution(4),
  render3dresolution(2),
  renderoption(1),
  rain(false),
  drawwall(false),
  gravity(true),
  m_springsize(500000),
  m_particlesPoolSize(5000),
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

  m_particles.clear();
  springs.clear();

  Particle defaultparticle;
  defaultparticle.setAlive(false);
  m_particles.resize(m_particlesPoolSize,defaultparticle);
  m_firstFreeParticle=0;
  m_lastTakenParticle=-1;
  howManyAliveParticles=0;

  Particle::Spring defaultspring;
  defaultspring.alive=false;
  springs.resize(m_springsize,defaultspring);
  m_firstFreeSpring=0;
  m_lastTakenSpring=-1;

  // DEFAULT PARTICLE PROPERTIES
  m_particleTypes.push_back(ParticleProperties()); //water
  m_particleTypes.push_back(ParticleProperties(true,0.3f,0.2f,0.004f,0.2f,0.01f,0.004f,0.3f,10.0f,0.8f,0.52f,0.25f,false)); //slime
  m_particleTypes.push_back(ParticleProperties(false,0.3f,0.2f,0.004f,0.01f,0.01f,0.004f,0.3f,10.0f,0.8f,0.52f,0.25f,false)); //blobby
  m_particleTypes.push_back(ParticleProperties()); //random
  m_particleTypes.push_back(ParticleProperties(true,0.3f,0.2f,0.004f,0.2f,0.01f,0.004f,0.7f,10.0f,0.8f,0.52f,0.25f,false)); // cube
                                              //     sig, bet, gamma,alph, knear, k,   kspri,p0,  red,  green,  blue

  //water=ParticleProperties(true, 0.6f,0.8f,0.4,0.8f,0.01f,0.004,0.3,10.0f,0.5f,0.27f,0.07f,false);
  //water=ParticleProperties(false,0.0175,0.3472,0.0004,0.3,0.007336,0.0038962,0.3,2.368,0.1f,0.5,0.8f,true);

  //random=ParticleProperties();
  //random.randomize();
  m_todraw=0; // This is the liquid to draw (tap or mouse)

  m_previousmousex=-10;
  m_previousmousey=-10;

  m_camerarotatey=0.0f;
  m_camerarotatex=0.0f;

  //m_marching = new MarchingAlgorithms(m_mainrender2dthreshold, m_mainrender3dthreshold);

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
    if(m_marching.getSnapshotMode())
    {
      handleKeys('t');
    }
    grid.resize(gridheight*gridwidth*griddepth);
    cellsContainingParticles.resize(gridheight*gridwidth*griddepth,false);
  }

  renderwidth=gridwidth*renderresolution;
  renderheight=gridheight*renderresolution;

  render3dwidth=gridwidth*render3dresolution;
  render3dheight=gridheight*render3dresolution;

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

  m_marching=MarchingAlgorithms( m_mainrender2dthreshold, m_mainrender3dthreshold, squaresize,renderresolution,render3dresolution,halfwidth,halfheight);


}

/**
 * @brief World::draw draws the World to the current GL context. Called a lot - make this fast!
 */
void World::draw() {
  if (!m_isInit) return;

  glMatrixMode(GL_MODELVIEW);

  bool current_3d=m_3d;
  if(current_3d )
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
    for(int i=0; i<m_lastTakenParticle+1; ++i){
      if(m_particles[i].getAlive())
        m_particles[i].drawParticle(pointsize);
    }
  }


  else if(renderoption==2)
  {
    if(!m_3d)
    {
      for(auto& i : m_particleTypes)
      {
        std::vector<std::vector<float>> waterRenderGrid = renderGrid(&i);
        m_marching.calculateMarchingSquares(waterRenderGrid,i,false);
        m_marching.calculateMarchingSquares(waterRenderGrid,i,true);
      }
      m_marching.draw2DRealtime();
    }
    else
    {
      // DRAW LOADING SCREEN
      if(m_marching.getSnapshotMode()==1)
      {
        drawLoading();
        m_marching.setSnapshotMode(2);
      }

      // SNAPSHOT MODE PROCESSING
      else if(m_marching.getSnapshotMode()==2)
      {
        render3dresolution*=4;
        render3dwidth=gridwidth*render3dresolution;
        render3dheight=gridheight*render3dresolution;
        m_marching.toggle3DResolution();
        m_marching.clearSnapshot3DTriangles();
        for(auto& i : m_particleTypes)
        {
          std::vector<std::vector<std::vector<float>>> waterRender3dGrid = render3dGrid(&i);
          m_marching.calculateMarchingCubes(waterRender3dGrid,i);
        }
        m_marching.setSnapshotMode(3);
      }

      // DRAW THE GENERATED SNAPSHOT
      else if(m_marching.getSnapshotMode()>2)
      {
        m_marching.draw3DSnapshot();
      }

      // DRAW REAL-TIME FLUID MARCHING CUBES
      else
      {
        m_marching.clearSnapshot3DTriangles();
        for(auto& i : m_particleTypes)
        {
          std::vector<std::vector<std::vector<float>>> waterRender3dGrid = render3dGrid(&i);
          m_marching.calculateMarchingCubes(waterRender3dGrid,i);
        }
        m_marching.draw3DRealtime();
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
        for(int i = 0; i<6; ++i)
        {
          Particle newParticle;
          if(interactionradius==1.0f)
          {
            newParticle = Particle(Vec3(-3.0f+i*0.3f,halfheight/2+0.5f,-2.0f),&m_particleTypes[m_todraw]);
            newParticle.addVelocity(Vec3(3*0.03f-i*0.03f,-0.1f,0.0f));
          }
          else{
            newParticle = Particle(Vec3(-3.0f+i*0.1f,halfheight/2+0.5f,-2.0f),&m_particleTypes[m_todraw]);
            newParticle.addVelocity(Vec3(0,-0.1f,0.0f));
          }
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
    for(int i=0; i<m_lastTakenParticle+1; ++i)
    {
      if(m_particles[i].getAlive()) m_particles[i].addVelocity(gravityvel);
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

  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive())
    {
      m_particles[i].updatePrevPosition();
      if(!(m_particles[i].getDrag())&&!(m_particles[i].getWall()))
        m_particles[i].updatePosition(m_timestep,halfheight,halfwidth);                  // HEERERERERE
    }
  }
  hashParticles();

  //--------------------------------------SPRING ALGORITMNS-----------------------------------------------

  /*
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

                for(auto& spring : i->m_particleSprings)
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

                  i->m_particleSprings.push_back(thisspring);
                  j->m_particleSprings.push_back(thisspring);
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
      Vec3 rij = (m_particles[i.indexj].getPosition() - m_particles[i.indexi].getPosition());
      float rijmag = rij.length();

      if(rijmag>interactionradius && !particles[i.indexi].isObject())
      {
        deleteSpring(count);
      }

      else{
        rij.normalize();
        Vec3 D = rij*m_timestep*m_timestep*m_particles[i.indexi].getProperties()->getKspring()*(1-(i.L/interactionradius))*(i.L-rijmag);
        m_particles[i.indexi].addPosition(-D/2,halfheight,halfwidth);
        m_particles[i.indexj].addPosition(D/2,halfheight,halfwidth);   // HERE
      }

    }
    count++;
  }
  defragSprings();
  // */

  //----------------------------------DOUBLEDENSITY------------------------------------------


  int count =0;

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


      // MODIFY DENSITY AT BOUNDARIES
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
          if(!(j->getWall())) j->addPosition(D/2, halfheight, halfwidth); // HERE
          dx-=(D/2);
        }
      }
      if(!(i->getWall())) i->addPosition(dx, halfheight, halfwidth); // HERE
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


  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive())
    {
      // OpenMP <<---
      // MTSi parallelism for loop as prrallel as you can on an intel.

      if(m_boundaryType==0)
      {
        //------------------------------------BOTTOM------------------------------
        if(m_particles[i].getPosition()[1]-0.5f<-halfheight)
        {
          m_particles[i].setPosition(Vec3(m_particles[i].getPosition()[0],-halfheight+0.5f,m_particles[i].getPosition()[2]));
          m_particles[i].setVelocity(Vec3(m_particles[i].getVelocity()[0],-0.8f*m_particles[i].getVelocity()[1],0.0f));
        }
        //------------------------------------TOP------------------------------

        if(m_particles[i].getPosition()[1]+1.5f>halfheight)
        {
          m_particles[i].setPosition(Vec3(m_particles[i].getPosition()[0],halfheight-1.5f,m_particles[i].getPosition()[2]));
          m_particles[i].addVelocity(Vec3(0.0f,-0.8f*m_particles[i].getVelocity()[1],0.0f));
        }

        //------------------------------------RIGHT------------------------------
        if(m_particles[i].getPosition()[0]>(halfwidth-0.5f)*smallen)
        {
          m_particles[i].setPosition(Vec3(smallen*(halfwidth-0.5f),m_particles[i].getPosition()[1],m_particles[i].getPosition()[2]));
          m_particles[i].addVelocity(Vec3(-0.8f*m_particles[i].getVelocity()[0],0.0f));
        }
        //------------------------------------LEFT------------------------------
        if(m_particles[i].getPosition()[0]<(-halfwidth+0.5f)*smallen)
        {
          m_particles[i].setPosition(Vec3(smallen*(-halfwidth+0.5f),m_particles[i].getPosition()[1],m_particles[i].getPosition()[2]));
          m_particles[i].addVelocity(Vec3(-0.8f*m_particles[i].getVelocity()[0],0.0f));
        }

        if(m_particles[i].getPosition()[2]<-2-(halfwidth+0.5f)*smallen)
        {
          m_particles[i].setPosition(Vec3(m_particles[i].getPosition()[0],m_particles[i].getPosition()[1],-2-(halfwidth+0.5)*smallen));
          m_particles[i].addVelocity(Vec3(0.0f,0.0f,-0.8f*m_particles[i].getVelocity()[2]));
        }
        if(m_particles[i].getPosition()[2]>-2+(halfwidth-0.5f)*smallen)
        {
          m_particles[i].setPosition(Vec3(m_particles[i].getPosition()[0],m_particles[i].getPosition()[1],-2+(halfwidth-0.5f)*smallen));
          m_particles[i].addVelocity(Vec3(0.0f,0.0f,-0.8f*m_particles[i].getVelocity()[2]));
        }
      }

      if(m_boundaryType==1)
      {

        float fmult = 1.0f;

        float distance = - m_particles[i].getPosition()[1] + halfheight - 0.5f;
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(0.0f,-sqrt(fmult*force),0.0f));
        }

        distance = halfheight + m_particles[i].getPosition()[1];
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(0.0f,sqrt(fmult*force),0.0f));
        }

        distance = m_particles[i].getPosition()[0] + halfwidth*smallen;
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(sqrt(fmult*force),0.0f,0.0f));
        }

        distance = halfwidth*smallen - m_particles[i].getPosition()[0];
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(-sqrt(fmult*force),0.0f,0.0f));
        }

        distance = m_particles[i].getPosition()[2] - (-2-halfwidth*smallen);
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(0.0f,0.0f,sqrt(fmult*force)));
        }

        distance = (-2+halfwidth*smallen) - m_particles[i].getPosition()[2] ;
        if(distance<(m_boundaryMultiplier*interactionradius))
        {
          float force = ((m_boundaryMultiplier*interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(0.0f,0.0f,-sqrt(fmult*force)));
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
  if(((float)m_lastTakenParticle-(float)m_firstFreeParticle)/((float)howmany) >0.5)
  {
    std::cout<<"fraction: "<<((float)m_lastTakenParticle-(float)m_firstFreeParticle)/((float)howmany)<<std::endl;
    std::cout<<"lasttaken: "<<m_lastTakenParticle<<"  firstfree: "<<m_firstFreeParticle<<std::endl;
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
  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive())
    {
      float positionx = m_particles[i].getPosition()[0];
      float positiony = m_particles[i].getPosition()[1];
      float positionz = m_particles[i].getPosition()[2];

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

      m_particles[i].setGridPosition(grid_cell);

      if(grid_cell>=0 && grid_cell<gridSize)
      {
        cellsContainingParticles[grid_cell]=true;
        grid[grid_cell].push_back(&m_particles[i]);
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
      i->addPosition(Vec3(toaddx,-toaddy,0.0f),halfheight,halfwidth);
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

  case 't':
    if(m_3d)
    {
      if(renderoption==1) renderoption=2;
      if(m_marching.getSnapshotMode()>2)
      {
        render3dresolution/=4;
        render3dwidth=gridwidth*render3dresolution;
        render3dheight=gridheight*render3dresolution;
        m_marching.toggle3DResolution();
        m_marching.setSnapshotMode(0);
      }
      else
      {

        m_marching.setSnapshotMode(1);
      }
    }


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

  case 'c' :
    drawCube();
    break;

  case '>':
    makeParticlesBig();
    break;
  case '<':
    makeParticlesSmall();
    std::cout<<"YOO"<<std::endl;
    break;

  default:
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
      //if(!(i->isObject()))
      deleteParticle(i->getIndex());
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
  m_particles[m_firstFreeParticle]=particle;
  m_particles[m_firstFreeParticle].setIndex(m_firstFreeParticle);
  if(m_lastTakenParticle<m_firstFreeParticle)
  {
    ++m_lastTakenParticle;
    ++m_firstFreeParticle;
  }
  else{
    while(m_particles[m_firstFreeParticle].getAlive()==true)
    {
      ++m_firstFreeParticle;
    }
  }
  ++howManyAliveParticles;
}

void World::deleteParticle(int p)
{
  m_particles[p].setAlive(false);
  for(auto& i : m_particles[p].m_particleSprings)
  {
    deleteSpring(i);
  }

  if(m_lastTakenParticle==p)
  {
    while(m_particles[m_lastTakenParticle].getAlive()==false && m_lastTakenParticle>-1)
    {
      --m_lastTakenParticle;
    }
  }
  if(m_firstFreeParticle>p) m_firstFreeParticle=p;
  --howManyAliveParticles;
}

void World::defragParticles()
{
  for(int i=m_lastTakenParticle; i>m_firstFreeParticle; --i)
  {
    if(m_particles[i].getAlive())
    {
      for(auto& j : m_particles[i].m_particleSprings)
      {
        if(springs[j].indexi==i) springs[j].indexi=m_firstFreeParticle;
        else if(springs[j].indexj==i) springs[j].indexj=m_firstFreeParticle;
      }
      insertParticle(m_particles[i]);
      deleteParticle(i);
    }
  }
}

//-------------------------SPRING FUNCTIONS----------------------------------------

int World::insertSpring(Particle::Spring spring)
{
  int result = m_firstFreeSpring;
  springs[m_firstFreeSpring]=spring;
  if(m_lastTakenSpring<m_firstFreeSpring)
  {
    ++m_lastTakenSpring;
    ++m_firstFreeSpring;
  }
  else{
    while(springs[m_firstFreeSpring].alive)
    {
      ++m_firstFreeSpring;
    }
  }
  return result;
}

void World::deleteSpring(int s)
{
  springs[s].alive=false;
  m_particles[springs[s].indexi].updateSpringIndex(s,-1);
  m_particles[springs[s].indexj].updateSpringIndex(s,-1);
  if(m_lastTakenSpring==s)
  {
    while(springs[m_lastTakenSpring].alive==false && m_lastTakenSpring>-1)
    {
      --m_lastTakenSpring;
    }
  }
  if(m_firstFreeSpring>s) m_firstFreeSpring=s;
}

void World::defragSprings()
{
  for(int i=m_lastTakenSpring; i>m_firstFreeSpring; --i)
  {
    if(springs[i].alive)
    {
      m_particles[springs[i].indexi].updateSpringIndex(i,m_firstFreeSpring);
      m_particles[springs[i].indexj].updateSpringIndex(i,m_firstFreeSpring);
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
  if(m_lastTakenParticle<0) m_lastTakenParticle=0;
  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    deleteParticle(i);
  }

  hashParticles();

  //if(m_lastTakenSpring<0) m_lastTakenSpring=0;
  for(int i=0; i<m_lastTakenSpring+1; ++i)
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

// keep in world
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
  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive()&&(m_particles[i].getProperties()==p))
    {
      Vec3 heightwidth = getGridColumnRow(m_particles[i].getGridPosition())*renderresolution;
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

            float metaballx = currentx-m_particles[i].getPosition()[0];
            float metabally = currenty-m_particles[i].getPosition()[1];

            float metaballfloat = (interactionradius*interactionradius)/(metaballx*metaballx + metabally*metabally);

            rendergrid[currentrow][currentcolumn]+=metaballfloat;
          }
        }
      }
    }
  }
  return rendergrid;
}

void World::set3D(bool b)
{
  m_3d=b;
}

bool World::get3D()
{
  return m_3d;
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

std::vector<std::vector<std::vector<float>>> World::render3dGrid(ParticleProperties *p)
{
  std::vector<std::vector<std::vector<float>>> rendergrid;
  rendergrid.clear();
  rendergrid.resize(render3dwidth+1);
  for(auto& i : rendergrid)
  {
    i.resize(render3dheight+1);
    for(auto& j : i)
    {
      j.resize(render3dwidth+1,0.0f);
    }
  }

  float rendersquare=squaresize/render3dresolution;

  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive()&&(m_particles[i].getProperties()==p))
    {
      Vec3 heightwidthdepth = getGridXYZ(m_particles[i].getGridPosition())*render3dresolution; // 3Dify this

      for(int x = -2*render3dresolution; x<=4*render3dresolution; ++x)
      {
        for(int y = -2*render3dresolution; y<=4*render3dresolution ; ++y)
        {
          for(int z = -2*render3dresolution; z<=4*render3dresolution ; ++z)
          {

            //std::cout<<"hello"<<std::endl;
            int currentcolumn=heightwidthdepth[0]+x;
            int currentrow=heightwidthdepth[1]+y;
            int currentdepth=heightwidthdepth[2]+z;

            if(currentcolumn<render3dwidth && currentcolumn>0 &&
               currentrow<render3dheight && currentrow>0 &&
               currentdepth<render3dwidth && currentdepth>0)
            {
              float currentx = rendersquare*(float)currentcolumn - halfwidth;
              float currenty = rendersquare*(float)currentrow - halfheight;
              float currentz = rendersquare*(float)currentdepth - 2 - halfwidth;

              float metaballx = currentx-m_particles[i].getPosition()[0];
              float metabally = currenty-m_particles[i].getPosition()[1];
              float metaballz = currentz-m_particles[i].getPosition()[2];

              float metaballfloat = (interactionradius*interactionradius)/(metaballx*metaballx + metabally*metabally + metaballz*metaballz);
              //std::cout<<metaballfloat<<std::endl;
              rendergrid[currentcolumn][currentrow][currentdepth]+=metaballfloat;
            }
          }
        }
      }
    }
  }
  return rendergrid;
}

Vec3 World::getGridXYZ(int k) // CHECK THIS
{
  int z = floor(k/(gridwidth*gridheight));
  int y = floor((k - z*gridwidth*gridheight)/gridwidth);
  int x = k - y*gridwidth - z*gridwidth*gridheight;

  return Vec3(x,y,z);
}

int World::getSnapshotMode()
{
  return m_marching.getSnapshotMode();
}

void World::drawLoading()
{
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // You should probably use CSurface::OnLoad ... ;)
  //-- and make sure the Surface pointer is good!
  GLuint titleTextureID = 0;
  SDL_Surface* Surface = IMG_Load("textures/buttons.png");
  if(!Surface)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    std::cout<<"error"<<std::endl;
  }

  glGenTextures(1, &titleTextureID);
  glBindTexture(GL_TEXTURE_2D, titleTextureID);

  int Mode = GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  float texH = 0.1f;
  float texW = 0.6f;
  float X = -1.0f;
  float Y = -0.3f;
  float Width = 2.8;
  float Height = 0.6f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(0, 0.9+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(0+texW, 0.9+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(0+texW, 0.9); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(0, 0.9); glVertex3f(X, Y + Height, -2);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING) ;
}

void World::drawCube()
{
  if(!m_3d)
  {
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
      {
        Particle newparticle = Particle(Vec3(-3.0f+i*0.2f,3.0f-j*0.2f,-2.0f),&m_particleTypes[0]);
        newparticle.setIsObject();
        insertParticle(newparticle);
      }
    }
  }
}

void World::makeParticlesBig()
{
    pointsize=10.f;
    squaresize=1.0f;
    m_marching.setSquareSize(1.0f);
    interactionradius=1.0f;
    hashParticles();
}

void World::makeParticlesSmall()
{
    pointsize=5.0f;
    squaresize=0.5f;
    m_marching.setSquareSize(0.5f);
    interactionradius=0.5f;
    hashParticles();
}

void World::increase2DResolutionWORLD()
{
  ++renderresolution;
  m_marching.increase2DResolution();
  renderwidth=gridwidth*renderresolution;
  renderheight=gridheight*renderresolution;
}

void World::decrease2DResolutionWORLD()
{
  m_marching.decrease2DResolution();
  if(renderresolution!=1)
    --renderresolution;
  renderwidth=gridwidth*renderresolution;
  renderheight=gridheight*renderresolution;
}
