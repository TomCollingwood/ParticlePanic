///
///  @file ParticleProperties.cpp
///  @brief contains the properties of a particle type. Each particle has a pointer to such a class.

#include "include/ParticleProperties.h"

GLfloat ParticleProperties::getAlpha() const
{
  return m_alpha;
}

GLfloat ParticleProperties::getBeta() const
{
  return m_beta;
}

GLfloat ParticleProperties::getGamma() const
{
  return m_gamma;
}

GLfloat ParticleProperties::getSigma() const
{
  return m_sigma;
}

GLfloat ParticleProperties::getKnear() const
{
  return m_knear;
}

GLfloat ParticleProperties::getK() const
{
  return m_k;
}

GLfloat ParticleProperties::getKspring() const
{
  return m_kspring;
}

GLfloat ParticleProperties::getRed() const
{
  return m_red;
}

GLfloat ParticleProperties::getBlue() const
{
  return m_blue;
}

GLfloat ParticleProperties::getGreen() const
{
  return m_green;
}

GLfloat ParticleProperties::getP0() const
{
  return m_p0;
}

bool ParticleProperties::getSpring() const
{
  return m_spring;
}

bool ParticleProperties::getColourEffect() const
{
  return m_coloureffect;
}

void ParticleProperties::printVariables() const
{
  if(m_spring)
  {
  std::cout<<"----------SPRINGS------------"<<std::endl<<
              "Alpha: "<<m_alpha<<"  Gamma: "<<m_gamma<<"  Kspring: "<<m_kspring<<std::endl;
  }
  else
  {
    std::cout<<"--------NO SPRINGS----------"<<std::endl;
  }

  std::cout<<"----------DENSITY------------"<<std::endl<<
              "p0: "<<m_p0<<"  knear: "<<m_knear<<"  k:"<<m_k<<std::endl<<
              "----------VISCOSITY----------"<<std::endl<<
             "sigma: "<<m_sigma<<"  beta: "<<m_beta<<std::endl;

}

void ParticleProperties::randomize(int _seed)
{
  srand (_seed);
  //SPRING
  m_spring=false;
  if (rand() % 3==0) m_spring=true;  // 1/3 chance it uses spring algorithm
  m_alpha     = (rand() % 1000) * 0.0005f; // 0.0 - 0.5
  m_gamma     = (rand() % 1000) * 0.0003f; // 0.0 - 0.2
  m_kspring   = ((rand() % 1000) * 0.0008f) + 0.01f; // 0.01f - 0.81f
  //DENSITY
  m_p0        = ((rand() % 1000) * 0.019f) + 1;  // 1 - 20.0
  m_knear     = ((rand() % 1000) * 0.000099f) + 0.001f; // 0.1 - 0.0001
  m_k         = ((rand() % 1000) * 0.0000999f) + 0.0001f; // 0.1 - 0.00001
  //VISCOSITY
  m_sigma     = (rand() % 1000) * 0.0005; // 0-0.5
  m_beta      = (rand() % 1000) * 0.0008; // 0-0.8
  //COLOURS
  m_red       = (rand() % 1000) * 0.001; // 0-1.0f
  m_blue       = (rand() % 1000) * 0.001; // 0-1.0f
  m_green       = (rand() % 1000) * 0.001; // 0-1.0f
  m_coloureffect=false;
  if(rand()%2==0) m_coloureffect=true;
  printVariables();
}
