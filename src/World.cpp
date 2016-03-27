#include "include/World.h"

// Use this to keep track of the times
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>

/**
 * @brief World::World
 */
World::World() :
  m_isInit(false),
  m_startTime(0.0),
  m_elapsedTime(0.0),
  interactionradius(0.5f),
  squaresize(0.5f),
  m_timestep(0.001),
  pointsize(20.0f),
  renderthreshold(3000.0f),
  renderresolution(10),
  renderoption(1)
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

    // create start two blocks of particles
    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
        particles.push_back(Particle(Vec3(-3.0f+i*0.1f,3.0f-j*0.1f)));
    }
    //for(int a=0; a<10*10; ++a)
    //{
      //particles[a].setVelocity(Vec3(-8.0f,0.0f));
      //particles[a].addVelocity(Vec3(0.0f,(float)(rand() % 100 - 50)*0.01f));
      //particles[a].setVelocity(Vec3(((float)(rand() % 100 - 50)),((float)(rand() % 10000 - 50))));
    //}


    for(int i = 0; i<10; ++i)
    {
      for(int j=0; j<10; ++j)
        particles.push_back(Particle(Vec3(3.0f+i*0.2f,3.0f-j*0.2f)));
    }

    for(auto& i : particles)
      i.addVelocity(Vec3(((float)(rand() % 100 - 50)*0.001f),((float)(rand() % 10000 - 50))*0.001f));

    previousmousex=-10;
    previousmousey=-10;

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

  pixelheight=h;
  pixelwidth=w;

  float i = 5;
  float ara = float(w)/float(h);
  glOrtho(-i*ara,i*ara,-i,i,0.1, 10.0);

  halfheight=i;
  halfwidth=i*ara;

  glViewport(0,0,w,h);

  glMatrixMode(GL_MODELVIEW);

  gridwidth=ceil((halfwidth*2)/squaresize);
  gridheight=ceil((halfheight*2)/squaresize);

  renderwidth=gridwidth*renderresolution;
  renderheight=gridheight*renderresolution;

  grid.resize(gridheight*gridwidth);

  cellsContainingParticles.clear();
  cellsContainingParticles.resize(gridheight*gridwidth,false);

  hashParticles();

  renderGrid.clear();
  renderGrid.resize(renderheight+1);
  for(auto& i : renderGrid)
  {
    i.resize(renderwidth+1,0.0f);
  }

  std::cout<<"HELLO THERE"<<std::endl;
}

/**
 * @brief World::draw draws the World to the current GL context. Called a lot - make this fast!
 */
