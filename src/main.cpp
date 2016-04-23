// Some crap we need to compile on Windows (not tested)
#ifdef _WIN32
#include <windows.h>
#endif

// Includes needed for SDL and GL
//


#include <iostream>

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include "GLUT/glut.h"
#include <SDL.h>
#include <SDL_image.h>
#else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_image.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
  //#include <GL/glut.h>
#endif


// Include the header file for our current World
#include "include/Vec3.h"
#include "include/Particle.h"
#include "include/World.h"
#include "include/Toolbar.h"
#include "include/Commands.h"



// Change this if you want something different.
#define WINDOW_TITLE "ParticlePanic"

// These defines are for the initial window size (it can be changed in the resize function)
int WIDTH = 900;
int HEIGHT = 600;

// Our World, which will store all the GL stuff
World *world = NULL;
Toolbar *toolbar = NULL;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

bool leftMouseOnWorld = false;
bool leftMouseOnToolbar = false;
bool leftMouseOnWorldPrevious=false;
bool rightMouseButton = false;
bool pookd = false;
bool updateinprogress = false;
bool drawInTimer=false;

int frame=0;
int previousframe=0;
int framedrawn=0;

std::vector<Command*> commands;

/**
 * @brief initSDL fires up the SDL window and readies it for OpenGL
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int initSDL()
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }
    else
    {
        //Use OpenGL 3.1 core
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        //Create window
        gWindow = SDL_CreateWindow( WINDOW_TITLE,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    WIDTH,
                                    HEIGHT,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * @brief timerCallback an SDL2 callback function which will trigger whenever the timer has hit the elapsed time.
 * @param interval The elapsed time (not used - World uses it's own internal clock)
 * @return the elapsed time.
 */
Uint32 timerCallback(Uint32 interval, void *) {
    if (world != NULL)
    {
      for(auto& i : commands)
      {
        i->execute();
      }
      for(auto& i : commands)
      {
        delete i;
      }
      commands.clear();

      if(world->getSnapshotMode()<2)
        world->update(&updateinprogress);

      if(drawInTimer)
      {
        SDL_GL_MakeCurrent(gWindow,gContext);
        world->draw();
        toolbar->drawToolbar(HEIGHT);
        SDL_GL_SwapWindow( gWindow );
      }

    }
    ++frame;
    return interval;
}

/**
 * @brief main The main opengl loop is managed here
 * @param argc Not used
 * @param args Not used
 * @return EXIT_SUCCESS if it went well!
 */
