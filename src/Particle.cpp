#include "include/Particle.h"

void Particle::drawParticle()
{

  glBegin(GL_POINTS);
  glColor3f(1.0f,1.0f,1.0f);
  glVertex3f(position[0],position[1],-2.0f);
  glEnd();
}

void Particle::updateVelocity()
{
  velocity+=(force/mass);
}

void Particle::updatePosition()
{
  position+=velocity;
}

Vec3 Particle::getPosition() const
{
  return position;
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
  force+=Vec3(0.0f,-0.0008f);
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

  if(difference[0]<0.0005f && difference[1]<0.0005f)
    return true;
  else return false;
}

