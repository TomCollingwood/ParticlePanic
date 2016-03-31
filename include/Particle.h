#ifndef PARTICLE
#define PARTICLE
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include <cmath>
#include <vector>
#include "Vec3.h"
#include <algorithm>
#include <cstdlib>


class Particle
{
public:
  typedef struct spring{Particle *indexi, *indexj; GLfloat L; int count; bool alive;} Spring;
  //Particle();
  Particle(const Particle &_p) = default;
  Particle(Vec3 pos=Vec3() /*,ParticleProperties prop*/) :
    position(pos),
    velocity(Vec3()),
    force(Vec3()),
    mass(1.0f),
    sigma(0.0f),
    beta(0.2f), //above 0.4 explodes
    gamma(0.004f),
    alpha(0.3f),
    dragged(false),
    wall(false)
/*  system(prop),
    rotation(0.0f),
    timeToDeath(-1),
    particleColour(Colour(0.0f,0.0f,0.0f))*/{}

  void drawParticle();

  void setPosition(Vec3 pos);
  Vec3 getPosition() const;
  void addPosition(Vec3 pos);

  void updatePrevPosition();
  Vec3 getPrevPosition() const;

  void clearForces();
  Vec3 getForce() const;
  void setForce(Vec3 newforce);
  void addForce(Vec3 addedforce);
  void applyGravity(float m_timestep);

  void setVelocity(Vec3 newvel);
  Vec3 getVelocity() const;
  void addVelocity(Vec3 addedvel);

  void updateVelocity(double elapsedtime);
  void updatePosition(double elapsedtime);

  GLfloat sig() const;
  GLfloat bet() const;
  GLfloat gam() const;
  GLfloat alp() const;

  bool operator ==(const Particle &_rhs) const;

  int getGridPosition() const;
  void setGridPosition(int p);

  std::vector<int> particleSprings;

  bool collision(Particle p) const;

  void setDrag(bool drag);
  bool getDrag() const;

  bool getWall() const;
  void setWall(bool newwall);

//  void updateDeathTime();
//  void setColour(Colour newcolour);

private:
  Vec3 position;
  Vec3 prevPosition;
  int gridPosition;
  Vec3 velocity;
  Vec3 force;
  GLfloat mass;
  GLfloat sigma;
  GLfloat beta;
  GLfloat gamma;
  GLfloat alpha;

  bool wall;

  bool dragged;

  //std::vector<Spring *> particleSprings;

//  GLfloat rotation;
//  Colour particleColour;
//  int timeToDeath;
//  ParticleProperties* system;
};

#endif
