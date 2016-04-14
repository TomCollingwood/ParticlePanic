#include "include/Commands.h"

void ClearWorld::execute()
{
  m_world->clearWorld();
}

void MouseErase::setxy(int _x, int _y)
{
  m_x=_x;
  m_y=_y;
}

void MouseErase::execute()
{
  m_world->mouseErase(m_x,m_y);
}

void MouseDrag::setxy(int _x, int _y)
{
  m_x=_x;
  m_y=_y;
}

void MouseDrag::execute()
{
  m_world->mouseDrag(m_x,m_y);
}

void SelectDraggedParticles::setxy(int _x, int _y)
{
  m_x=_x;
  m_y=_y;
}

void SelectDraggedParticles::execute()
{
  m_world->selectDraggedParticles(m_x,m_y);
}

void MouseDragEnd::setxy(int _x, int _y)
{
  m_x=_x;
  m_y=_y;
}

void MouseDragEnd::execute()
{
  m_world->mouseDragEnd(m_x,m_y);
}
