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
    m_draw(true),
    m_drag(false),
    m_tap(false),
    m_clear(false),
    m_help(false),
    m_gravity(true),
    m_erase(false),
    m_randomize(false),
    m_camera(false),
    m_dropdownopen(false),
    m_dropdownselect(0),
    m_helpscreen(false){}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief drawToolbar  draws the toolbar on screen
  /// \param _h           window height in pixels
  //----------------------------------------------------------------------------------------------------------------------
  void drawToolbar(int _h) const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief handleClickDown  handles click on the toolbar
  /// \param[in] _x           x coordinate of mouse on screen in pixels
  /// \param[in] _y           y coordinate of mouse on screen in pixels
  /// \param[in] _WIDTH       width of window in pixels
  /// \param[in] _HEIGHT      height of window in pixels
  /// \return                 if click is on screen returns true else false
  //----------------------------------------------------------------------------------------------------------------------
  bool handleClickDown(int _x, int _y, int _WIDTH, int _HEIGHT);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief handleClickUp  used when toolbar button only recesses while clicked down
  //----------------------------------------------------------------------------------------------------------------------
  void handleClickUp();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief handleClickDropDown  handles clicks on the dropdown menu
  /// \param[in] _x           x coordinate of mouse on screen in pixels
  /// \param[in] _y           y coordinate of mouse on screen in pixels
  /// \param[in] _WIDTH       width of window in pixels
  /// \param[in] _HEIGHT      height of window in pixels
  //----------------------------------------------------------------------------------------------------------------------
  void handleClickDropDown(int _x, int _y, int _WIDTH, int _HEIGHT);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief toggleBool         toggles the bool
  /// \param[inout] io_toggleme pointer to the bool to toggle
  //----------------------------------------------------------------------------------------------------------------------
  void toggleBool(bool *io_toggleme);

  bool getDrag();
  bool getDraw();
  bool getHelp();
  bool getErase();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief getdropdownopen  returns bool saying whether the dropdown menu is open
  /// \return                 bool saying whether the dropdown menu is open
  //----------------------------------------------------------------------------------------------------------------------
  bool getdropdownopen();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief setWorld   sets m_world to the pointer input
  /// \param[in] _world the pointer to set m_world to
  //----------------------------------------------------------------------------------------------------------------------
  void setWorld(World *_world);

  // Functions below are called to toggle bools when button is pressed
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

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief removeNumber removes number from the seed for the randomised particle
  //----------------------------------------------------------------------------------------------------------------------
  void removeNumber();

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief drawNumbers  draws the random seed numbers on screen
  /// \param[in] _x       x coordinate of mouse on screen in pixels
  /// \param[in] _y       y coordinate of mouse on screen in pixels
  /// \param[in] _h       height of window in pixels
  /// \param[in] _numbers numbers to draw
  //----------------------------------------------------------------------------------------------------------------------
  void drawNumbers(float _x, float _y, int _h, std::string _numbers) const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief handleKeys makes sure relevant keys affect the toolbar buttons
  /// \param[in] _input the character to handle
  //----------------------------------------------------------------------------------------------------------------------
  void handleKeys(char _input);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief addNumber  adds number to the random seed (in char form)
  /// \param[in] _p     character to add
  //----------------------------------------------------------------------------------------------------------------------
  void addNumber(char p);

  void drawHelpScreen() const;

private:
  bool m_draw, m_erase, m_drag, m_tap, m_gravity, m_clear, m_help, m_randomize, m_camera;
  bool m_dropdownopen;
  int m_dropdownselect;
  bool m_helpscreen;
  GLuint m_iconsTexture; //
  int m_clickdownbutton; //
  World *m_world;
  std::string m_randomSeed;

};

#endif // TOOLBAR_H
