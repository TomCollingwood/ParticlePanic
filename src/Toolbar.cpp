#include "include/Toolbar.h"

void Toolbar::drawTitle(float halfheight, float halfwidth) const
{
  glEnable(GL_TEXTURE_2D);
  // You should probably use CSurface::OnLoad ... ;)
  //-- and make sure the Surface pointer is good!
  GLuint titleTextureID = 0;
  SDL_Surface* Surface = IMG_Load("textures/title.png");
  if(!Surface)
    {
      printf("IMG_Load: %s\n", IMG_GetError());
      std::cout<<"error"<<std::endl;
    }

  glGenTextures(1, &titleTextureID);
  glBindTexture(GL_TEXTURE_2D, titleTextureID);

  int Mode = GL_RGB;

  if(Surface->format->BytesPerPixel == 4) {
      Mode = GL_RGBA;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  float X = -halfwidth;
  float Y = halfheight-1.5;
  float Width = 5;
  float Height = 1.5;
  glBegin(GL_QUADS);
      glColor3f(1.0f,1.0f,1.0f);
      glTexCoord2f(0, 1); glVertex3f(X, Y, -2);
      glTexCoord2f(1, 1); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(1, 0); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(0, 0); glVertex3f(X, Y + Height, -2);
  glEnd();

  glDisable(GL_TEXTURE_2D);
}

void Toolbar::drawToolbar(float halfheight, float halfwidth) const
{



  glEnable(GL_TEXTURE_2D);
  // You should probably use CSurface::OnLoad ... ;)
  //-- and make sure the Surface pointer is good!
  GLuint titleTextureID = 0;
  SDL_Surface* Surface = IMG_Load("textures/icons.png");
  if(!Surface)
    {
      printf("IMG_Load: %s\n", IMG_GetError());
      std::cout<<"error"<<std::endl;
    }

  glGenTextures(1, &titleTextureID);
  glBindTexture(GL_TEXTURE_2D, titleTextureID);

  int Mode = GL_RGB;

  if(Surface->format->BytesPerPixel == 4) {
      Mode = GL_RGBA;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // ------------------------DRAW----------------------
  float X = -halfwidth+5;
  float Y = halfheight-1.5;
  float Width = 1;
  float Height = 1.5;
  float texX;
  float texY = 0.0f;
  float texW = 0.05f;
  float texH = 1.0f;

  if(draw)
    texX=0.25;
  else
    texX=0;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // ---------------------ERASE --------------------------

  X = -halfwidth+5+1;
  texW=0.06;
  Width=1.2;
  if(erase)
    texX=0.66;
  else
    texX=0.596f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // --------------------DRAG--------------

  texW=0.05;
  X = X+1.2;
  Width=1;
  if(drag)
    texX=0.3;
  else
    texX=0.05f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // -------------------TAP -------------
  X++;
  if(tap)
    texX=0.35;
  else
    texX=0.1f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // -------------------GRAVITY -------------
  X++;
  if(gravity)
    texX=0.544;
  else
    texX=0.494f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // -------------------CLEAR -------------
  X++;
  if(clear)
    texX=0.15;
  else
    texX=0.4f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // -------------------HELP -------------
  X++;
  if(help)
    texX=0.2;
  else
    texX=0.45f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();


  glDisable(GL_TEXTURE_2D);
}

void Toolbar::handleClickDown(World *world, int WIDTH, int x)
{
  float halfwidth = world->getHalfWidth();
  float startx=-halfwidth+5;
  float worldx = ((float)x/(float)WIDTH)*(halfwidth*2) - halfwidth;

  if(worldx>startx && worldx<startx+1)  // draw
  {
    if(!draw) draw=true;
    clickdownbutton=0;
    if(drag) drag=false;
    if(erase) erase=false;
  }
  else if(worldx>startx+1 && worldx<startx+2.2) // erase
  {
    if(!erase) erase=true;
    clickdownbutton=1;
    if(draw) draw=false;
    if(drag) drag=false;
  }
  else if(worldx>startx+2.2 && worldx<startx+3.2) //drag
  {
    toggleBool(&drag);
    clickdownbutton=2;
    if(draw) draw=false;
    if(erase) erase=false;
  }
  else if(worldx>startx+3.2 && worldx<startx+4.2) //tap
  {
    toggleBool(&tap);
    world->toggleRain();
    clickdownbutton=3;
  }
  else if(worldx>startx+4.2 && worldx<startx+5.2)//gravity
  {
    toggleBool(&gravity);
    world->toggleGravity();
    clickdownbutton=4;
  }
  else if(worldx>startx+5.2 && worldx<startx+6.2) //clear
  {
    toggleBool(&clear);
    clickdownbutton=5;
    world->clearWorld();
  }
  else if(worldx>startx+5.2 && worldx<startx+6.2) //help
  {
    toggleBool(&help);
    clickdownbutton=5;
  }
}

void Toolbar::handleClickUp()
{
  if (clickdownbutton==5) toggleBool(&clear);
}

void Toolbar::toggleBool(bool *toggleme)
{
  if(*toggleme) (*toggleme)=false;
  else (*toggleme)=true;
}

bool Toolbar::getDrag()
{
  return drag;
}

bool Toolbar::getDraw()
{
  return draw;
}

bool Toolbar::getHelp()
{
  return help;
}

bool Toolbar::getErase()
{
  return erase;
}


