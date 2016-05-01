/// \file ParticleProperties.h
/// \brief contains the properties of a particle type. Each particle has a pointer to such a class.
/// \author Thomas Collingwood
/// \version 1.0
/// \date 26/4/16 Updated to NCCA Coding standard
/// Revision History : See https://github.com/TomCollingwood/ParticlePanic

#ifndef _PARTICLEPROPERTIES_H_
#define _PARTICLEPROPERTIES_H_

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <iostream>

class ParticleProperties
{
public:
  /*
  ParticleProperties(bool spring=true,
                     GLfloat _sigma=0.0f,
                     GLfloat _beta=0.1f,
                     GLfloat _gamma=0.4f,
                     GLfloat _alpha=3.0f,
                     GLfloat _knear=0.01f,
                     GLfloat _k=0.004f,
                     GLfloat _kspring=0.05f,
                     GLfloat _p0=10.0f,
                     GLfloat _red=0,
                     GLfloat _green=0,
                     GLfloat _blue=1.0f,
                     bool _coloureffect=true):


    //water */

    ParticleProperties(bool spring=false,
                       GLfloat _sigma=0.05f,
                       GLfloat _beta=0.1f,
                       GLfloat _gamma=0.004f,
                       GLfloat _alpha=0.3f,
                       GLfloat _knear=0.01f,
                       GLfloat _k=0.004f,
                       GLfloat _kspring=0.3f,
                       GLfloat _p0=5.0f,
                       GLfloat _red=0,
                       GLfloat _green=0,
                       GLfloat _blue=1.0f,
                       bool _coloureffect=true):
                        //  */
    m_spring(spring),
    m_sigma(_sigma),
    m_beta(_beta),
    m_gamma(_gamma),
    m_alpha(_alpha),
    m_knear(_knear),
    m_k(_k),
    m_kspring(_kspring),
    m_p0(_p0),
    m_red(_red),
    m_green(_green),
    m_blue(_blue),
    m_coloureffect(_coloureffect){}
  GLfloat getSigma() const;
  GLfloat getBeta() const;
  GLfloat getGamma() const;
  GLfloat getAlpha() const;
  GLfloat getKnear() const;
  GLfloat getK() const;
  GLfloat getKspring() const;
  GLfloat getRed() const;
  GLfloat getGreen() const;
  GLfloat getBlue() const;
  GLfloat getP0() const;
  bool getSpring() const;
  bool getColourEffect() const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief printVariables prints the attributes. Used when randomizing ParticleProperties.
  //----------------------------------------------------------------------------------------------------------------------
  void printVariables() const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief randomize  randomizes the attributes of the object
  /// \param[in] _seed  the srand() input, same seed same, ParticleProperties attributes
  //----------------------------------------------------------------------------------------------------------------------
  void randomize(int _seed);

private:
  GLfloat m_sigma, m_beta, m_gamma, m_alpha, m_knear, m_k, m_kspring, m_p0, m_red, m_green, m_blue, m_spring, m_coloureffect;

};

#endif // PARTICLEPROPERTIES_H
