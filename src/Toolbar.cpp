#include "include/Toolbar.h"

void Toolbar::drawTitle(float halfheight, float halfwidth) const
{
  glDisable(GL_LIGHTING);
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
  glEnable(GL_LIGHTING);
}

void Toolbar::drawToolbar(int h) const
{
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // You should probably use CSurface::OnLoad ... ;)
  //-- and make sure the Surface pointer is good!
  GLuint titleTextureID = 0;
  SDL_Surface* Surface = IMG_Load("textures/buttons.png");
  if(!Surface)
    {
      printf("IMG_Load: %s\n", IMG_GetError());
      std::cout<<"error"<<std::endl;
    }

  glGenTextures(1, &titleTextureID);
  glBindTexture(GL_TEXTURE_2D, titleTextureID);



  int Mode = GL_RGBA;

//  if(Surface->format->BytesPerPixel == 4) {
//      Mode = GL_RGBA;
//  }

  glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // ------------------------DRAW----------------------

  float halfheight = m_world->getHalfHeight();
  float halfwidth = m_world->getHalfWidth();

  float Width = ((halfheight*2)/h)*65;
  float Height = ((halfheight*2)/h)*50;

  float gap = 0.1f;

  float X = -halfwidth;
  float Y = halfheight-Height-gap;

  float texX;
  float texY = 0.0f;

  float texW = 74.0f/425.0f;
  float texH = 0.1f;

  // Button
  if(draw)
    texX=0.4f;
  else
    texX=0.0f;

    glBegin(GL_QUADS);
    glColor3f(1.0f,1.0f,1.0f);
    glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
    glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
    glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
    glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
    glEnd();

  // Icon

  texX=0.8f;
  texY=0.2f;
  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
  glEnd();

  texY=0.0f;


  // ---------------------ERASE --------------------------

  X+=Width+gap;

  // Button
  if(erase)
    texX=0.4f;
  else
    texX=0.0f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // Icon
  texX=0.2f;
  texY=0.3f;
  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
  glEnd();

  texY=0.0f;

  // --------------------DRAG--------------

  X+=Width+gap;

  // Button
  if(drag)
    texX=0.4f;
  else
    texX=0.0f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // Icon
  texX=0.0f;
  texY=0.3f;
  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
  glEnd();

  texY=0.0f;

  // -------------------TAP -------------
  X+=Width+gap;

  // Button
  if(tap)
    texX=0.4f;
  else
    texX=0.0f;


  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

//  Icon
  texX=0.4f;
  texY=0.2f;
  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
  glEnd();

  texY=0.0f;

  // -------------------GRAVITY -------------
  X+=Width+gap;
  if(gravity)
    texX=0.4;
  else
    texX=0.0f;

  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // Icon
  texX=0.4f;
  texY=0.3f;
  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
  glEnd();

  texY=0.0f;

  // -------------------CLEAR -------------
  X+=Width+gap;
  if(clear)
    texX=0.4f;
  else
    texX=0.0f;


  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // Icon
  texX=0.6f;
  texY=0.2f;
  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
  glEnd();

  texY=0.0f;

  // -------------------HELP -------------
  X+=Width+gap;
  if(help)
    texX=0.4f;
  else
    texX=0.0f;


  glBegin(GL_QUADS);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
  glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
  glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
  glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
  glEnd();

  // Icon
    texX=0.6f;
    texY=0.3f;
    glBegin(GL_QUADS);
    glColor3f(1.0f,1.0f,1.0f);
    glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
    glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
    glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
    glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
    glEnd();

    // ----------------------_RANDOMIZE-=-----------------------

    X+=Width*5+gap*6;

    if(randomize)
      texX=0.4f;
    else
      texX=0.0f;
    texY=0.0f;

    glBegin(GL_QUADS);
    glColor3f(1.0f,1.0f,1.0f);
    glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
    glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
    glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
    glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
    glEnd();

    texX=0.8f;
    texY=0.3f;
    glBegin(GL_QUADS);
    glColor3f(1.0f,1.0f,1.0f);
    glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
    glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
    glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
    glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
    glEnd();

    //std::cout<<X<<std::endl;

    X-=Width*5+gap*6;

    // ----------------------CAMERA SNAPSHOT-----------------------


    float prevX = X;
    float prevY = Y;
    X=halfwidth-1.3;
    Y=-halfheight+0.2f;

    if(randomize)
      texX=0.4f;
    else
      texX=0.0f;
    texY=0.0f;

    glBegin(GL_QUADS);
    glColor3f(1.0f,1.0f,1.0f);
    glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
    glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
    glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
    glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
    glEnd();

    texX=0.4f;
    texY=0.6f;
    glBegin(GL_QUADS);
    glColor3f(1.0f,1.0f,1.0f);
    glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -1);
    glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -1);
    glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -1);
    glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -1);
    glEnd();

    //std::cout<<X<<std::endl;

    X=prevX;
    Y=prevY;

    // */

    X=prevX;
    Y=prevY;



    // ------------------DROP DOWN MENU ---------------

    X+=Width+gap;

    Width=((halfheight*2)/h)*140;
    texW=0.4;
    if(!m_dropdownopen)
    {
      texY=0.4f;
      texX=0.6f;
      glBegin(GL_QUADS);
      glColor3f(1.0f,1.0f,1.0f);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();

      X+=gap;
      Y+=gap*1.5;

      glBegin(GL_QUADS); // WATER
      glTexCoord2f(0, 0.65+0.05*m_dropdownselect+texH*0.6); glVertex3f(X, Y, -1);
      glTexCoord2f(0+texW, 0.65+0.05*m_dropdownselect+texH*0.7); glVertex3f(X + Width, Y, -1);
      glTexCoord2f(0+texW, 0.65+0.05*m_dropdownselect); glVertex3f(X + Width, Y + Height*0.55, -1);
      glTexCoord2f(0, 0.65+0.05*m_dropdownselect); glVertex3f(X, Y + Height*0.55, -1);
      glEnd();

      X-=gap;
      Y-=gap*1.5;
    }
    else
    {
      texY=0.6f;
      texX=0.6f;
      texH=48.f/530.f;
      glBegin(GL_QUADS);
      glColor3f(1.0f,1.0f,1.0f);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();

      texH=42.f/530.f;
      texY=0.7f;
      Y-=Height*(m_dropdownselect+1);

      glBegin(GL_QUADS);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();

      Y+=Height*(m_dropdownselect+1);

      texH=42.f/530.f;
      texY=0.8f;
      Y-=Height;

      glBegin(GL_QUADS);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();



      Y-=Height;


      glBegin(GL_QUADS);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();

      Y-=Height;


      glBegin(GL_QUADS);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();

      Y-=Height;


      glBegin(GL_QUADS);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();

      Y-=Height;
      texY+=0.1f;


      glBegin(GL_QUADS);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();
      Y+=Height*5;

      Y-=Height*0.72;
      X+=0.1f;

      glBegin(GL_QUADS); // WATER
      glTexCoord2f(0, 0.65+texH*0.6); glVertex3f(X, Y, -1);
      glTexCoord2f(0+texW, 0.65+texH*0.7); glVertex3f(X + Width, Y, -1);
      glTexCoord2f(0+texW, 0.65); glVertex3f(X + Width, Y + Height*0.45, -1);
      glTexCoord2f(0, 0.65); glVertex3f(X, Y + Height*0.45, -1);
      glEnd();

      Y-=Height*1.03;

      glBegin(GL_QUADS); // SLIME
      glTexCoord2f(0, 0.7+texH*0.7); glVertex3f(X, Y, -1);
      glTexCoord2f(0+texW, 0.7+texH*0.7); glVertex3f(X + Width, Y, -1);
      glTexCoord2f(0+texW, 0.7); glVertex3f(X + Width, Y + Height*0.52, -1);
      glTexCoord2f(0, 0.7); glVertex3f(X, Y + Height*0.52, -1);
      glEnd();

      Y-=Height;

      glBegin(GL_QUADS); // BLOBBY
      glTexCoord2f(0, 0.75+texH*0.7); glVertex3f(X, Y, -1);
      glTexCoord2f(0+texW, 0.75+texH*0.7); glVertex3f(X + Width, Y, -1);
      glTexCoord2f(0+texW, 0.75); glVertex3f(X + Width, Y + Height*0.52, -1);
      glTexCoord2f(0, 0.75); glVertex3f(X, Y + Height*0.52, -1);
      glEnd();

      Y-=Height;

      glBegin(GL_QUADS); // BLOBBY
      glTexCoord2f(0, 0.8+texH*0.7); glVertex3f(X, Y, -1);
      glTexCoord2f(0+texW, 0.8+texH*0.7); glVertex3f(X + Width, Y, -1);
      glTexCoord2f(0+texW, 0.8); glVertex3f(X + Width, Y + Height*0.52, -1);
      glTexCoord2f(0, 0.8); glVertex3f(X, Y + Height*0.52, -1);
      glEnd();

      Y+=Height*3.75;

      X-=0.1f;
      texH=0.1f;
    }
    X+=Width+gap;
    if(m_randomSeed.size()!=0)
    {
      texY=0.4f;
      texX=0.0f;
      texW=223.f/425.f;
      glBegin(GL_QUADS);
      glColor3f(1.0f,1.0f,1.0f);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();
    }
    else
    {
      texY=0.5f;
      texX=0.0f;
      texW=223.f/425.f;
      glBegin(GL_QUADS);
      glColor3f(1.0f,1.0f,1.0f);
      glTexCoord2f(texX, texY+texH); glVertex3f(X, Y, -2);
      glTexCoord2f(texX+texW, texY+texH); glVertex3f(X + Width, Y, -2);
      glTexCoord2f(texX+texW, texY); glVertex3f(X + Width, Y + Height, -2);
      glTexCoord2f(texX, texY); glVertex3f(X, Y + Height, -2);
      glEnd();
    }

    this->drawNumbers(X+((halfheight*2)/h)*8,Y+((halfheight*2)/h)*13,h,m_randomSeed);
    //std::cout<<"seed"<<m_randomSeed<<std::endl;

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

