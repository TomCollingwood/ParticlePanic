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

#include "ParticleProperties.h"


class Particle
{
public:
  typedef struct spring{int indexi, indexj; GLfloat L; int count; bool alive;} Spring;

  //Particle();
  Particle(const Particle &_p) = default;
  Particle(Vec3 pos=Vec3(), ParticleProperties *_properties=NULL) :
    position(pos),
    velocity(Vec3(0.0f,0.0f,0.0f)),
    force(Vec3()),
    mass(1.0f),
    m_properties(_properties),
    wall(false),
    dragged(false),
    m_isPartOfObject(false),
    m_init(false),
    m_alive(true)
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

  bool operator ==(const Particle &_rhs) const;

  int getGridPosition() const;
  void setGridPosition(int p);

  std::vector<int> particleSprings;

  bool collision(Particle p) const;

  void setDrag(bool drag);
  bool getDrag() const;

  bool getWall() const;
  void setWall(bool newwall);

  void setIsObject();
  bool isObject();
  void setInit();
  bool isInit();

  void setAlive(bool i);
  bool isAlive();

  void setIndex(int i);
  int getIndex();

  void updateSpringIndex(int from, int to);

  ParticleProperties *getProperties() const;

//  void updateDeathTime();
//  void setColour(Colour newcolour);

private:
  Vec3 position;
  Vec3 prevPosition;
  int gridPosition;
  Vec3 velocity;
  Vec3 force;
  GLfloat mass;
  ParticleProperties *m_properties;

  bool wall;

  bool m_isPartOfObject;
  bool m_init;
  bool dragged;
  bool m_alive;
  int m_index;

  //std::vector<Spring *> particleSprings;

//  GLfloat rotation;
//  Colour particleColour;
//  int timeToDeath;
//  ParticleProperties* system;
};

#endif
