///
///  @file World.cpp
///  @brief contains all particles and methods to draw and update them

#include "include/World.h"

World::World() :
  m_isInit(false),
  m_startTime(0.0),
  m_elapsedTime(0.0),
  m_interactionradius(1.0f),
  m_squaresize(1.0f),
  m_timestep(1.0f),
  m_pointsize(10.0f),
  m_mainrender2dthreshold(90.0f),
  m_mainrender3dthreshold(100.0f),
  m_render2DResolution(4),
  m_render3dresolution(2),
  m_renderoption(1),
  m_rain(false),
  m_drawwall(false),
  m_gravity(true),
  m_springsize(500000),
  m_particlesPoolSize(5000),
  m_3d(false),
  m_boundaryMultiplier(1.0f),
  m_boundaryType(2),  // Have a go at changing if you want (values 0, 1, 2)
  m_snapshotmultiplier(4)
{
}

World::~World() {
}

void World::init() {

  if (m_isInit) return;


  glEnable(GL_TEXTURE_2D);

  glFrontFace(GL_CCW);

  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT_MODEL_AMBIENT);

  GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

  glEnable( GL_MULTISAMPLE_ARB);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glPointSize(m_pointsize);

  // Set the background colour
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  m_particles.clear();
  m_springs.clear();

  Particle defaultparticle;
  defaultparticle.setAlive(false);
  m_particles.resize(m_particlesPoolSize,defaultparticle);
  m_firstFreeParticle=0;
  m_lastTakenParticle=-1;
  m_howManyAliveParticles=0;

  Particle::Spring defaultspring;
  defaultspring.alive=false;
  m_springs.resize(m_springsize,defaultspring);
  m_firstFreeSpring=0;
  m_lastTakenSpring=-1;

  // DEFAULT PARTICLE PROPERTIES
  m_particleTypes.push_back(ParticleProperties()); //water
  m_particleTypes.push_back(ParticleProperties(true,0.3f,0.2f,0.004f,0.2f,0.01f,0.004f,0.3f,10.0f,0.1f,0.3f,0.1f,true)); //slime
  m_particleTypes.push_back(ParticleProperties(false,0.303f,0.2952f,0.004f,0.2f,0.096337f,0.0787213f,0.3f,2.957f,0.2f,0.52f,0.8f,true)); //blobby
  m_particleTypes.push_back(ParticleProperties()); //random
  m_particleTypes.push_back(ParticleProperties(true,0.3f,0.2f,0.004f,0.2f,0.01f,0.004f,0.7f,10.0f,0.8f,0.52f,0.25f,false)); // cube
                                              //     sig, bet, gamma,alph, knear, k,   kspri,p0,  red,  green,  blue

  m_todraw=0; // This is the liquid to draw (tap or mouse)

  m_previousmousex=-10;
  m_previousmousey=-10;

  m_camerarotatey=0.0f;
  m_camerarotatex=0.0f;

  m_isInit = true;
}

void World::resizeWorld(int w, int h)
{
  // I have to clear world otherwise will crash when marching cubes are being rendered
  clearWorld();
  m_pixelheight=h;
  m_pixelwidth=w;

  float i = 5;
  float ara = float(w)/float(h);

  m_halfheight=i;
  m_halfwidth=i*ara;

  m_gridwidth=ceil((m_halfwidth*2)/m_squaresize);
  m_gridheight=ceil((m_halfheight*2)/m_squaresize);
  m_griddepth=m_gridwidth;

  m_cellsContainingParticles.clear();
  m_grid.clear();

  if(!m_3d)
  {
    m_grid.resize(m_gridheight*m_gridwidth);
    m_cellsContainingParticles.resize(m_gridheight*m_gridwidth,false);
  }
  else
  {
    if(m_marching.getSnapshotMode()==3)
    {
      handleKeys('t');
    }
    m_grid.resize(m_gridheight*m_gridwidth*m_griddepth);
    m_cellsContainingParticles.resize(m_gridheight*m_gridwidth*m_griddepth,false);
  }

  m_render2dwidth=m_gridwidth*m_render2DResolution;
  m_render2dheight=m_gridheight*m_render2DResolution;

  m_render3dwidth=m_gridwidth*m_render3dresolution;
  m_render3dheight=m_gridheight*m_render3dresolution;

  hashParticles();

}

