#include "include/Particle.h"
//#include "GLUT/glut.h"

void Particle::drawParticle()
{
  //glutBitmapCharacter(GLUT_BITMAP_9_BY_15,'a');
  //glBegin(GL_POINTS);
  if(!wall)
  {
    float fast=velocity.length()*5;
    if(fast>1.0f) fast=1.0f;
    //glColor3f((float)(rand() &100) /100.0f,(float)(rand() &100) /100.0f,(float)(rand() &100) /100.0f); //funky
    if(m_properties->getColourEffect())
      glColor3f(m_properties->getRed()+fast,m_properties->getGreen()+fast,m_properties->getBlue()+fast);
    else
      glColor3f(m_properties->getRed(),m_properties->getGreen(),m_properties->getBlue());

    //glColor3f(0.8f,0.5f,0.2f);
  }
  else
  {
    glColor3f(1.0f,0.0f,0.0f);
  }

  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  glTranslatef(position[0], position[1], position[2]); // move back to focus of gluLookAt

  GLUquadricObj *quadric;
  quadric = gluNewQuadric();
  gluQuadricDrawStyle(quadric, GLU_FILL );
  gluSphere( quadric , 0.25 , 16 , 16 );
  gluDeleteQuadric(quadric);

  glPopMatrix();

  //glVertex3f(position[0],position[1],position[2]);
  //glEnd();
}

void Particle::updatePosition(double elapsedtime)
{
  position+=velocity*elapsedtime;
}

Vec3 Particle::getPosition() const
{
  return position;
}

void Particle::setPosition(Vec3 pos)
{
  position=pos;
}

void Particle::setVelocity(Vec3 newvel)
{
  velocity = newvel;
}

Vec3 Particle::getVelocity() const
{
  return velocity;
}

void Particle::clearForces()
{
  force=Vec3();
}

void Particle::applyGravity(float m_timestep)
{
  velocity+=Vec3(0.0f,-0.008*m_timestep,0.0f);
}

void Particle::setForce(Vec3 newforce)
{
  force=newforce;
}

void Particle::addForce(Vec3 addedforce)
{
  force+=addedforce;
}

Vec3 Particle::getForce() const
{
  return force;
}

bool Particle::collision(Particle p) const
{
  Vec3 difference = position - p.getPosition();

  if(std::abs(difference[0])<0.5f && std::abs(difference[1])<0.5f)
    return true;
  else return false;
}

void Particle::addVelocity(Vec3 addedvel)
{
  velocity+=addedvel;
}


void Particle::addPosition(Vec3 pos)
{
  position+=pos;
}

void Particle::updatePrevPosition()
{
  prevPosition=Vec3(position[0],position[1],position[2]);
}

Vec3 Particle::getPrevPosition() const
{
  return prevPosition;
}

bool Particle::operator ==(const Particle &_rhs) const
{
  if(position==_rhs.position && velocity==_rhs.velocity) return true;
  else return false;
}

void Particle::setGridPosition(int p)
{
  gridPosition=p;
}

int Particle::getGridPosition() const
{
  return gridPosition;
}

void Particle::setDrag(bool drag)
{
  dragged=drag;
}

bool Particle::getDrag() const
{
  return dragged;
}

bool Particle::getWall() const
{
  return wall;
}

void Particle::setWall(bool newwall)
{
  wall=newwall;
}

ParticleProperties *Particle::getProperties() const
{
  return m_properties;
}

void Particle::setIsObject()
{
  m_isPartOfObject=true;
}

void Particle::setInit()
{
  m_init=true;
}

bool Particle::isInit()
{
  return m_init;
}

bool Particle::isObject()
{
  return m_isPartOfObject;
}

void Particle::setAlive(bool i)
{
  m_alive=i;
}

bool Particle::isAlive()
{
  return m_alive;
}

void Particle::setIndex(int i)
{
  m_index=i;
}

int Particle::getIndex()
{
  return m_index;
}

void Particle::updateSpringIndex(int from, int to)
{
  for(int i=0; i<(int)particleSprings.size(); ++i)
  {
    if(particleSprings[i]==from)
    {
      if(to>-1) particleSprings[i]=to;
      else particleSprings.erase(particleSprings.begin()+i);
      break;
    }
  }
}

void Particle::clearParticleSprings()
{
  particleSprings.clear();
}
