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

Particle::Spring *Particle::getSpring(int i) const
{
  return particlesprings[i];
}

void Particle::addSpring(Spring *newspring)
{
  particlesprings.push_back(newspring);
}

int Particle::springNumber() const
{
  return (int)particlesprings.size();
}

GLfloat Particle::alp() const
{
  return alpha;
}

void Particle::deleteSpring(int s)
{
  bool quit = false;
  for(int i =0; i<(int)particlesprings.size() && !quit; ++i)
  {
    if(particlesprings[i]->indexi == s || particlesprings[i]->indexj == s)
    {
      particlesprings.erase(i);
      quit=true;
    }
  }
}