bool Toolbar::handleClickDown(int x, int y, int WIDTH, int HEIGHT)
{

  float halfwidth = m_world->getHalfWidth();
  float halfheight = m_world->getHalfHeight();

  float Width = ((halfheight*2)/HEIGHT)*65;
  float Height = ((halfheight*2)/HEIGHT)*50;

  float gap = 0.1f;

  float startx = -halfwidth;
  float Y = halfheight-Height-gap;

  float worldx = ((float)x/(float)WIDTH)*(halfwidth*2) - halfwidth;
  float worldy = - ((float)y/(float)HEIGHT)*(halfheight*2) + halfheight;

  if(worldy>halfheight-(halfheight*(4.f/20.f)))
  {
    if(worldx>startx && worldx<startx+Width)  // draw
    {
      pressDraw();
      return true;
    }

    else if(worldx>startx+Width+gap && worldx<startx+Width*2+gap) // erase
    {
      pressErase();
      return true;
    }

    else if(worldx>startx+Width*2+gap*2 && worldx<startx+Width*3+gap*2) //drag
    {
      pressDrag();
      return true;
    }

    else if(worldx>startx+Width*3+gap*3 && worldx<startx+Width*4+gap*3) //tap
    {
      pressTap();
      return true;
    }

    else if(worldx>startx+Width*4+gap*4  && worldx<startx+Width*5+gap*4)//gravity
    {
      pressGravity();
      return true;
    }

    else if(worldx>startx+Width*5+gap*5 && worldx<startx+Width*6+gap*5) //clear
    {
      pressClear();
      return true;
    }

    else if(worldx>startx+Width*6+gap*6 && worldx<startx+Width*7+gap*6) //help
    {
      pressHelp();
      return true;
    }

    else if(worldx>startx+Width*7+gap*7 && worldx<startx+Width*6+gap*7+(((halfheight*2)/HEIGHT)*205))
    {
      pressDropDownMenu();
      return true;
    }

    else if(worldx>5.61667 && worldx<5.61667+Width) //help
    {
      pressRandomize();
      return true;
    }
  }


  else if(
          worldy<-halfheight+1.f &&
          worldx>halfwidth-1.3
          )
  {
    pressCamera();
    return true;
  }

  return false;
}

