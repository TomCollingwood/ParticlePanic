/// \file Toolbar.h
/// \brief GUI implementation from scratch, includes draw and input interaction.
/// \author Thomas Collingwood
/// \version 1.0
/// \date 26/4/16 Updated to NCCA Coding standard
/// Revision History : See https://github.com/TomCollingwood/ParticlePanic

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#include <SDL.h>
#include <SDL_image.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif


#include <iostream>
#include <time.h>

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
    erase(false),
    randomize(false),
    camera(false),
    m_dropdownopen(false),
    m_dropdownselect(0){}

  void drawTitle(float halfheight, float halfwidth) const;
  void drawToolbar(int h) const;
  bool handleClickDown(int x, int y, int WIDTH, int HEIGHT);
  void handleClickUp();

  void handleClickDropDown(int x, int y, int WIDTH, int HEIGHT);

  void toggleBool(bool *toggleme);
  bool getDrag();
  bool getDraw();
  bool getHelp();
  bool getErase();

  bool getdropdownopen();

  void setWorld(World *_world);

  void pressDraw();
  void pressDrag();
  void pressErase();
  void pressGravity();
  void pressClear();
  void pressHelp();
  void pressTap();
  void pressDropDownMenu();
  void pressRandomize();
  void pressCamera();

  void removeNumber();

  void drawNumbers(float x, float y, int h, std::string _numbers) const;

  void handleKeys(char _input);

  void addNumber(char p);

private:
  bool draw, erase, drag, tap, gravity, clear, help, randomize, camera;
  bool m_dropdownopen;
  int m_dropdownselect;
  GLuint iconsTexture, titleTexture;
  int clickdownbutton;
  World *m_world;
  std::string m_randomSeed;

};

#endif // TOOLBAR_H