void World::resizeWindow(int w, int h) {

  m_howmanytimesrandomized=0;
  if (!m_isInit) return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  m_pixelheight=h;
  m_pixelwidth=w;

  float i = 5;
  float ara = float(w)/float(h);

  glOrtho(-i*ara,i*ara,-i,i,0.1, 5000.0);

  m_halfheight=i;
  m_halfwidth=i*ara;

  glViewport(0,0,w,h);

  glMatrixMode(GL_MODELVIEW);

  m_marching=MarchingAlgorithms( m_mainrender2dthreshold, m_mainrender3dthreshold, m_squaresize,
                                 m_render2DResolution,m_render3dresolution,m_halfwidth,m_halfheight,
                                 m_snapshotmultiplier);

  }

void World::draw() {
  if (!m_isInit) return;

  glMatrixMode(GL_MODELVIEW);

  bool current_3d=m_3d;
  if(current_3d && m_marching.getSnapshotMode()!=1)
  {
    glPushMatrix();
    glTranslatef(0.0f,2.0f,-10.0f);
    glTranslatef(0.0f, 0.0f, -2.0f); // move back to focus of gluLookAt
    glRotatef(m_camerarotatex,0.0f,1.0f,0.0f); //  rotate around center
    glRotatef(m_camerarotatey,1.0f,0.0f,0.0f); //  rotate around center
    glTranslatef(0.0f, 0.0f, 2.0f); //move object to center
  }

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  if(m_renderoption==1){
    for(int i=0; i<m_lastTakenParticle+1; ++i){
      if(m_particles[i].getAlive())
        m_particles[i].drawParticle(m_pointsize);
    }
  }


  else if(m_renderoption==2)
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
        m_render3dresolution*=m_snapshotmultiplier;
        m_render3dwidth=m_gridwidth*m_render3dresolution;
        m_render3dheight=m_gridheight*m_render3dresolution;
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

  if(m_rain)
  {
    if(everyother%2==0){
      if(!m_3d)
      {
        for(int i = 0; i<6; ++i)
        {
          Particle newParticle;
          if(m_interactionradius==1.0f)
          {
            newParticle = Particle(Vec3(-3.0f+i*0.3f,m_halfheight/2+0.5f,-2.0f),&m_particleTypes[m_todraw]);
            newParticle.addVelocity(Vec3(3*0.03f-i*0.03f,-0.1f,0.0f));
          }
          else{
            newParticle = Particle(Vec3(-3.0f+i*0.1f,m_halfheight/2+0.5f,-2.0f),&m_particleTypes[m_todraw]);
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
            Particle newParticle =Particle(Vec3(i*0.3f,m_halfheight/5,-2.0+j*0.3f),&m_particleTypes[m_todraw]);
            newParticle.addVelocity(Vec3(0.0f,0.0f,0.0f));
            insertParticle(newParticle);
          }
        }
      }
    }
  }

  // ------------------------------GRAVITY --------------------------------------------
  if(m_gravity)
  {
    Vec3 gravityvel = Vec3(0.0f,-0.008*m_timestep,0.0f);

    // The line below rotates the gravity when in 3D according to how far you tip the box.
    // gravityvel.rotateAroundXAxisf(-m_camerarotatey*(M_PI/180.0f));

    for(int i=0; i<m_lastTakenParticle+1; ++i)
    {
      if(m_particles[i].getAlive()) m_particles[i].addVelocity(gravityvel);
    }
  }

  // ------------------------------VISCOSITY--------------------------------------------
  // TODO : Implement spatial hash for loop here


  int choo = 0;

  //#pragma omp parallel for ordered schedule(dynamic)
  for(auto k = 0; k<(int)m_grid.size(); ++k)
  {
    int ploo = 0;
    for(auto& i : m_grid[k])
    {
      if(!(i->getWall()))
      {
        std::vector<Particle *> surroundingParticles = getSurroundingParticles(choo,1,false);
        int cloo = 0;
        for(auto& j : surroundingParticles)
        {
          if(cloo>ploo && !(j->getWall()))
          {
            Vec3 rij=(j->getPosition()-i->getPosition());
            float q = rij.length()/m_interactionradius;
            if(q<1 && q!=0)
            {
              rij.normalize();
              float u = (i->getVelocity()-j->getVelocity()).dot(rij);
              if(u>0)
              {
                ParticleProperties *thisproperties = i->getProperties();
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
        m_particles[i].updatePosition(m_timestep,m_halfheight,m_halfwidth,m_3d);
    }
  }
  hashParticles();

  //--------------------------------------SPRING ALGORITMNS-----------------------------------------------

  for(int k=0; k<m_gridheight*m_gridwidth; ++k)
  {
    if(m_cellsContainingParticles[k])
    {
      std::vector<Particle *> surroundingParticles = getSurroundingParticles(k,3,false);

      for(auto& i : m_grid[k])
      {
        if(i->getProperties()->getSpring() && (!i->isObject() || (i->isObject() && !i->isInit()) ) && !i->getWall())
        {
          for(auto& j : surroundingParticles)
          {
            if(j->getProperties()==i->getProperties()) // They only cling when same type
            {
              Vec3 rij=(j->getPosition()-i->getPosition());
              float rijmag = rij.length();
              float q = rijmag/m_interactionradius;

              if(q<1 && q!=0)
              {
                // FINDING / CREATING THE SPRING
                bool quiter = false;
                int thisspring;

                for(auto& spring : i->m_particleSprings)
                {
                  if(((m_springs[spring].indexi==i->getIndex()) && (m_springs[spring].indexj==j->getIndex())) ||
                     ((m_springs[spring].indexi==j->getIndex()) && (m_springs[spring].indexj==i->getIndex())))
                  {
                    // FOUND EXISTING SPRING
                    m_springs[spring].alive=true;
                    thisspring=spring;
                    quiter=true;
                    break;
                  }
                }

                if(!quiter)
                {
                  // HAVE TO CREATE A NEW SPRING
                  Particle::Spring newspring;
                  newspring.indexi=i->getIndex();
                  newspring.indexj=j->getIndex();
                  newspring.count=everyother-1;
                  newspring.alive=true;
                  newspring.L = m_interactionradius;

                  thisspring = insertSpring(newspring);
                  if(thisspring==-1) break;

                  i->m_particleSprings.push_back(thisspring);
                  j->m_particleSprings.push_back(thisspring);
                }

                // MAKING SURE EACH SPRING IS ONLY UPDATED ONCE PER FRAME with count
                if(m_springs[thisspring].count!=everyother)
                {
                  GLfloat L = m_springs[thisspring].L;
                  GLfloat d= L*i->getProperties()->getGamma();
                  GLfloat alpha = i->getProperties()->getAlpha();

                  if(rijmag>L+d)
                  {
                    m_springs[thisspring].L=L+m_timestep*alpha*(rijmag-L-d);
                  }
                  else if(rijmag<L-d)
                  {
                    m_springs[thisspring].L=L-m_timestep*alpha*(L-d-rijmag);
                  }
                  m_springs[thisspring].count++;
                }
              }
            }
          }
          i->setInit();
        }
      }
    }
  }

  int count=0;
  for(auto& i : m_springs)
  {
    if(i.alive){
      Vec3 rij = (m_particles[i.indexj].getPosition() - m_particles[i.indexi].getPosition());
      float rijmag = rij.length();

      // WE DELETE SPRING IF PARTICLES TOO FAR APART
      if(rijmag>m_interactionradius && !m_particles[i.indexi].isObject())
      {
        deleteSpring(count);
      }

      // ELSE WE MOVE THE PARTICLE ACCORDING TO SPRING
      else
      {
        rij.normalize();
        Vec3 D = rij*m_timestep*m_timestep*m_particles[i.indexi].getProperties()->getKspring()*(1-(i.L/m_interactionradius))*(i.L-rijmag);
        m_particles[i.indexi].addPosition(-D/2,m_halfheight,m_halfwidth,m_3d);
        m_particles[i.indexj].addPosition(D/2,m_halfheight,m_halfwidth,m_3d);   // HERE
      }

    }
    count++;
  }
  defragSprings();

  //----------------------------------DOUBLEDENSITY------------------------------------------
  count =0;

  for(int k = 0; k<(int)m_grid.size(); ++k)
  {
    std::vector<Particle *> neighbours=getSurroundingParticles(count,1,false);

    for(auto& i : m_grid[k])
    {
      float density =0;
      float neardensity=0;
      for(auto& j : neighbours)
      {
        Vec3 rij = j->getPosition()-i->getPosition();
        float rijmag = rij.length();
        float q = rijmag/m_interactionradius;
        if(q<1 && q!=0) // q==0 when same particle
        {
          density+=(1.0f-q)*(1.0f-q);
          neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);
        }
      }


      // MODIFY DENSITY AT BOUNDARIES when boundary type == 1
      if(m_boundaryType==1)
      {
        // BOTTOM
        float distance = m_halfheight + i->getPosition()[1];
        float q = distance/(m_boundaryMultiplier*m_interactionradius);
        if(q<1 && q!=0) // q==0 when same particle
        {
          density+=(1.0f-q)*(1.0f-q);
          neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);
        }
        // RIGHT
        distance = m_halfwidth - i->getPosition()[0];
        q = distance/(m_boundaryMultiplier*m_interactionradius);
        if(q<1 && q!=0) // q==0 when same particle
        {
          density+=(1.0f-q)*(1.0f-q);
          neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);
        }

        // LEFT
        distance = i->getPosition()[0] + m_halfwidth ;
        q = distance/(m_boundaryMultiplier*m_interactionradius);
        if(q<1 && q!=0) // q==0 when same particle
        {
          density+=(1.0f-q)*(1.0f-q);
          neardensity+=(1.0f-q)*(1.0f-q)*(1.0f-q);
        }
      }

      float p0 = i->getProperties()->getP0();
      float k = i->getProperties()->getK();
      float knear = i->getProperties()->getKnear();

      float P = k*(density -p0);
      float Pnear = knear * neardensity;
      Vec3 dx = Vec3();
      for(auto& j : neighbours)
      {
        Vec3 rij = j->getPosition()-i->getPosition();
        float rijmag = rij.length();
        float q = rijmag/m_interactionradius;
        if(q<1 && q!=0)
        {
          rij.normalize();
          Vec3 D = rij*(m_timestep*m_timestep*(P*(1.0f-q))+Pnear*(1.0f-q)*(1.0f-q));
          if(!(j->getWall())) j->addPosition(D/2, m_halfheight, m_halfwidth,m_3d);
          dx-=(D/2);
        }
      }
      if(!(i->getWall())) i->addPosition(dx, m_halfheight, m_halfwidth,m_3d);
    }
    count++;
  }

  //----------------------------------MAKE NEW VELOCITY-------------------------------------

  for(auto& list : m_grid)
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


  // I found that the particles glitch and jump as they are being drawn in the middle of update
  // So we see the particles before this boundary algorithm has been run.

  // So these boundary algorithms have been replaced by the new and improved
  // addPosition and updatePosition in Particle.cpp. Means they never leave the boundary when
  // position is updated.
  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive())
    {
      if(m_boundaryType==0)
      {
        //------------------------------------BOTTOM------------------------------
        if(m_particles[i].getPosition()[1]-0.5f<-m_halfheight)
        {
          m_particles[i].setPosition(Vec3(m_particles[i].getPosition()[0],-m_halfheight+0.5f,m_particles[i].getPosition()[2]));
          m_particles[i].setVelocity(Vec3(m_particles[i].getVelocity()[0],-0.8f*m_particles[i].getVelocity()[1],0.0f));
        }
        //------------------------------------TOP------------------------------

        if(m_particles[i].getPosition()[1]+1.5f>m_halfheight)
        {
          m_particles[i].setPosition(Vec3(m_particles[i].getPosition()[0],m_halfheight-1.5f,m_particles[i].getPosition()[2]));
          m_particles[i].addVelocity(Vec3(0.0f,-0.8f*m_particles[i].getVelocity()[1],0.0f));
        }

        //------------------------------------RIGHT------------------------------
        if(m_particles[i].getPosition()[0]>(m_halfwidth-0.5f)*smallen)
        {
          m_particles[i].setPosition(Vec3(smallen*(m_halfwidth-0.5f),m_particles[i].getPosition()[1],m_particles[i].getPosition()[2]));
          m_particles[i].addVelocity(Vec3(-0.8f*m_particles[i].getVelocity()[0],0.0f));
        }
        //------------------------------------LEFT------------------------------
        if(m_particles[i].getPosition()[0]<(-m_halfwidth+0.5f)*smallen)
        {
          m_particles[i].setPosition(Vec3(smallen*(-m_halfwidth+0.5f),m_particles[i].getPosition()[1],m_particles[i].getPosition()[2]));
          m_particles[i].addVelocity(Vec3(-0.8f*m_particles[i].getVelocity()[0],0.0f));
        }

        if(m_particles[i].getPosition()[2]<-2-(m_halfwidth+0.5f)*smallen)
        {
          m_particles[i].setPosition(Vec3(m_particles[i].getPosition()[0],m_particles[i].getPosition()[1],-2-(m_halfwidth+0.5)*smallen));
          m_particles[i].addVelocity(Vec3(0.0f,0.0f,-0.8f*m_particles[i].getVelocity()[2]));
        }
        if(m_particles[i].getPosition()[2]>-2+(m_halfwidth-0.5f)*smallen)
        {
          m_particles[i].setPosition(Vec3(m_particles[i].getPosition()[0],m_particles[i].getPosition()[1],-2+(m_halfwidth-0.5f)*smallen));
          m_particles[i].addVelocity(Vec3(0.0f,0.0f,-0.8f*m_particles[i].getVelocity()[2]));
        }
      }

      if(m_boundaryType==1)
      {
        // This version would have a higher velocity added to the particle as it got closer to the
        // boundary. However I got lots of unwanted effects to the overall blob of fluid. Such as
        // a "bubbling" effect like water emerging after a bubble has risen.

        float fmult = 1.0f;

        float distance = - m_particles[i].getPosition()[1] + m_halfheight - 0.5f;
        if(distance<(m_boundaryMultiplier*m_interactionradius))
        {
          float force = ((m_boundaryMultiplier*m_interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(0.0f,-sqrt(fmult*force),0.0f));
        }

        distance = m_halfheight + m_particles[i].getPosition()[1];
        if(distance<(m_boundaryMultiplier*m_interactionradius))
        {
          float force = ((m_boundaryMultiplier*m_interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(0.0f,sqrt(fmult*force),0.0f));
        }

        distance = m_particles[i].getPosition()[0] + m_halfwidth*smallen;
        if(distance<(m_boundaryMultiplier*m_interactionradius))
        {
          float force = ((m_boundaryMultiplier*m_interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(sqrt(fmult*force),0.0f,0.0f));
        }

        distance = m_halfwidth*smallen - m_particles[i].getPosition()[0];
        if(distance<(m_boundaryMultiplier*m_interactionradius))
        {
          float force = ((m_boundaryMultiplier*m_interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(-sqrt(fmult*force),0.0f,0.0f));
        }

        distance = m_particles[i].getPosition()[2] - (-2-m_halfwidth*smallen);
        if(distance<(m_boundaryMultiplier*m_interactionradius))
        {
          float force = ((m_boundaryMultiplier*m_interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(0.0f,0.0f,sqrt(fmult*force)));
        }

        distance = (-2+m_halfwidth*smallen) - m_particles[i].getPosition()[2] ;
        if(distance<(m_boundaryMultiplier*m_interactionradius))
        {
          float force = ((m_boundaryMultiplier*m_interactionradius)-distance)/(m_timestep*m_timestep);
          m_particles[i].addVelocity(Vec3(0.0f,0.0f,-sqrt(fmult*force)));
        }


      }

    }
  }

  //----------------------------------CLEANUP ------------------------------------------------

  if(everyother%30==0)
  {
    std::cout<<"Numebr:"<<m_howManyAliveParticles<<std::endl;
  }

  *updateinprogress = false;
}

//---------------------------------HASH FUNCTIONS--------------------------------------------------------

void World::hashParticles()
{
  int gridSize;
  if(!m_3d) gridSize = m_gridwidth*m_gridheight;
  else gridSize = m_gridwidth*m_gridheight*m_griddepth;

  m_cellsContainingParticles.assign(gridSize,false);
  std::vector<Particle *> newvector;
  m_grid.assign(gridSize,newvector);
  int grid_cell;
  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive())
    {
      float positionx = m_particles[i].getPosition()[0];
      float positiony = m_particles[i].getPosition()[1];
      float positionz = m_particles[i].getPosition()[2];

      if(positionx<-m_halfwidth) positionx=m_halfwidth;
      else if (positionx>m_halfwidth) positionx=m_halfwidth;
      if(positiony<-m_halfheight) positiony=m_halfheight;
      else if (positiony>m_halfheight) positiony=m_halfheight;
      if(positionz<-2-m_halfwidth) positionz=-2-m_halfwidth;
      else if (positionz>-2+m_halfwidth) positionz=-2+m_halfwidth;


      grid_cell=
          floor((positionx+m_halfwidth)/m_squaresize)+
          floor((positiony+m_halfheight)/m_squaresize)*m_gridwidth;


      if(m_3d) grid_cell+=floor((positionz+m_halfwidth+2)/m_squaresize)*m_gridwidth*m_gridheight;

      m_particles[i].setGridPosition(grid_cell);

      if(grid_cell>=0 && grid_cell<gridSize)
      {
        m_cellsContainingParticles[grid_cell]=true;
        m_grid[grid_cell].push_back(&m_particles[i]);
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
        int grid_cell = thiscell+ i + j*m_gridwidth;
        if(grid_cell<(m_gridwidth*m_gridheight) && grid_cell>=0)
        {
          for(auto& p : m_grid[grid_cell])
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
          int grid_cell = thiscell+ i + j*m_gridwidth + k*m_gridwidth*m_gridheight;

          if(grid_cell<(m_gridwidth*m_gridheight*m_griddepth) && grid_cell>=0)
          {
            for(auto& p : m_grid[grid_cell])
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
  int row = floor(k/m_gridwidth);
  int column = k - row*m_gridwidth;
  return Vec3(column,row);
}

//--------------------------------INPUT FUNCTIONS---------------------------------------------------

void World::mouseDraw(int x, int y)
{
  float objectdensity=0.1f;
  if(m_drawwall) objectdensity=0.05f;

  float currentx = ((float)x/(float)m_pixelwidth)*(m_halfwidth*2) - m_halfwidth;
  float currenty = -((float)y/(float)m_pixelheight)*(m_halfheight*2) + m_halfheight;

  float correctedx = floor(currentx/objectdensity + 0.5f);
  correctedx*=objectdensity;

  float correctedy = floor(currenty/objectdensity + 0.5f);
  correctedy*=objectdensity;

  bool drawparticle=true;
  int grid_cell=floor((correctedx+m_halfwidth)/m_squaresize)+floor((correctedy+m_halfheight)/m_squaresize)*m_gridwidth;
  for(auto& i : m_grid[grid_cell])
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
    if(m_drawwall)
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

void World::mouseDrag(int _x, int _y)
{
  if(m_previousmousex>0 && m_previousmousey>0)
  {
    float toaddx = (_x-m_previousmousex)*((m_halfwidth*2)/(float)m_pixelwidth);
    float toaddy = (_y-m_previousmousey)*((m_halfwidth*2)/(float)m_pixelwidth);

    for(auto& i : m_draggedParticles)
    {
      i->addPosition(Vec3(toaddx,-toaddy,0.0f),m_halfheight,m_halfwidth,m_3d);
      getbackhere(&(*i));
    }
    hashParticles();
  }
  m_previousmousex=_x;
  m_previousmousey=_y;
}

void World::selectDraggedParticles(int _x, int _y)
{
  // find OpenGL coordinates from window coordinates
  float worldx = ((float)_x/(float)m_pixelwidth)*(m_halfwidth*2) - m_halfwidth;
  float worldy = -((float)_y/(float)m_pixelheight)*(m_halfheight*2) + m_halfheight;

  // find spatial hash index
  int grid_cell=floor((worldx+m_halfwidth)/m_squaresize)+floor((worldy+m_halfheight)/m_squaresize)*m_gridwidth;
  // get surrounding particles
  m_draggedParticles = getSurroundingParticles(grid_cell,2,true);

  for(auto& i : m_draggedParticles)
  {
    i->setDrag(true);
  }
  m_previousmousex=_x;
  m_previousmousey=_y;
}

void World::getbackhere(Particle * p)
{
  if(p->getPosition()[0]>m_halfwidth-0.5f) p->getPosition()[0]=m_halfwidth-0.5f;
  else if(p->getPosition()[0]<-m_halfwidth+0.5f) p->getPosition()[0]=-m_halfwidth+0.5f;
  if(p->getPosition()[1]>m_halfheight-0.5f) p->getPosition()[1]=m_halfheight-0.5f;
  else if(p->getPosition()[1]<-m_halfheight+0.5f) p->getPosition()[1]=-m_halfheight+0.5f;
}

void World::mouseDragEnd(int _x, int _y)
{
  Vec3 newVelocity = Vec3(_x-m_previousmousex,m_previousmousey-_y);

  for(auto& i : m_draggedParticles)
  {
    i->setDrag(false);
    i->addVelocity(newVelocity*0.05f);
  }
  m_draggedParticles.clear();
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
      if(m_drawwall) m_drawwall=false;
      else m_drawwall=true;
    }
    break;
  case 'r':
    if(m_renderoption==1) m_renderoption=2;
    else m_renderoption=1;
    break;

  case 't':
    if(m_3d)
    {
      if(m_renderoption==1) m_renderoption=2;
      if(m_marching.getSnapshotMode()>2)
      {
        m_render3dresolution/=m_snapshotmultiplier;
        m_render3dwidth=m_gridwidth*m_render3dresolution;
        m_render3dheight=m_gridheight*m_render3dresolution;
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
      resizeWindow(m_pixelwidth,m_pixelheight);
      if(m_renderoption==2) m_renderoption=1;
      m_camerarotatex=0.0f;
      m_camerarotatey=0.0f;
    }
    break;

  case 'o' :
    resizeWindow(m_pixelwidth,m_pixelheight);
    break;

  case 'c' :
    drawCube();
    break;

  default:
    break;

  }
}

void World::mouseErase(int x, int y)
{
  hashParticles();
  float worldx = ((float)x/(float)m_pixelwidth)*(m_halfwidth*2) - m_halfwidth;
  float worldy = -((float)y/(float)m_pixelheight)*(m_halfheight*2) + m_halfheight;
  int grid_cell=floor((worldx+m_halfwidth)/m_squaresize)+floor((worldy+m_halfheight)/m_squaresize)*m_gridwidth;
  if (m_cellsContainingParticles[grid_cell])
  {
    bool thereisanobject=false;
    for(auto& i : m_grid[grid_cell])
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
  if(m_firstFreeParticle<m_particlesPoolSize-1)
  {
    m_particles[m_firstFreeParticle]=particle;
    m_particles[m_firstFreeParticle].setIndex(m_firstFreeParticle);
    if(m_lastTakenParticle<m_firstFreeParticle)
    {
      ++m_lastTakenParticle;
      ++m_firstFreeParticle;
    }
    else{
      while(m_particles[m_firstFreeParticle].getAlive()==true && m_firstFreeParticle!=m_particlesPoolSize)
      {
        ++m_firstFreeParticle;
      }
    }
    ++m_howManyAliveParticles;
  }
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
  --m_howManyAliveParticles;
}

void World::defragParticles()
{
  for(int i=m_lastTakenParticle; i>m_firstFreeParticle; --i)
  {
    if(m_particles[i].getAlive())
    {
      for(auto& j : m_particles[i].m_particleSprings)
      {
        if(m_springs[j].indexi==i) m_springs[j].indexi=m_firstFreeParticle;
        else if(m_springs[j].indexj==i) m_springs[j].indexj=m_firstFreeParticle;
      }
      insertParticle(m_particles[i]);
      deleteParticle(i);
    }
  }
}

//-------------------------SPRING FUNCTIONS----------------------------------------

int World::insertSpring(Particle::Spring spring)
{

  if(m_firstFreeSpring<m_springsize-1)
  {
    int result = m_firstFreeSpring;
    m_springs[m_firstFreeSpring]=spring;
    if(m_lastTakenSpring<m_firstFreeSpring)
    {
      ++m_lastTakenSpring;
      ++m_firstFreeSpring;
    }
    else{
      while(m_springs[m_firstFreeSpring].alive)
      {
        ++m_firstFreeSpring;
      }
    }
    return result;
  }
  else
  {
    return -1;
  }
}

void World::deleteSpring(int s)
{
  m_springs[s].alive=false;
  m_particles[m_springs[s].indexi].updateSpringIndex(s,-1);
  m_particles[m_springs[s].indexj].updateSpringIndex(s,-1);
  if(m_lastTakenSpring==s)
  {
    while(m_springs[m_lastTakenSpring].alive==false && m_lastTakenSpring>-1)
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
    if(m_springs[i].alive)
    {
      m_particles[m_springs[i].indexi].updateSpringIndex(i,m_firstFreeSpring);
      m_particles[m_springs[i].indexj].updateSpringIndex(i,m_firstFreeSpring);
      insertSpring(m_springs[i]);
      deleteSpring(i);
    }
  }
}
//-------------------------GETTERS------------------------------

float World::getHalfHeight() const
{
  return m_halfheight;
}

float World::getHalfWidth() const
{
  return m_halfwidth;
}

void World::toggleRain()
{
  if(m_rain) m_rain=false;
  else m_rain=true;
}

void World::clearWorld()
{
  if(m_lastTakenParticle<0) m_lastTakenParticle=0;
  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    // This also deletes all springs
    deleteParticle(i);
  }

  hashParticles();
}

void World::toggleGravity()
{
  if(m_gravity) m_gravity=false;
  else m_gravity=true;
}

void World::drawWith(int type)
{
  if(type==0)
  {
    m_todraw=0;
  }
  else if(type==1)
  {
    m_todraw=1;
    m_howmanytimesrandomized++;
  }
}

//--------------------------3D STUFF ------------------------------------------------

void World::mouseMove(const int &x, const int &y, bool leftclick) {
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

    m_previousmousex=x;
    m_previousmousey=y;
  }
}

std::vector<std::vector<float>> World::renderGrid(ParticleProperties *p)
{
  std::vector<std::vector<float>> rendergrid;
  rendergrid.clear();
  rendergrid.resize(m_render2dheight+1);
  for(auto& i : rendergrid)
  {
    i.resize(m_render2dwidth+1,0.0f);
  }

  float rendersquare=m_squaresize/m_render2DResolution;
  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive()&&(m_particles[i].getProperties()==p))
    {
      Vec3 heightwidth = getGridColumnRow(m_particles[i].getGridPosition())*m_render2DResolution;
      for(int x = -2*m_render2DResolution; x<=4*m_render2DResolution; ++x)
      {
        for(int y = -2*m_render2DResolution; y<=4*m_render2DResolution ; ++y)
        {
          int currentcolumn=heightwidth[0]+x;
          int currentrow=heightwidth[1]+y;

          if(currentcolumn<m_render2dwidth && currentcolumn>0 &&
             currentrow<m_render2dheight && currentrow>0)
          {
            float currentx = rendersquare*(float)currentcolumn - m_halfwidth;
            float currenty = rendersquare*(float)currentrow - m_halfheight;

            float metaballx = currentx-m_particles[i].getPosition()[0];
            float metabally = currenty-m_particles[i].getPosition()[1];

            float metaballfloat = (m_interactionradius*m_interactionradius)/(metaballx*metaballx + metabally*metabally);

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
  rendergrid.resize(m_render3dwidth+1);
  for(auto& i : rendergrid)
  {
    i.resize(m_render3dheight+1);
    for(auto& j : i)
    {
      j.resize(m_render3dwidth+1,0.0f);
    }
  }

  float rendersquare=m_squaresize/m_render3dresolution;

  for(int i=0; i<m_lastTakenParticle+1; ++i)
  {
    if(m_particles[i].getAlive()&&(m_particles[i].getProperties()==p))
    {
      Vec3 heightwidthdepth = getGridXYZ(m_particles[i].getGridPosition())*m_render3dresolution; // 3Dify this

      for(int x = -2*m_render3dresolution; x<=4*m_render3dresolution; ++x)
      {
        for(int y = -2*m_render3dresolution; y<=4*m_render3dresolution ; ++y)
        {
          for(int z = -2*m_render3dresolution; z<=4*m_render3dresolution ; ++z)
          {

            //std::cout<<"hello"<<std::endl;
            int currentcolumn=heightwidthdepth[0]+x;
            int currentrow=heightwidthdepth[1]+y;
            int currentdepth=heightwidthdepth[2]+z;

            if(currentcolumn<m_render3dwidth && currentcolumn>0 &&
               currentrow<m_render3dheight && currentrow>0 &&
               currentdepth<m_render3dwidth && currentdepth>0)
            {
              float currentx = rendersquare*(float)currentcolumn - m_halfwidth;
              float currenty = rendersquare*(float)currentrow - m_halfheight;
              float currentz = rendersquare*(float)currentdepth - 2 - m_halfwidth;

              float metaballx = currentx-m_particles[i].getPosition()[0];
              float metabally = currenty-m_particles[i].getPosition()[1];
              float metaballz = currentz-m_particles[i].getPosition()[2];

              float metaballfloat = (m_interactionradius*m_interactionradius)/(metaballx*metaballx + metabally*metabally + metaballz*metaballz);
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
  int z = floor(k/(m_gridwidth*m_gridheight));
  int y = floor((k - z*m_gridwidth*m_gridheight)/m_gridwidth);
  int x = k - y*m_gridwidth - z*m_gridwidth*m_gridheight;

  return Vec3(x,y,z);
}

int World::getSnapshotMode()
{
  return m_marching.getSnapshotMode();
}

void World::drawLoading()
{

  /// The following section is modified from :-
  /// Tim Jones (2011). SDL Tip - SDL Surface to OpenGL Texture [online]. [Accessed 2016].
  /// Available from: <http://www.sdltutorials.com/sdl-tip-sdl-surface-to-opengl-texture>.
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

  // end of Citation

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
        Particle newparticle = Particle(Vec3(-3.0f+i*0.2f,3.0f-j*0.2f,-2.0f),&m_particleTypes[m_todraw]);
        newparticle.setIsObject();
        insertParticle(newparticle);
      }
    }
  }
}

void World::makeParticlesBig()
{
    m_pointsize=10.f;
    m_squaresize=1.0f;
    m_marching.setSquareSize(1.0f);
    m_interactionradius=1.0f;
    hashParticles();
}

void World::makeParticlesSmall()
{
    m_pointsize=5.0f;
    m_squaresize=0.5f;
    m_marching.setSquareSize(0.5f);
    m_interactionradius=0.5f;
    hashParticles();
}

void World::increase2DResolutionWORLD()
{
  ++m_render2DResolution;
  m_marching.increase2DResolution();
  m_render2dwidth=m_gridwidth*m_render2DResolution;
  m_render2dheight=m_gridheight*m_render2DResolution;
}

void World::decrease2DResolutionWORLD()
{
  m_marching.decrease2DResolution();
  if(m_render2DResolution!=1)
    --m_render2DResolution;
  m_render2dwidth=m_gridwidth*m_render2DResolution;
  m_render2dheight=m_gridheight*m_render2DResolution;
}
