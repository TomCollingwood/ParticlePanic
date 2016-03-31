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
  float texW = 0.1f;
  float texH = 1.0f;

  if(draw)
    texX=0.5;
  else
    texX=0;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // ---------------------DRAG --------------------------

  X = -halfwidth+5+1;
  if(drag)
    texX=0.6;
  else
    texX=0.1f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // --------------------TAP--------------

  X = -halfwidth+5+2;
  if(tap)
    texX=0.7;
  else
    texX=0.2f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // -------------------CLEAR -------------
  X = -halfwidth+5+3;
  if(clear)
    texX=0.8;
  else
    texX=0.3f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // -------------------HELP -------------
  X = -halfwidth+5+4;
  if(clear)
    texX=0.9;
  else
    texX=0.4f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();


  glDisable(GL_TEXTURE_2D);

}
