#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "include/World.h"

class Command
{
public:
  virtual void execute()=0;
  Command(World *_world):
      m_world(_world){}
protected:
  World *m_world;
};

class ClearWorld : public Command
{
public:
  void execute();
};

class MouseErase : public Command
{
public:
  void execute();
  void setxy(int x, int y);
private:
  int m_x, m_y;
};

class MouseDrag : public Command
{
public:

};

#endif // COMMAND