void World::draw() {
    if (!m_isInit) return;

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

            if(currentcolumn<renderwidth+1 && currentcolumn>0 &&
               currentrow<renderheight+1 && currentrow>0)
            {
              float currentx = squaresize/renderresolution*(float)currentcolumn - halfwidth;
              float currenty = squaresize/renderresolution*(float)currentrow - halfheight;

              float metaballx = currentx-i.getPosition()[0];
              float metabally = currenty-i.getPosition()[1];

              float metaballfloat = 40.f/(metaballx*metaballx + metabally*metabally);

              renderGrid[currentrow][currentcolumn]+=metaballfloat;

              //            glBegin(GL_POINTS);
              //            glColor3f(1.0f,1.0f,1.0f);
              //            glVertex3f(currentx,currenty,-2.0f);
              //            glEnd();
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


          std::vector<bool> boolpoints;
          boolpoints.push_back(renderGrid[currentrow][currentcolumn]>renderthreshold);
          boolpoints.push_back(renderGrid[currentrow][currentcolumn+1]>renderthreshold);
          boolpoints.push_back(renderGrid[currentrow+1][currentcolumn]>renderthreshold);
          boolpoints.push_back(renderGrid[currentrow+1][currentcolumn+1]>renderthreshold);

          //std::cout<<"p1x:"<<p1x<<std::endl;
          //std::cout<<"p1y:"<<p1y<<std::endl;

          //        glBegin(GL_POINTS);
          //        glColor3f(1.0f,1.0f,1.0f);
          //        glVertex3f(p1x,p1y,-2.0f);
          //        glEnd();


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
            //std::cout<<"hi mom";
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
      //GRID2


      //clear render grid

      for(auto& i : renderGrid)
      {
        i.assign(renderwidth+1,0.0f);
      }
      // */

    }


    struct timeval tim;
    gettimeofday(&tim, NULL);
    static double now = tim.tv_sec+(tim.tv_usec * 1e-6);
    static double now2 = tim.tv_sec+(tim.tv_usec * 1e-6);

    now2=now;
    now=tim.tv_sec+(tim.tv_usec * 1e-6);

    double calcTimeStep = now-now2;
    //if(calcTimeStep==0) calcTimeStep=0.03;

    static double max = calcTimeStep;
    if(calcTimeStep>max) max=calcTimeStep;
    //std::cout<<"o"<<max<<std::endl;
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

    static int everyother = 0;
    everyother++;

    int divisor = 5; //(int)(0.00005f/m_timestep);
    if(divisor==0) divisor=1;
    if(everyother%5==0){
      for(int i = 0; i<5; ++i)
      {
        particles.push_back(Particle(Vec3(-3.0f+i*0.4f,4.4f)));
        particles.back().addVelocity(Vec3(0.0f,-30.0f));
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
        std::vector<Particle *> surroundingParticles = getSurroundingParticles(choo,1);
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
              //std::cout<<"->>"<<impulse.length()<<std::endl;
              //printf("(%f)",u);
              if(u>0)
              {
                float sig = i->sig();
                float bet = i->bet();
                Vec3 impulse = rij*((1-q)*(sig*u + bet*u*u))*m_timestep;
                //std::cout<<"-p>>"<<impulse.length()<<std::endl;
                i->addVelocity(-impulse/2.0f);
                j->addVelocity(impulse/2.0f);
              }
            }

          }
          cloo++;
        }
        ploo++;
      }
      choo++;
    }
    // */

    //------------------------------------------POSITION----------------------------------------

    for(auto& i : particles)
    {
      i.updatePrevPosition();
      if(!(i.getDrag())) i.updatePosition(m_timestep);
    }
    hashParticles();

    //--------------------------------------SPRINGS-----------------------------------------------


    /*
    for(int k=0; k<gridheight*gridwidth; ++k)
    {
      if(cellsContainingParticles[k])
      {
        //std::cout<<"not now"<<std::endl;
        std::vector<Particle *> surroundingParticles = getSurroundingParticles(k,1);
        // std::cout<<(int)surroundingParticles.size()<<std::endl;

        //std::cout<<"START";
        for(auto& i : grid[k])
        {
          //std::cout<<"("<<surroundingParticles[i]->getPosition()[0]<<")"<<std::endl;
          for(auto& j : surroundingParticles)
          {
            Vec3 rij=(j->getPosition()-i->getPosition());
            float rijmag = rij.length();
            //std::cout<<rijmag<<std::endl;
            float q = rijmag/interactionradius;

            if(q<1 && q!=0)
            {
              bool quiter = false;
              Particle::Spring *thisspring;

              for(auto& spring : i->particleSprings)
              {
                if(((spring->indexi==i) && (spring->indexj==j)) ||
                   ((spring->indexi==j) && (spring->indexj==i)))
                {
                  thisspring=(spring);
                  quiter=true;
                  //std::cout<<"FUCK"<<std::endl;
                  break;
                }

              }


              if(!quiter)
              {
                Particle::Spring newspring;
                newspring.indexi=i;
                newspring.indexj=j;
                newspring.count=everyother-1;
                newspring.L = interactionradius; // maybe change this to sum of radius of two particles

                springs.push_back(newspring);
                thisspring = &(springs.back());

                i->particleSprings.push_back(thisspring);
                j->particleSprings.push_back(thisspring);

                //thisspring = &newspring; // delete this
              }

              if(thisspring->count!=everyother)
              {
                GLfloat L = thisspring->L;
                GLfloat d= L*i->gam();
                GLfloat alpha = i->alp();

                if(rijmag>L+d)
                {
                  thisspring->L=L+m_timestep*alpha*(rijmag-L-d);
                  //std::cout<<"Heyboss"<<std::endl;
                }
                else if(rijmag<L-d)
                {
                  thisspring->L=L-m_timestep*alpha*(L-d-rijmag);
                  //std::cout<<"Heyboss"<<std::endl;
                }
                thisspring->count++;
              }
            }
            //
          }
        }
      }
    }
    //

    std::cout<<"s"<<(int)springs.size()<<"p"<<(int)particles.size()<<std::endl;
    // delete springs if over rest length?


    choo=0;
    for(auto& i : springs)
    {
      if(i.L>interactionradius)
      {
        std::list<Particle::Spring>::iterator it;
        it = springs.begin();
        advance(it,choo);
        int hoho=0;
        for(auto& a : (*it).indexi->particleSprings)
        {
          if(a==&i)
            (*it).indexi->particleSprings.erase((*it).indexj->particleSprings.begin()+hoho);
          hoho++;
        }
        hoho=0;
        for(auto& b : (*it).indexj->particleSprings)
        {
          if(b==&i)
            (*it).indexj->particleSprings.erase((*it).indexj->particleSprings.begin()+hoho);
          hoho++;
        }
        springs.erase(it);
      }
      else{choo++;}
    }

    //
    //spring displacements


    for(auto& i : springs)
    {
      Vec3 rij = (*(i.indexj)).getPosition() - (*(i.indexi)).getPosition();
      float rijmag = rij.length();

      rij.normalize();
      Vec3 D = rij*m_timestep*1000.f*(1-(i.L/interactionradius))*(i.L-rijmag);
      //if((1-(springs[i].L/interactionradius))!=0) std::cout<<(1-(springs[i].L/interactionradius))<<"<---"<<std::endl;
      i.indexi->addPosition(-D/2);
      i.indexj->addPosition(D/2);
    }
    // */

    //----------------------------------DOUBLEDENSITY------------------------------------------


    int count =0;

    for(auto& list : grid)
    {
      std::vector<Particle *> neighbours=getSurroundingParticles(count,1);
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
        std::list<Particle>::iterator it = particles.begin(); //EAGLE
        advance(it, count);
        particles.erase(it);
      }
      else
        ++count;
      // */


      if(it.getPosition()[1]-0.5f<-halfheight)
      {
        it.setPosition(Vec3(it.getPosition()[0],-halfheight+0.5f));
        it.addVelocity(Vec3(0.0f,-1.1f*it.getVelocity()[1]));
      }

      if(it.getPosition()[1]+0.5f>halfheight)
      {
        it.setPosition(Vec3(it.getPosition()[0],halfheight-0.5f));
        it.addVelocity(Vec3(0.0f,-1.1f*it.getVelocity()[1]));
      }

      if(it.getPosition()[0]>halfwidth-0.5f)
      {
        it.setPosition(Vec3(halfwidth-0.5f,it.getPosition()[1]));
        it.addVelocity(Vec3(-1.1f*it.getVelocity()[0],0.0f));
      }

      if(it.getPosition()[0]<-halfwidth+0.5f)
      {
        it.setPosition(Vec3(-halfwidth+0.5f,it.getPosition()[1]));
        it.addVelocity(Vec3(-1.1f*it.getVelocity()[0],0.0f));
      }
      //*/
    }
    //----------------------------------CLEANUP ------------------------------------------------

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

