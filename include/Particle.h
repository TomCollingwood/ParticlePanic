/// \file Particle.h
/// \brief Particle class that includes all attributes of the particle
/// \author Thomas Collingwood
/// \version 1.0
/// \date 26/4/16 Updated to NCCA Coding standard
/// Revision History : See https://github.com/TomCollingwood/ParticlePanic

#ifndef _PARTICLE_H_
#define _PARTICLE_H_
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
    m_position(pos),
    m_velocity(Vec3(0.0f,0.0f,0.0f)),
    m_properties(_properties),
    m_wall(false),
    m_dragged(false),
    m_isPartOfObject(false),
    m_init(false),
    m_alive(true)
    {}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief drawParticle   draws the particle with gluSphere
  /// \param[in] _pointsize size of sphere
  //----------------------------------------------------------------------------------------------------------------------
  void drawParticle(const float _pointsize);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setPosition  sets the Vec3 position of the particle
  /// \param[in] pos      position to be set to
  //----------------------------------------------------------------------------------------------------------------------
  void setPosition(const Vec3 _pos);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getPosition  returns the value of the Vec3 position of the particle
  /// \return             Vec3 position of particle
  //----------------------------------------------------------------------------------------------------------------------
  Vec3 getPosition() const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief addPosition      adds a Vec3 to current position
  /// \param[in] _pos         Vec3 to add to particle's position
  /// \param[in] _halfheight  makes sure particle does not leave the boundaries
  /// \param[in] _halfwidth   makes sure particle does not leave the boundaries
  //----------------------------------------------------------------------------------------------------------------------
  void addPosition(const Vec3 _pos, const float _halfheight, const float _halfwidth, bool is3D);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief updatePrevPosition updates the previous position of the particle to current position
  //----------------------------------------------------------------------------------------------------------------------
  void updatePrevPosition();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getPrevPosition  returns the previous position of the particle. Is needed to calculate particle's velocity.
  /// \return                 Vec3 of previous position
  //----------------------------------------------------------------------------------------------------------------------
  Vec3 getPrevPosition() const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setVelocity  sets the Vec3 velocity of the particle
  /// \param[in] _newvel   Vec3 velocity to set particle's velocity
  //----------------------------------------------------------------------------------------------------------------------
  void setVelocity(const Vec3 _newvel);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getVelocity  returns the current velocity of the particle
  /// \return             Vec3 velocity of particle
  //----------------------------------------------------------------------------------------------------------------------
  Vec3 getVelocity() const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief addVelocity    adds a Vec3 to current velocity of particle
  /// \param[in] _addedvel  Vec3 to add to particle's velocity
  //----------------------------------------------------------------------------------------------------------------------
  void addVelocity(const Vec3 addedvel);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief updatePosition   updates the position according to particle's velocity
  /// \param[in] _elapsedtime multiplies the velocity according to this timestep
  /// \param[in] _halfheight  makes sure particle does not leave the boundaries
  /// \param[in] _halfwidth   makes sure particle does not leave the boundaries
  //----------------------------------------------------------------------------------------------------------------------
  void updatePosition(double elapsedtime, float halfheight, float halfwidth, bool is3D);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getGridPosition  returns the spatial hash grid index
  /// \return                 spatial hash index for particle
  //----------------------------------------------------------------------------------------------------------------------
  int getGridPosition() const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setGridPosition  sets the value inside the particle representing the spatial hash grid index
  /// \param[in] _p           int to set the index to
  //----------------------------------------------------------------------------------------------------------------------
  void setGridPosition(int p);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief particleSprings  vector of spring indexes that the particle is attached to.
  ///                         It is public as we need to loop through these inside of world.
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<int> m_particleSprings;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setDrag    sets the drag bool which shows whether the particle is being dragged by the mouse or not
  /// \param[in] _drag  the bool to set m_drag to
  //----------------------------------------------------------------------------------------------------------------------
  void setDrag(bool drag);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getDrag  sets the drag bool which shows whether the particle is being dragged by the mouse or not
  /// \return         drag bool
  //----------------------------------------------------------------------------------------------------------------------
  bool getDrag() const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getWall  returns the bool that shows whether the particle is static or not
  /// \return         the wall bool
  //----------------------------------------------------------------------------------------------------------------------
  bool getWall() const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setWall  returns the bool that shows whether the particle is static or not
  /// \param newwall  the bool to set m_wall to
  //----------------------------------------------------------------------------------------------------------------------
  void setWall(bool newwall);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setIsObject  sets the object bool on the particle.
  ///                     This means it keeps it's springs and makes no new one.
  //----------------------------------------------------------------------------------------------------------------------
  void setIsObject();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief isObject returns the object bool on the particle.
  /// \return         bool that shows whether particle is an object
  //----------------------------------------------------------------------------------------------------------------------
  bool isObject();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setInit  m_init is set when the particle's springs have been made.
  ///                 Is used only when particle is part of an object (m_isPartOfObject==true);
  //----------------------------------------------------------------------------------------------------------------------
  void setInit();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief isInit returns the value of m_init
  /// \return       m_init bool value
  //----------------------------------------------------------------------------------------------------------------------
  bool isInit();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setAlive sets m_alive to new bool.
  /// \param[in] _i   bool to set m_alive to
  //----------------------------------------------------------------------------------------------------------------------
  void setAlive(bool _i);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getAlive returns value of m_alive
  /// \return         m_alive value
  //----------------------------------------------------------------------------------------------------------------------
  bool getAlive();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setIndex sets m_index which is index of particle inside world's vector m_particles
  /// \param[in] _i   new value of m_index to be set
  //----------------------------------------------------------------------------------------------------------------------
  void setIndex(int _i);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getIndex returns m_index which is index of particle inside world's vector m_particles
  /// \return         bool value of m_index
  //----------------------------------------------------------------------------------------------------------------------
  int getIndex();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief updateSpringIndex  checks if there is a spring with index from and if so changes it to to
  ///                           if to == -1 then it deletes that spring index from particleSprings
  /// \param[in] _from          index of spring to change
  /// \param[in] _to            index of spring to change to
  //----------------------------------------------------------------------------------------------------------------------
  void updateSpringIndex(int _from, int _to);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getProperties  returns pointer to the ParticleProperties object the particle has
  /// \return               *ParticleProperties of the particle
  //----------------------------------------------------------------------------------------------------------------------
  ParticleProperties *getProperties() const;

private:
  Vec3 m_position;
  Vec3 m_prevPosition;
  int m_gridPosition;
  Vec3 m_velocity;

  ParticleProperties *m_properties;

  bool m_wall;

  bool m_isPartOfObject;
  bool m_init;
  bool m_dragged;
  bool m_alive;
  int m_index;

  //std::vector<Spring *> particleSprings;

//  GLfloat rotation;
//  Colour particleColour;
//  int timeToDeath;
//  ParticleProperties* system;
};

#endif
