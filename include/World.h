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
#include <deque>
#include <array>
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

    typedef struct spring{Particle *indexi, *indexj; GLfloat L;} Spring;

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

    void deleteParticle(int i);

    void insertParticle(Particle p);

    void hashParticles();

    std::vector<Particle *> getSurroundingParticles(int thiscell) const;

protected: // Protected means that it is accessible to derived classes
    /// Keep track of whether this has been initialised - otherwise it won't be ready to draw!
    bool m_isInit;

    /// The time since the object was initialised, which is used for animation purposes
    double m_startTime;

    /// A member that is updated when update() is called indicating the elapsed time
    double m_elapsedTime;

    std::deque<Particle> particles; //using double ended queue to avoid grid having dangling pointers
    std::deque<Spring> springs;
    std::vector<std::vector<Particle *>> grid; //we initialize this on resize
    std::vector<bool> cellsContainingParticles;

    float halfwidth, halfheight;
    float interactionradius;
    float squaresize;

    int gridheight, gridwidth;
};

#endif // WORLD_H
