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


class Particle
{
public:

  typedef struct spring{GLfloat indexi, indexj, L;} Spring;

  //Particle();
  Particle(const Particle &_p) = default;
  Particle(Vec3 pos=Vec3() /*,ParticleProperties prop*/) :
    position(pos),
    velocity(Vec3()),
    force(Vec3()),
    mass(1.0f),
    sigma(0.4f),
    beta(0.4f),
    gamma()
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
  void addVelocity(Vec3 addedvel);

  void updateVelocity();
  void updatePosition();

  GLfloat sig() const;
  GLfloat bet() const;
  GLfloat gam() const;
  GLfloat alp() const;

  bool collision(Particle p) const;

  Spring *getSpring(int i) const;
  void addSpring(Spring *newspring);
  int springNumber() const;
  void deleteSpring(int s); // deletes spring with indexi or indexj == i


//  void updateDeathTime();
//  void setColour(Colour newcolour);
private:
  Vec3 position;
  Vec3 velocity;
  Vec3 force;
  GLfloat mass;
  GLfloat sigma;
  GLfloat beta;
  GLfloat gamma;
  GLfloat alpha;
  std::vector<Spring *> particlesprings;

//  GLfloat rotation;
//  Colour particleColour;
//  int timeToDeath;
//  int* connected[4];
//  ParticleProperties* system;
};

#endif
