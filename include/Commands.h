/// \file Commands.h
/// \brief Command  objects to be executed inside timerCallback in main. The purpose of this is to
///                 make sure the commands are not executed in the middle of update() when outside the timer
/// \author Thomas Collingwood
/// \version 1.0
/// \date 26/4/16 Updated to NCCA Coding standard
/// Revision History : See https://github.com/TomCollingwood/ParticlePanic

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "include/World.h"

class Command
{
public:
  ~Command() {}
  virtual void execute() = 0;
  void setWorld(World *_world) { m_world=_world; }
protected:
  World *m_world;
};

class ClearWorld : public Command
{
public:
  ClearWorld(){}
  ~ClearWorld(){}
  void execute() {m_world->clearWorld();}
};

class MouseErase : public Command
{
public:
  MouseErase(){}
  ~MouseErase(){}
  void execute() {m_world->mouseErase(m_x,m_y);}
  void setxy(int _x, int _y)
  {
    m_x=_x;
    m_y=_y;
  }
private:
  int m_x, m_y;
};

class MouseDraw : public Command
{
public:
  MouseDraw(){}
  ~MouseDraw(){}
  void execute() {m_world->mouseDraw(m_x,m_y);}
  void setxy(int _x, int _y)
  {
    m_x=_x;
    m_y=_y;
  }
private:
  int m_x, m_y;
};

class MouseDrag : public Command
{
public:
  MouseDrag(){}
  ~MouseDrag(){}
  void execute() {m_world->mouseDrag(m_x,m_y);}
  void setxy(int _x, int _y)
  {
    m_x=_x;
    m_y=_y;
  }
private:
  int m_x, m_y;
};

class SelectDraggedParticles : public Command
{
public:
  SelectDraggedParticles(){}
  ~SelectDraggedParticles(){}
  void execute() {m_world->selectDraggedParticles(m_x,m_y);}
  void setxy(int _x, int _y)
  {
    m_x=_x;
    m_y=_y;
  }
private:
  int m_x, m_y;
};

class MouseDragEnd : public Command
{
public:
  MouseDragEnd(){}
  ~MouseDragEnd(){}
  void execute() {m_world->mouseDragEnd(m_x,m_y);}
  void setxy(int _x, int _y)
  {
    m_x=_x;
    m_y=_y;
  }
private:
  int m_x, m_y;
};

class ResizeWorld : public Command
{
public:
  ResizeWorld(){}
  ~ResizeWorld(){}
  void execute() {m_world->resizeWorld(m_w,m_h);}
  void setwh(int _w, int _h)
  {
    m_w=_w;
    m_h=_h;
  }
private:
  int m_w, m_h;
};

class Set3D : public Command
{
public:
  Set3D(){}
  ~Set3D(){}
  void execute() {m_world->set3D(m_3d);}
  void setBool(bool b)
  {
    m_3d=b;
  }
private:
  bool m_3d;
};

#endif // COMMAND

