#ifndef VEC3
#define VEC3

#ifdef __linux__
  #include <GL/gl.h>
#endif
#ifdef __APPLE__
  #include <OpenGL/gl.h>
#endif

#include <cmath>
#include <cassert>

#include"include/Mat3.h"

class Vec3
{
public:
  Vec3(const Vec3 &_rhs)=default;
  Vec3(GLfloat _x=0.0f,
       GLfloat _y=0.0f,
       GLfloat _z=0.0f) :
    m_x(_x),
    m_y(_y),
    m_z(_z){}

  // @ calc cross product of
  // self and _rhs (self x _rhs)
  float dot(const Vec3 &_rhs) const;
  float length() const;
  float lengthSquared() const;
  void normalize();
  Vec3 cross(Vec3 &_rhs) const;

  void rotateAroundXAxisf(float degrees);

  Vec3 operator *(Mat3 &_rhs);
  Vec3 operator *(GLfloat _rhs) const;
  void operator *=(GLfloat _rhs);
  Vec3 operator /(GLfloat _rhs) const;
  void operator /=(GLfloat _rhs);
  Vec3 operator +(const Vec3 &_r) const;
  void operator +=(const Vec3 &_r);
  Vec3 operator -(const Vec3 &_rhs) const;
  Vec3 operator -();
  void operator -=(const Vec3 &_r);
  bool operator ==(const Vec3 &_rhs) const;
  GLfloat & operator [](int _i);
  void set(GLfloat _x, GLfloat _y, GLfloat _z);
  void vertexGL() const;

private:
  union
  {
    GLfloat m_openGL[3];
    struct
    {
      GLfloat m_x;
      GLfloat m_y;
      GLfloat m_z;
    };
  };
};

#endif
