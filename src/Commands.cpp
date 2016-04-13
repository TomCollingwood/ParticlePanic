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