void Toolbar::handleClickUp()
{
  if (clickdownbutton==5) toggleBool(&clear);
  if (clickdownbutton==6) toggleBool(&help);
  if (clickdownbutton==8) toggleBool(&randomize);
  if (clickdownbutton==9)
  {
    toggleBool(&camera);
    m_world->handleKeys('t');
  }
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

void Toolbar::pressDraw()
{
  if(!draw) draw=true;
  if(drag) drag=false;
  if(erase) erase=false;
}

void Toolbar::pressErase()
{
  if(!erase) erase=true;
  clickdownbutton=1;
  if(draw) draw=false;
  if(drag) drag=false;
}

void Toolbar::pressDrag()
{
  toggleBool(&drag);
  clickdownbutton=2;
  if(draw) draw=false;
  if(erase) erase=false;
}

void Toolbar::pressTap()
{
  toggleBool(&tap);
  m_world->toggleRain();
  clickdownbutton=3;
}

void Toolbar::pressGravity()
{
  toggleBool(&gravity);
  m_world->toggleGravity();
  clickdownbutton=4;
}

void Toolbar::pressClear()
{
  toggleBool(&clear);
  clickdownbutton=5;
  m_world->clearWorld();
}

void Toolbar::pressHelp()
{
  toggleBool(&help);
  clickdownbutton=6;
}

void Toolbar::pressDropDownMenu()
{
  toggleBool(&m_dropdownopen);
  clickdownbutton=7;
}

void Toolbar::pressRandomize()
{
  toggleBool(&randomize);
  clickdownbutton=8;
  m_randomSeed.clear();
  for(int i=0; i<9; ++i)
  {
    char p = (char) ((rand() % 10) + 48);
    m_randomSeed.push_back(p);
  }
  int intRandomSeed = atoi(m_randomSeed.c_str());
  m_world->setRandomType(intRandomSeed);
}

void Toolbar::pressCamera()
{
  clickdownbutton=9;
  toggleBool(&camera);
}

void Toolbar::setWorld(World *_world)
{
  m_world=_world;
}

void Toolbar::handleKeys(char _input)
{
  switch(_input)
  {
  case 'i' :
    pressTap();
    break;
  case 'g' :
    pressGravity();
    break;
  case '0' :
    //soon
    break;
  case '1' :
    //soon
    break;
  case 'w' :
    //if(!m_3d)
    //{
      //soon
    //}
    break;
  case 'r':
    //if(!m_3d)
    //{
      //soon
    //}
    break;
  case 'p':
    //if(!m_3d)
    //{
      //soon
    //}
    break;

  case 'o' :
    //soon
    break;
  }
}

void Toolbar::drawNumbers(float x, float y, int h, std::string _numbers) const
{

  float halfheight = m_world->getHalfHeight();

  float gap = ((halfheight*2)/h)*12;
  float height = ((halfheight*2)/h)*23;

  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // You should probably use CSurface::OnLoad ... ;)
  //-- and make sure the Surface pointer is good!
  GLuint titleTextureID = 0;
  SDL_Surface* Surface = IMG_Load("textures/numbers.png");
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



  float currentx = x;

  for(char&c : _numbers)
  {
    switch(c)
    {
     case '1':
      glBegin(GL_QUADS);
      glColor3f(0.0f,0.0f,0.0f);
      glTexCoord2f(0, 1); glVertex3f(currentx, y, -1);
      glTexCoord2f(0.1,1); glVertex3f(currentx+gap, y, -1);
      glTexCoord2f(0.1, 0); glVertex3f(currentx+gap, y+height, -1);
      glTexCoord2f(0, 0); glVertex3f(currentx, y + height, -1);
      glEnd();
      break;
    case '2':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.1, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(0.2,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(0.2, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.1, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    case '3':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.2, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(0.3,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(0.3, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.2, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    case '4':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.3, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(0.4,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(0.4, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.3, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    case '5':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.4, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(0.5,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(0.5, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.4, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    case '6':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.5, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(0.6,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(0.6, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.5, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    case '7':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.6, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(0.7,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(0.7, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.6, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    case '8':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.7, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(0.8,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(0.8, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.7, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    case '9':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.8, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(0.9,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(0.9, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.8, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    case '0':
     glBegin(GL_QUADS);
     glColor3f(0.0f,0.0f,0.0f);
     glTexCoord2f(0.9, 1); glVertex3f(currentx, y, -1);
     glTexCoord2f(1,1); glVertex3f(currentx+gap, y, -1);
     glTexCoord2f(1, 0); glVertex3f(currentx+gap, y+height, -1);
     glTexCoord2f(0.9, 0); glVertex3f(currentx, y + height, -1);
     glEnd();
     break;
    }
    currentx+=gap;
  }

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

bool Toolbar::getdropdownopen()
{
  return m_dropdownopen;
}

void Toolbar::handleClickDropDown(int x, int y, int WIDTH, int HEIGHT)
{
  float halfwidth = m_world->getHalfWidth();
  float halfheight = m_world->getHalfHeight();

  float Width = ((halfheight*2)/HEIGHT)*65;
  float Height = ((halfheight*2)/HEIGHT)*50;

  float startx = -halfwidth;

  float gap = 0.1f;

  float worldx = ((float)x/(float)WIDTH)*(halfwidth*2) - halfwidth;
  float worldy = ((float)y/(float)HEIGHT)*(halfheight*2) - halfheight;
  if(worldx<startx+Width*7+gap*7 || worldx>startx+Width*6+gap*7+(((halfheight*2)/HEIGHT)*210)
     || worldy<-halfheight+Height+gap || worldy>-halfheight+5*Height+gap)
    pressDropDownMenu();
  else if(worldy>-halfheight+Height*4+gap)
  {
    m_dropdownselect=3;
    pressDropDownMenu();
  }
  else if(worldy>-halfheight+Height*3+gap)
  {
    m_dropdownselect=2;
    pressDropDownMenu();
  }
  else if(worldy>-halfheight+Height*2+gap)
  {
    m_dropdownselect=1;
    pressDropDownMenu();
  }
  else if(worldy>-halfheight+Height*1+gap)
  {
    m_dropdownselect=0;
    pressDropDownMenu();
  }

  m_world->setToDraw(m_dropdownselect);

}

void Toolbar::addNumber(char p)
{
  if(m_randomSeed.size()<9)
  {
    m_randomSeed.push_back(p);
  }
  if(m_randomSeed.size()==9)
  {
    int intRandomSeed = atoi(m_randomSeed.c_str());
    m_world->setRandomType(intRandomSeed);
  }
}

void Toolbar::removeNumber()
{
  if(m_randomSeed.size()>0)
  {
    m_randomSeed.erase( m_randomSeed.size()-1, 1);
  }
}
