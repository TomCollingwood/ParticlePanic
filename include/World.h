#ifndef WORLD_H
#define WORLD_H

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include <stdlib.h>
#include <vector>
#include "include/Particle.h"

/**
 * @brief The Scene class
 */
class World
{
public:
    /// A struct to hold vertices
    typedef struct {GLfloat x,y,z;} Vertex;

    /// A struct to hold triangles
    typedef struct {GLuint idx[3];} Triangle;

public:
    /// A constructor, called when this class is instanced in the form of an object
    World();

    /// A virtual destructor, in case we want to inherit from this class
    virtual ~World();

    /// Initialises the scene, called before render().
    virtual void init();

    /// Called when the window size changes (for example, if the user maximises the window).
    virtual void resize(int w, int h);

    /// Called frequently, used if you are using some form of animation
    virtual void update();

    /// Called most frequently, redraws the scene.
    virtual void draw();

protected: // Protected means that it is accessible to derived classes
    /// Keep track of whether this has been initialised - otherwise it won't be ready to draw!
    bool m_isInit;

    /// The time since the object was initialised, which is used for animation purposes
    double m_startTime;

    /// A member that is updated when update() is called indicating the elapsed time
    double m_elapsedTime;

    std::vector<Particle> particles;
    std::vector<Particle::Spring> springs;
    //std::vector<std::vector<Particle>> grid(10, std::vector<Particle>);

    float halfwidth;
    float halfheight;
    float interactionradius;
};

#endif // WORLD_H
