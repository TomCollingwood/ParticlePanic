///
///  @file Mat3.cpp
///  @brief Mat3 only used in Vec3's rotateAroundXAxisf
///


#include "include/Mat3.h"

Mat3::Mat3(GLfloat _s) :
  m_00(_s),
  m_01(0.0f),
  m_02(0.0f),
  m_10(0.0f),
  m_11(_s),
  m_12(0.0f),
  m_20(0.0f),
  m_21(0.0f),
  m_22(_s){}

Mat3::Mat3(float input[9])
{
  for(int i=0; i<9; ++i)
  {
    m_openGL[i]=input[i];
  }
}

GLfloat & Mat3::operator [](int _i)
{
  assert(_i>=0 && _i<=8);
  return m_openGL[_i];
}

