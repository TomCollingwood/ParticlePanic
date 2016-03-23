#include "include/Vec3.h"
#include <cmath>
#include <cassert>

float Vec3::dot(const Vec3 &_rhs) const
{
  return m_x*_rhs.m_x + m_y*_rhs.m_y;
}

float Vec3::length() const
{
  return sqrt(m_x*m_x + m_y*m_y);
}

float Vec3::lengthSquared() const
{
  return m_x*m_x + m_y*m_y;
}

void Vec3::normalize()
{
  float l=length();
  //assert(l != 0.0f);
  if(l!=0.0f)
  {
    m_x/=l;
    m_y/=l;
  }
}

Vec3 Vec3::perpendicular()
{
  return Vec3(-m_y, m_x);
}

//void Vec3::operator *(const Mat3 &_r) const
//{

//}

Vec3 Vec3::operator *(GLfloat _rhs) const
{
  return Vec3(m_x * _rhs,
              m_y * _rhs,
              m_z);
}

void Vec3::operator *=(GLfloat _rhs)
{
  m_x *= _rhs;
  m_y *= _rhs;
}

Vec3 Vec3::operator /(GLfloat _rhs) const
{
  return Vec3(m_x / _rhs,
              m_y / _rhs,
              m_z);
}

void Vec3::operator /=(GLfloat _rhs)
{
  m_x /= _rhs;
  m_y /= _rhs;
}

Vec3 Vec3::operator +(const Vec3 &_r) const
{
  return Vec3(m_x+_r.m_x,
              m_y+_r.m_y,
              m_z);
}

void Vec3::operator +=(const Vec3 &_r)
{
  m_x += _r.m_x;
  m_y += _r.m_y;
}

Vec3 Vec3::operator -(const Vec3 &_rhs) const
{
  return Vec3(m_x-_rhs.m_x,
              m_y-_rhs.m_y,
              m_z);
}

Vec3 Vec3::operator -()
{
  return Vec3(-m_x,-m_y);
}

void Vec3::operator -=(const Vec3 &_r)
{
  m_x -= _r.m_x;
  m_y -= _r.m_y;
}

bool Vec3::operator ==(const Vec3 &_rhs) const
{
  if(m_x==_rhs.m_x && m_y==_rhs.m_y) return true;
  else return false;
}

GLfloat & Vec3::operator [](int _i)
{
  assert(_i>=0 && _i<=2);
  return m_openGL[_i];
}

void Vec3::set(GLfloat _x, GLfloat _y, GLfloat _z)
{
  m_x=_x;
  m_y=_y;
  m_z=_z;
}

void Vec3::vertexGL() const
{
  glVertex3f(m_x,m_y,0.0f);
}