void World::hashParticles()
{
  cellsContainingParticles.assign(gridwidth*gridheight,false);
  std::vector<Particle *> newvector;
  grid.assign(gridheight*gridwidth,newvector);
  int grid_cell;
  for(auto& i : particles)
  {
    grid_cell=floor((i.getPosition()[0]+halfwidth)/squaresize)+floor((i.getPosition()[1]+halfheight)/squaresize)*gridwidth;
    i.setGridPosition(grid_cell);
    if(grid_cell>=0 && grid_cell<gridheight*gridwidth)
    {
      cellsContainingParticles[grid_cell]=true;
      //std::cout<<"THIS ONE!"<<(&i)->getPosition()[1]<<std::endl;
      grid[grid_cell].push_back(&i);
    }
  }
}

std::vector<Particle *> World::getSurroundingParticles(int thiscell, int numsur) const
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


Vec3 World::getGridColumnRow(int k)
{
  int row = floor(k/gridwidth);
  int column = k - row*gridwidth;
  return Vec3(column,row);
}

Vec3 World::getRenderGridColumnRow(int k)
{
  //std::cout<<"k:"<<k<<std::endl;
  //std::cout<<"gridwidth:"<<gridwidth<<std::endl;
  //std::cout<<"gridheight:"<<gridheight<<std::endl;
  int row = floor(k/((gridwidth+1)*2));
  int column = k - row*((gridwidth+1)*2);
  //std::cout<<"row:"<<row<<std::endl;
  return Vec3(column, row);
}

