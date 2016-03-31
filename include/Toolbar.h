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

class Toolbar
{
public:
  Toolbar() :
    draw(true),
    drag(false),
    tap(false),
    clear(false),
    help(false){}

  void drawTitle(float halfheight, float halfwidth) const;
  void drawToolbar(float halfheight, float halfwidth) const;

private:
  bool draw, drag, tap, clear, help;
  GLuint iconsTexture, titleTexture;

};

#endif // TOOLBAR_H
