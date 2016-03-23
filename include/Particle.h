#ifndef PARTICLE
#define PARTICLE
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include "Vec3.h"

class Particle
{
public:
  //Particle();
  Particle(const Particle &_p) = default;
  Particle(Vec3 pos=Vec3() /*,ParticleProperties prop*/) :
    position(pos),
    velocity(Vec3()),
    force(Vec3()),
    mass(1.0f)
/*  system(prop),
    rotation(0.0f),
    timeToDeath(-1),
    particleColour(Colour(0.0f,0.0f,0.0f))*/{}

  void drawParticle();

  void setPosition(Vec3 pos);
  Vec3 getPosition() const;

  void clearForces();
  Vec3 getForce() const;
  void setForce(Vec3 newforce);
  void addForce(Vec3 addedforce);
  void applyGravity();

  void setVelocity(Vec3 newvel);
  Vec3 getVelocity() const;

  void updateVelocity();
  void updatePosition();

  bool collision(Particle p) const;


//  void updateDeathTime();
//  void setColour(Colour newcolour);
private:
  Vec3 position;
  Vec3 velocity;
  Vec3 force;
  GLfloat mass;

//  GLfloat rotation;
//  Colour particleColour;
//  int timeToDeath;
//  int* connected[4];
//  ParticleProperties* system;
};

#endif