int main( int argc, char* args[] ) {
    //Start up SDL and create window
    if( initSDL() == EXIT_FAILURE ) return EXIT_FAILURE;

    //Create context
    gContext = SDL_GL_CreateContext( gWindow );
    if( gContext == NULL ) return EXIT_FAILURE;

    //Use Vsync
    if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
        fprintf(stderr, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    }

    // We should now be ready to use OpenGL
    // This object holds our World. It needs to be initialised before it can be drawn.
    world = new World();

    toolbar = new Toolbar();
    toolbar->setWorld(world);

    // Initialise the World
    world->init();

    // Need an initial resize to make sure the projection matrix is initialised
    world->resizeWindow(WIDTH, HEIGHT);
    world->resizeWorld(WIDTH, HEIGHT);

    // Use a timer to update our World. This is the best way to handle updates,
    // as the timer runs in a separate thread and is therefore not affected by the
    // rendering performance.

    SDL_TimerID timerID = SDL_AddTimer(30, /*elapsed time in milliseconds*/
                                     timerCallback, /*callback function*/
                                     (void*) NULL /*parameters (none)*/);

    //glutTimerFunc(30, timerCallback, (void *) NULL);


    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    //Enable text input
    SDL_StartTextInput();

    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            // The window has been resized
            if ((e.type == SDL_WINDOWEVENT) &&
                    (e.window.event == SDL_WINDOWEVENT_RESIZED))
            {
                SDL_SetWindowSize(gWindow, e.window.data1, e.window.data2);

                ResizeWorld *newcommand=new ResizeWorld();
                newcommand->setwh(e.window.data1, e.window.data2);
                newcommand->setWorld(world);
                commands.push_back(newcommand);

                world->resizeWindow(e.window.data1, e.window.data2);
                WIDTH=e.window.data1;
                HEIGHT=e.window.data2;
            }
            //User requests quit
            else if( e.type == SDL_QUIT )
            {
              quit = true;
            }

            else if(e.type == SDL_KEYDOWN)
            {
              if(e.key.keysym.sym == SDLK_BACKSPACE)
              {
                toolbar->removeNumber();
              }
            }

            //Handle keypress with current mouse position
            else if( e.type == SDL_TEXTINPUT )
            {
              int charint = (int) e.text.text[0];
              if(charint > 47 && charint < 58)
              {
                toolbar->addNumber(e.text.text[0]);
              }
              if(e.text.text[0]=='p' || e.text.text[0]=='o')
              {
                if(!world->getSnapshotMode())
                {
                  bool toSet3D = false;
                  if(e.text.text[0]=='p') toSet3D=true;

                  ClearWorld *newcommand2=new ClearWorld();
                  newcommand2->setWorld(world);
                  commands.push_back(newcommand2);

                  Set3D *newcommand=new Set3D();
                  newcommand->setBool(toSet3D);
                  newcommand->setWorld(world);
                  commands.push_back(newcommand);

                  ResizeWorld *newcommand3=new ResizeWorld();
                  newcommand3->setwh(WIDTH,HEIGHT);
                  newcommand3->setWorld(world);
                  commands.push_back(newcommand3);
                }
              }
              world->handleKeys( e.text.text[ 0 ] );
              if(!world->getSnapshotMode())
                toolbar->handleKeys( e.text.text[ 0 ] );
            }

            else if( e.type == SDL_MOUSEBUTTONDOWN)
            {
              if (e.button.button == SDL_BUTTON_LEFT)
              {
                  int x = 0, y = 0;
                  SDL_GetMouseState( &x, &y );

                  if(toolbar->getdropdownopen() && !world->getSnapshotMode())
                  {
                    toolbar->handleClickDropDown(x, y, WIDTH, HEIGHT);
                  }

                  else if(!world->getSnapshotMode())
                  {
                    leftMouseOnToolbar = toolbar->handleClickDown(x, y, WIDTH, HEIGHT);
                  }
                  else
                    leftMouseOnWorld=true;
                }

              else if (e.button.button == SDL_BUTTON_RIGHT)
              {
                if(!world->getSnapshotMode())
                {
                  rightMouseButton=true;
                }
              }
            }

            else if( e.type == SDL_MOUSEBUTTONUP)
            {
              if (e.button.button == SDL_BUTTON_LEFT)
              {
                if(leftMouseOnWorld)
                {
                  leftMouseOnWorld=false;
                  leftMouseOnWorldPrevious=false;

                  if(toolbar->getDrag() && !world->getSnapshotMode())
                  {
                    int x = 0, y = 0;
                    SDL_GetMouseState( &x, &y );

                    MouseDragEnd *newcommand=new MouseDragEnd();
                    newcommand->setxy(x,y);
                    newcommand->setWorld(world);
                    commands.push_back(newcommand);
                  }
                }
                else if(leftMouseOnToolbar)
                {
                  toolbar->handleClickUp();
                  leftMouseOnToolbar=false;
                }
              }
              else if (e.button.button == SDL_BUTTON_RIGHT)
                rightMouseButton=false;
            }

            else if (e.type == SDL_MOUSEMOTION) {
              int x = 0, y = 0;
              SDL_GetMouseState( &x, &y );
              world->mouseMove(x, y, leftMouseOnWorld, rightMouseButton);
            }

        }

        if(leftMouseOnWorld)
        {
          int x = 0, y = 0;
          SDL_GetMouseState(&x, &y);
          if(toolbar->getDrag())
          {
            if(!leftMouseOnWorldPrevious)
            {
              SelectDraggedParticles *newcommand=new SelectDraggedParticles();
              newcommand->setWorld(world);
              newcommand->setxy(x,y);
              commands.push_back(newcommand);

              leftMouseOnWorldPrevious=true;
            }

            MouseDrag *newcommand=new MouseDrag();
            newcommand->setWorld(world);
            newcommand->setxy(x,y);
            commands.push_back(newcommand);
          }
          else if(toolbar->getDraw())
          {
            MouseDraw *newcommand=new MouseDraw();
            newcommand->setWorld(world);
            newcommand->setxy(x,y);
            commands.push_back(newcommand);
          }
          else if(toolbar->getErase())
          {
            MouseErase *newcommand=new MouseErase();
            newcommand->setWorld(world);
            newcommand->setxy(x,y);
            commands.push_back(newcommand);
          }
        }

        else if(rightMouseButton)
        {
          int x = 0, y = 0;
          SDL_GetMouseState(&x, &y);
          std::cout<<"heyboos"<<std::endl;

          MouseDraw *newcommand=new MouseDraw();
          newcommand->setWorld(world);
          newcommand->setxy(x,y);
          commands.push_back(newcommand);
        }

        if(!drawInTimer)
        {
          world->draw();

          if(!world->getSnapshotMode())
          {
            toolbar->drawToolbar(HEIGHT);
          }

          SDL_GL_SwapWindow( gWindow );
        }
    }

    //Disable text input
    SDL_StopTextInput();

    // Disable our timer
    SDL_RemoveTimer(timerID);

    world->clearWorld();

    // Delete our World
    delete world;
    //Destroy window
    SDL_DestroyWindow( gWindow );

    //Quit SDL subsystems
    SDL_Quit();

    return EXIT_SUCCESS;
}
