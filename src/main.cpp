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
#include <SDL.h>
#include <SDL_image.h>
#else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_image.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtc/matrix_transform.hpp>


// Include the header file for our current World
#include "include/Vec3.h"
#include "include/Particle.h"
#include "include/World.h"
#include "include/Toolbar.h"

// Change this if you want something different.
#define WINDOW_TITLE "ParticlePanic"

// These defines are for the initial window size (it can be changed in the resize function)
int WIDTH = 640;
int HEIGHT = 400;

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

    }
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

    // Initialise the World
    world->init();
    // Need an initial resize to make sure the projection matrix is initialised
    world->resize(WIDTH, HEIGHT);

    // Use a timer to update our World. This is the best way to handle updates,
    // as the timer runs in a separate thread and is therefore not affected by the
    // rendering performance.
    SDL_TimerID timerID = SDL_AddTimer(1, /*elapsed time in milliseconds*/
                                     timerCallback, /*callback function*/
                                     (void*) NULL /*parameters (none)*/);

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    //Enable text input
    SDL_StartTextInput();

    bool pause=false;

    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            // The window has been resized
            if ((e.type == SDL_WINDOWEVENT) &&
                    (e.window.event == SDL_WINDOWEVENT_RESIZED)) {
                SDL_SetWindowSize(gWindow, e.window.data1, e.window.data2);
                world->resize(e.window.data1, e.window.data2);
                WIDTH=e.window.data1;
                HEIGHT=e.window.data2;
                //world->hashParticles();
            }
            //User requests quit
            else if( e.type == SDL_QUIT ) {
                quit = true;
            }
            //Handle keypress with current mouse position
            else if( e.type == SDL_TEXTINPUT ) { //on mouse press?
                world->handleKeys( e.text.text[ 0 ] );
            }
            else if( e.type == SDL_MOUSEBUTTONDOWN) { //on mouse press?
              //std::cout<<e.button<<std::endl;
              if (e.button.button == SDL_BUTTON_LEFT)
              {
                int x = 0, y = 0;
                SDL_GetMouseState( &x, &y );

                if(y<(float)(3.0f/20.0f)*HEIGHT)
                {
                  std::cout<<"yoooo"<<std::endl;
                  toolbar->handleClickDown(world, WIDTH, x);
                  leftMouseOnToolbar=true;
                }
                else
                  leftMouseOnWorld=true;
              }

              else if (e.button.button == SDL_BUTTON_RIGHT)
                rightMouseButton=true;
            }
            else if( e.type == SDL_MOUSEBUTTONUP) { //on mouse press?
              if (e.button.button == SDL_BUTTON_LEFT)
              {
                if(leftMouseOnWorld)
                {
                  leftMouseOnWorld=false;
                  leftMouseOnWorldPrevious=false;
                  if(toolbar->getDrag())
                  {
                    int x = 0, y = 0;
                    SDL_GetMouseState( &x, &y );
                    world->mouseDragEnd(x,y);
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
              world->selectDraggedParticles(x,y);
              leftMouseOnWorldPrevious=true;
            }
            world->mouseDrag( x, y);
          }
          else if(toolbar->getDraw())
          {
            world->mouseDraw( x, y );
          }
          else if(toolbar->getErase())
          {
            world->mouseErase(x, y);
          }
        }

        else if(rightMouseButton)
        {
          int x = 0, y = 0;
          SDL_GetMouseState(&x, &y);
          std::cout<<"heyboos"<<std::endl;
          world->mouseDraw( x, y );
        }

        if(!(leftMouseOnWorld&&toolbar->getDraw()))
        {
          world->update();
        }

        world->draw();
        //Render the World
        world->draw();
        toolbar->drawTitle(world->getHalfHeight(), world->getHalfWidth());
        toolbar->drawToolbar(world->getHalfHeight(), world->getHalfWidth());


        //Update screen
        SDL_GL_SwapWindow( gWindow );
    }

    //Disable text input
    SDL_StopTextInput();

    // Disable our timer
    SDL_RemoveTimer(timerID);

    // Delete our World
    delete world;
    //Destroy window
    SDL_DestroyWindow( gWindow );

    //Quit SDL subsystems
    SDL_Quit();

    return EXIT_SUCCESS;
}
