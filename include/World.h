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
#include <list>
#include <tuple>
#include "include/Particle.h"

/**
 * @brief The Scene class
 */
class World
{
public:
    /// A constructor, called when this class is instanced in the form of an object
    World();

    /// A virtual destructor, in case we want to inherit from this class
    virtual ~World();

    /// Initialises the scene, called before render().
    virtual void init();

    /// Called when the window size changes (for example, if the user maximises the window).
    virtual void resizeWorld(int w, int h);
    virtual void resizeWindow(int w, int h);

    /// Called frequently, used if you are using some form of animation
    virtual void update(bool *updateinprogress);

    /// Called most frequently, redraws the scene.
    virtual void draw();

    void hashParticles();

    Vec3 getGridColumnRow(int k);

    Vec3 getRenderGridxy(int h, int w);
    Vec3 getRenderGridColumnRow(int k);
    Vec3 getRenderGridxyfromIndex(int k);

    bool isLeftButtonPressed();

    void mouseDraw(int x, int y);
    void mouseDrag(int x, int y);
    void mouseErase(int x, int y);

    std::vector<Particle *> getSurroundingParticles(int thiscell,int numsur, bool withwalls) const;

    void mouseDragEnd(int x, int y);
    void selectDraggedParticles(int x, int y);

    void getbackhere(Particle * p);

    void handleKeys(char _input);

    void vectorvslist();

    void drawMenu();

    void deleteSpring(int s);
    int insertSpring(Particle::Spring spring);

    void deleteParticle(int p);
    void insertParticle(Particle particle);

    float getHalfHeight() const;
    float getHalfWidth() const;

    void toggleRain();
    void toggleGravity();
    void clearWorld();

    void set3D(bool b);

    void drawWith(int type);

    std::vector<std::vector<float>> renderGrid(ParticleProperties *p);
    void drawMarchingSquares(std::vector<std::vector<float>> renderGrid, ParticleProperties p, bool inner);

    void mouseMove(const int &x, const int &y, bool leftclick, bool rightclick);

    void defragParticles();
    void defragSprings();

    void drawchar();

    void setToDraw(int _todraw);
    void setRandomType(int _randomSeed);

protected: // Protected means that it is accessible to derived classes
    /// Keep track of whether this has been initialised - otherwise it won't be ready to draw!
    bool m_isInit;

    /// The time since the object was initialised, which is used for animation purposes
    double m_startTime;

    /// A member that is updated when update() is called indicating the elapsed time
    double m_elapsedTime;

    double m_timestep;

    // PARTICLES
    std::vector<Particle> particles; // std::list keeps it's pointers when reallocated
    int particlesPoolSize;
    int firstFreeParticle;
    int lastTakenParticle;
    int howManyAliveParticles;

    std::vector<std::vector<Particle *>> grid;
    std::vector<bool> cellsContainingParticles;

    //std::vector<std::vector<float>> renderGrid;

    // WORLD SIZE ATTRIBUTES
    float halfwidth, halfheight;
    float interactionradius;
    float squaresize;
    int gridheight, gridwidth, griddepth;
    int pixelwidth, pixelheight;

    // RENDERING ATTRIBUTES
    float pointsize;
    float mainrenderthreshold;
    int renderresolution;
    int renderwidth, renderheight;
    int renderoption;

    // SPRING ATTRIBUTES
    std::vector<Particle::Spring> springs;
    int firstFreeSpring;
    int lastTakenSpring;
    int springsize;

    std::vector<Particle *> draggedParticles;

    // Some input options
    bool rain;
    bool drawwall;
    bool gravity;

    int m_previousmousex, m_previousmousey;

    // FUN PARTICLE TYPES
    int m_todraw;

    int howmanytimesrandomized;
    bool m_3d;

    float m_camerarotatey, m_camerarotatex, m_camerazoom;

    std::vector<ParticleProperties> m_particleTypes;

    float m_boundaryMultiplier;

};

#endif // WORLD_H
