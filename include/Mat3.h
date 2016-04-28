/// \file Mat3.h
/// \brief Object needed to rotate particles in 3D mode
/// \author Thomas Collingwood
/// \version 1.0
/// \date 26/4/16 Updated to NCCA Coding standard
/// Revision History : See https://github.com/TomCollingwood/ParticlePanic

#ifndef _MAT3_H_
#define _MAT3_H_

#ifdef __linux__
  #include <GL/gl.h>
#endif
#ifdef __APPLE__
  #include <OpenGL/gl.h>
#endif

#include <cassert>

class Mat3{
  public:
    Mat3();
    Mat3(GLfloat _s=1.0f);
    Mat3(float input[9]);
    Mat3(const Mat3 &_r)=default;
    GLfloat & operator [](int _i);
  union
  {
    float m_m[3][3];
    float m_openGL[9];
    struct{
      GLfloat m_00;
      GLfloat m_01;
      GLfloat m_02;
      GLfloat m_10;
      GLfloat m_11;
      GLfloat m_12;
      GLfloat m_20;
      GLfloat m_21;
      GLfloat m_22;
    };
  };
};


#endif