Vec3 World::getRenderGridxy(int h, int w) //wrong
{
  float x = w*(squaresize/2) - halfwidth;
  float y = h*(squaresize/2) - halfheight;
  return Vec3(x,y);
}

Vec3 World::getRenderGridxyfromIndex(int k) //wrong
{
  //std::cout<<"k:"<<k<<std::endl;
  Vec3 temp= getRenderGridColumnRow(k);
  return getRenderGridxy(temp[1],temp[0]); // wrong
}

void World::mouseDraw(int x, int y)
{
  //if(x%2==0 && y%2==0)
  //{
    //std::cout<<"yoo"<<std::endl;
    float currentx = ((float)x/(float)pixelwidth)*(halfwidth*2) - halfwidth;
    float currenty = -((float)y/(float)pixelheight)*(halfheight*2) + halfheight;
    //std::cout<<"x"<<x<<"y"<<y<<std::endl;
    //particles.push_back(Particle(Vec3(currentx,currenty)));
    particles.push_back(Particle(Vec3(currentx,currenty)));
  //}

}

void World::vectorvslist()
{
  std::vector<Particle> vectorparticles;
  for(int i = 0 ; i<800 ; ++i)
  {
    vectorparticles.push_back(Particle(Vec3()));
  }

  std::list<Particle> listparticles;
  for(int i = 0 ; i<800 ; ++i)
  {
    listparticles.push_back(Particle(Vec3()));
  }

  struct timeval tim;
  gettimeofday(&tim, NULL);
  double now = tim.tv_sec+(tim.tv_usec * 1e-6);

  for(auto& i : vectorparticles)
  {
    i.addPosition(Vec3(-0.5f,2.0f));
  }

  gettimeofday(&tim, NULL);
  double now2 = tim.tv_sec+(tim.tv_usec * 1e-6);
  std::cout<<now2-now<<std::endl;
}

void World::mouseDrag(int x, int y)
{
  if(previousmousex>0 && previousmousey>0)
  {
    float toaddx = (x-previousmousex)*((halfwidth*2)/(float)pixelwidth);
    float toaddy = (y-previousmousey)*((halfwidth*2)/(float)pixelwidth);
    std::cout<<toaddx<<std::endl;
    for(auto& i : draggedParticles)
    {
      i->addPosition(Vec3(toaddx,-toaddy));
      getbackhere(&(*i));
    }
    hashParticles();
  }
  previousmousex=x;
  previousmousey=y;
}

void World::selectDraggedParticles(int x, int y)
{
  float worldx = ((float)x/(float)pixelwidth)*(halfwidth*2) - halfwidth;
  float worldy = -((float)y/(float)pixelheight)*(halfheight*2) + halfheight;
  int grid_cell=floor((worldx+halfwidth)/squaresize)+floor((worldy+halfheight)/squaresize)*gridwidth;
  draggedParticles = getSurroundingParticles(grid_cell,2);
  for(auto& i : draggedParticles)
  {
    i->setDrag(true);
  }
}

void World::getbackhere(Particle * p)
{
  if(p->getPosition()[0]>halfwidth-0.5f) p->getPosition()[0]=halfwidth-0.5f;
  else if(p->getPosition()[0]<-halfwidth+0.5f) p->getPosition()[0]=-halfwidth+0.5f;
  if(p->getPosition()[1]>halfheight-0.5f) p->getPosition()[1]=halfheight-0.5f;
  else if(p->getPosition()[1]<-halfheight+0.5f) p->getPosition()[1]=-halfheight+0.5f;
}

void World::clearDraggedParticles()
{
  previousmousex=-10;
  previousmousey=-10;
  for(auto& i : draggedParticles)
  {
    i->setDrag(false);
  }
  draggedParticles.clear();
}
