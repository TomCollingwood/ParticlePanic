#include "include/Particle.h"

void Particle::drawParticle()
{
  glBegin(GL_POINTS);
  glColor3f(1.0f,1.0f,1.0f);
  glVertex3f(position[0],position[1],-2.0f);
  glEnd();
}

void Particle::updateVelocity(double elapsedtime)
{
  velocity+=(force/mass)*elapsedtime;
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

void Particle::applyGravity()
{
  force+=Vec3(0.0f,-1.0f);
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

GLfloat Particle::sig() const
{
  return sigma;
}

GLfloat Particle::bet() const
{
  return beta;
}

void Particle::addVelocity(Vec3 addedvel)
{
  velocity+=addedvel;
}

GLfloat Particle::alp() const
{
  return alpha;
}

GLfloat Particle::gam() const
{
  return gamma;
}

void Particle::addPosition(Vec3 pos)
{
  position+=pos;
}

void Particle::updatePrevPosition()
{
  prevPosition=Vec3(position[0],position[1]);
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
