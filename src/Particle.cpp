//
///  @file Particle.cpp
///  @brief Particle class that includes all attributes of the particle

#include "include/Particle.h"

void Particle::drawParticle(const float _pointsize)
{
  if(!m_wall)
  {
    float fast=m_velocity.length()*5;

    if(fast>1.0f) fast=1.0f;

    if(m_properties->getColourEffect())
      glColor3f(m_properties->getRed()+fast,m_properties->getGreen()+fast,m_properties->getBlue()+fast);
    else
      glColor3f(m_properties->getRed(),m_properties->getGreen(),m_properties->getBlue());
  }
  else
  {
    glColor3f(1.0f,0.0f,0.0f);
  }

  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  glTranslatef(m_position[0], m_position[1], m_position[2]);

  GLUquadricObj *quadric;
  quadric = gluNewQuadric();
  gluQuadricDrawStyle(quadric, GLU_FILL );
  gluSphere( quadric , 0.25*(_pointsize/10.f) , 16 , 16 );
  gluDeleteQuadric(quadric);

  glPopMatrix();
}

void Particle::updatePosition(double _elapsedtime, float _halfheight, float _halfwidth, bool is3D)
{
  m_position+=m_velocity*_elapsedtime;

  float smallen = 1.0f;
  if(is3D) smallen = 0.4f;

  if(m_position[0]>(_halfwidth-0.5f)*smallen)
  {
    m_position[0] = (_halfwidth-0.5f)*smallen;
  }
  else if(m_position[0]<(-_halfwidth+0.5f)*smallen)
  {
    m_position[0]= (-_halfwidth+0.5f)*smallen;
  }
  if(m_position[1]<-_halfheight+0.5f)
  {
    m_position[1]=-_halfheight+0.5f;
  }
  else if (m_position[1]>_halfheight-1.5f)
  {
    m_position[1]=_halfheight-1.5f;
  }
  if(m_position[2]>(_halfwidth-0.5f))
  {
    m_position[2] = (_halfwidth-0.5f)*smallen;
  }
  else if(m_position[2]<(-_halfwidth+0.5f))
  {
    m_position[2]= (-_halfwidth+0.5f)*smallen;
  }
  // */
}

Vec3 Particle::getPosition() const
{
  return m_position;
}

void Particle::setPosition(Vec3 _pos)
{
  m_position=_pos;
}

void Particle::setVelocity(Vec3 _newvel)
{
  m_velocity = _newvel;
}

Vec3 Particle::getVelocity() const
{
  return m_velocity;
}

void Particle::addVelocity(Vec3 _addedvel)
{
  m_velocity+=_addedvel;
}


void Particle::addPosition(Vec3 _pos, float _halfheight, float _halfwidth, bool is3D)
{
  float smallen = 1.0f;
  if(is3D) smallen = 0.4f;

  m_position+=_pos;

  if(m_position[0]>(_halfwidth-0.5f)*smallen)
  {
    m_position[0] = (_halfwidth-0.5f)*smallen;
  }
  else if(m_position[0]<(-_halfwidth+0.5f)*smallen)
  {
    m_position[0]= (-_halfwidth+0.5f)*smallen;
  }
  if(m_position[1]<-_halfheight+0.5f)
  {
    m_position[1]=-_halfheight+0.5f;
  }
  else if (m_position[1]>_halfheight-1.5f)
  {
    m_position[1]=_halfheight-1.5f;
  }
  if(m_position[2]>(_halfwidth-0.5f)*smallen)
  {
    m_position[2] = (_halfwidth-0.5f)*smallen;
  }
  else if(m_position[2]<(-_halfwidth+0.5f)*smallen)
  {
    m_position[2]= (-_halfwidth+0.5f)*smallen;
  }
  // */
}

void Particle::updatePrevPosition()
{
  m_prevPosition=Vec3(m_position[0],m_position[1],m_position[2]);
}

Vec3 Particle::getPrevPosition() const
{
  return m_prevPosition;
}

void Particle::setGridPosition(int _p)
{
  m_gridPosition=_p;
}

int Particle::getGridPosition() const
{
  return m_gridPosition;
}

void Particle::setDrag(bool _drag)
{
  m_dragged=_drag;
}

bool Particle::getDrag() const
{
  return m_dragged;
}

bool Particle::getWall() const
{
  return m_wall;
}

void Particle::setWall(bool _newwall)
{
  m_wall=_newwall;
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

void Particle::setAlive(bool _i)
{
  m_alive=_i;
}

bool Particle::getAlive()
{
  return m_alive;
}

void Particle::setIndex(int _i)
{
  m_index=_i;
}

int Particle::getIndex()
{
  return m_index;
}

void Particle::updateSpringIndex(int _from, int _to)
{
  for(int i=0; i<(int)m_particleSprings.size(); ++i)
  {
    if(m_particleSprings[i]==_from)
    {
      if(_to>-1) m_particleSprings[i]=_to;
      else m_particleSprings.erase(m_particleSprings.begin()+i);
      break;
    }
  }
}
