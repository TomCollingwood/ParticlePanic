#ifndef TOOLBAR_H
#define TOOLBAR_H

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#include <include/World.h>


class Toolbar
{
public:
  Toolbar() :
    draw(true),
    drag(false),
    tap(false),
    clear(false),
    help(false),
    gravity(true),
    erase(false){}

  void drawTitle(float halfheight, float halfwidth) const;
  void drawToolbar(float halfheight, float halfwidth) const;
  void handleClickDown(World *world, int WIDTH, int x);
  void handleClickUp();
  void toggleBool(bool *toggleme);
  bool getDrag();
  bool getDraw();
  bool getHelp();

private:
  bool draw, erase, drag, tap, gravity, clear, help;
  GLuint iconsTexture, titleTexture;
  int clickdownbutton;

};

#endif // TOOLBAR_H
