/// \file World.h
/// \brief contains all particles and methods to draw and update them
/// \author Thomas Collingwood
/// \version 1.0
/// \date 26/4/16 Updated to NCCA Coding standard
/// Revision History : See https://github.com/TomCollingwood/ParticlePanic

#ifndef _WORLD_H_
#define _WORLD_H_

#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

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

#include "include/Vec3.h"
#include "include/Particle.h"
#include "include/ParticleProperties.h"
#include "include/MarchingAlgorithms.h"


/**
 * @brief The Scene class
 */
class World
{
public:
    /// A constructor, called when this class is instanced in the form of an object
    World();

    /// A virtual destructor, in case we want to inherit from this class
    ~World();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief init Initialises the scene, called before render().
    //----------------------------------------------------------------------------------------------------------------------
    void init();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief resizeWorld  resizes the spatial hash and world objects when window size changes
    /// \param[in] _w       new window width in pixels
    /// \param[in] _h       new window hieght in pixels
    //----------------------------------------------------------------------------------------------------------------------
    void resizeWorld(int _w, int _h);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief resizeWindow resizes the SDL window & OpenGL
    /// \param[in] _w       new window width in pixels
    /// \param[in] _h       new window hieght in pixels
    //----------------------------------------------------------------------------------------------------------------------
    void resizeWindow(int _w, int _h);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief update                   updates the particles in the world according to SPH algorithms. Called in timer.
    /// \param[out] o_updateinprogress  bool that is set when update is in progress
    //----------------------------------------------------------------------------------------------------------------------
    void update(bool *o_updateinprogress);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief draw Draws the particles in the world either in spheres, marching cubes or squares
    //----------------------------------------------------------------------------------------------------------------------
    void draw();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief getHalfHeight  returns half the height of the window in OpenGL coordinates
    /// \return               value of half the height of the window
    //----------------------------------------------------------------------------------------------------------------------
    float getHalfHeight() const;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief getHalfWidth returns half the width of the window in OpenGL coordinates
    /// \return             value of half the height of the window
    //----------------------------------------------------------------------------------------------------------------------
    float getHalfWidth() const;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief getGridColumnRow returns the grid's column and row number from the spatial hash number
    /// \param[in] _k           spatial hash grid index
    /// \return                 Vec3 with column as m_x, and row as m_y
    //----------------------------------------------------------------------------------------------------------------------
    Vec3 getGridColumnRow(int _k);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief getSurroundingParticles  gets all particles in surrounding grids.
    /// \param[in] _thiscell            the centre cell in which to search for surrounding particles from
    /// \param[in] _numsur              how far out neightbouring particles can be (counted in grid squares)
    ///                                 if numsur==1 then it would return all particles in 3x3 grid around thiscell
    /// \param[in] _withwalls           if true will also include particles that are of wall type
    /// \return                         returns vector of pointers to the surrounding particles
    //----------------------------------------------------------------------------------------------------------------------
    std::vector<Particle *> getSurroundingParticles(int thiscell,int numsur, bool withwalls) const;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief getbackhere  if particle is out of boundaries then it's position is changes so it is within boundaries
    /// \param[inout] io_p  pointer to particle to check
    //----------------------------------------------------------------------------------------------------------------------
    void getbackhere(Particle * io_p);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief handleKeys handles keyboard key inputs that affect the particles within the world
    /// \param[in] _input the inputted key
    //----------------------------------------------------------------------------------------------------------------------
    void handleKeys(char _input);

    // SPRING AND PARTICLE MAINTENANCE

    // Springs and Particles are stored in a pre-allocated vector of a certain size. When inserting we insert where
    // we know is the first free particle. We store this attribute in world. As m_firstFreeParticle and
    // m_firstFreeSpring. Once we inserted we find the next free particle and update this attribute. When deleting
    // particles / springs we also update the first free particle or spring attribute if we delete one that is before
    // the current first free particle / spring.

    // We need to defrag the list so that all the alive particles / springs are together and all the dead ones are
    // together also. As they can become fragmented when we delete particles / springs.

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief deleteSpring deletes a spring from the vector m_springs while updating m_lastFreeSpring and m_lastTakenSpring
    /// \param[in] _s       index of spring to delete
    //----------------------------------------------------------------------------------------------------------------------
    void deleteSpring(int _s);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief insertSpring Inserts a spring to the vector m_springs while updating m_lastFreeSpring and m_lastTakenSpring
    /// \param[in] _spring  Spring to insert
    /// \return             Index of the inserted spring in m_springs
    //----------------------------------------------------------------------------------------------------------------------
    int insertSpring(Particle::Spring _spring);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief deleteParticle deletes a particle from the vector m_particles while updating m_lastFreeParticle and
    ///                       m_lastTakenParticle
    /// \param[in] _p         Index of particle to delete
    //----------------------------------------------------------------------------------------------------------------------
    void deleteParticle(int _p);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief insertParticle inserts a particle to the vector m_particles while updating m_lastFreeParticle and
    ///                       m_lastTakenParticle
    /// \param[in] _particle  particle to insert
    //----------------------------------------------------------------------------------------------------------------------
    void insertParticle(Particle _particle);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief defragParticles  reorders the particles within m_particles so all alive particles are to the left
    //----------------------------------------------------------------------------------------------------------------------
    void defragParticles();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief defragSprings  reorders the springs within m_springs so that all alive springs are to the left
    //----------------------------------------------------------------------------------------------------------------------
    void defragSprings();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief hashParticles  takes m_grid and m_particles, using spatial hash organises the particles into
    ///                       buckets / squares as pointers
    //----------------------------------------------------------------------------------------------------------------------
    void hashParticles();

    // INPUTS
    //----------------------------------------------------------------------------------------------------------------------
    /// \brief mouseDraw  creates particles at mouse location
    /// \param[in] _x     x coordinate of mouse in pixels
    /// \param[in] _y     y coordinate of mouse in pixels
    //----------------------------------------------------------------------------------------------------------------------
    void mouseDraw(int _x, int _y);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief mouseDrag  drags the m_draggedparticles to mouse location
    /// \param[in] _x     x coordinate of mouse in pixels
    /// \param[in] _y     y coordinate of mouse in pixels
    //----------------------------------------------------------------------------------------------------------------------
    void mouseDrag(int _x, int _y);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief mouseErase erases particles in grid square at mouse location
    /// \param[in] _x     x coordinate of mouse in pixels
    /// \param[in] _y     y coordinate of mouse in pixels
    //----------------------------------------------------------------------------------------------------------------------
    void mouseErase(int _x, int _y);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief mouseDragEnd called at end of a drag to calculate which direction to fling particles
    /// \param[in] _x       x coordinate of mouse in pixels
    /// \param[in] _y       y coordinate of mouse in pixels
    //----------------------------------------------------------------------------------------------------------------------
    void mouseDragEnd(int _x, int _y);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief selectDraggedParticles selects the particles to drag - called once at beginning of drag
    /// \param[in] _x                 x coordinate of mouse in pixels
    /// \param[in] _y                 y coordinate of mouse in pixels
    //----------------------------------------------------------------------------------------------------------------------
    void selectDraggedParticles(int _x, int _y);

    void toggleRain();
    void toggleGravity();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief clearWorld deletes all particles in the world
    //----------------------------------------------------------------------------------------------------------------------
    void clearWorld();

    void set3D(bool b);
    bool get3D();

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief drawWith   sets m_todraw to the input. m_todraw determines which particle type to draw, is index of m_particleTypes
    /// \param[in] _type  value to set m_todraw to
    //----------------------------------------------------------------------------------------------------------------------
    void drawWith(int _type);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief mouseMove  used to rotate world in 3D according to mouse movement
    /// \param x          x position of mouse on screen in pixels
    /// \param y          y position of mouse on screen in pixels
    /// \param leftclick  true if left mouse button is pressed
    /// \param rightclick true if right mouse button is pressed
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMove(const int &x, const int &y, bool leftclick);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief setToDraw  used by Toolbar.h 's dropdown menu to set which particle type to draw with
    /// \param _todraw    int to set m_todraw to
    //----------------------------------------------------------------------------------------------------------------------
    void setToDraw(int _todraw);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief setRandomType  randomizes the random type particle (m_particleTypes[3])
    /// \param _randomSeed    seed to set srand() to
    //----------------------------------------------------------------------------------------------------------------------
    void setRandomType(int _randomSeed);

    void makeParticlesBig();
    void makeParticlesSmall();

    // RENDER GRIDS
    //----------------------------------------------------------------------------------------------------------------------
    /// \brief renderGrid creates a 2D vector containing floats that are calculated with a metaball function.
    ///                   The grid is specific to the particle type. The grid is used to create marching squares.
    /// \param p          ParticleProperties to create the grid for
    /// \return           the render grid
    //----------------------------------------------------------------------------------------------------------------------
    std::vector<std::vector<float>> renderGrid(ParticleProperties *p);

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief render3dGrid creates a 3D vector containing floats that are calculated with a metaball function.
    ///                     The grid is specific to the particle type. The grid is used to create marching cubes.
    /// \param p            ParticleProperties to create the grid for
    /// \return             the render grid
    //----------------------------------------------------------------------------------------------------------------------
    std::vector<std::vector<std::vector<float>>> render3dGrid(ParticleProperties *p);

    Vec3 getGridXYZ(int k);
    int getrenderoption();
    void drawLoading();

    /// called when down arrow is pressed - increases resolution of marching squares render
    void increase2DResolutionWORLD();

    /// called when up arrow is pressed - increases resolution of marching squares render
    void decrease2DResolutionWORLD();

    /// returns m_snapshotMode
    int getSnapshotMode();

    /// DRAWS A SQUARE OF m_particleTypes[todraw]
    void drawCube();



private:
    /// Keep track of whether this has been initialised - otherwise it won't be ready to draw!
    bool m_isInit;

    /// The time since the object was initialised, which is used for animation purposes
    double m_startTime;

    /// A member that is updated when update() is called indicating the elapsed time
    double m_elapsedTime;

    double m_timestep;

    // PARTICLES
    std::vector<Particle> m_particles;
    int m_particlesPoolSize;
    int m_firstFreeParticle;  // These two ints are needed for efficient insert and deletion
    int m_lastTakenParticle;  // See: insertParticle() and deleteParticle()
    int m_howManyAliveParticles;
    std::vector<ParticleProperties> m_particleTypes;

    // SPATIAL HASH
    std::vector<std::vector<Particle *>> m_grid;
    std::vector<bool> m_cellsContainingParticles;

    // WORLD SIZE ATTRIBUTES
    float m_halfwidth, m_halfheight;
    float m_interactionradius;
    float m_squaresize;
    int m_gridheight, m_gridwidth, m_griddepth;
    int m_pixelwidth, m_pixelheight;

    // RENDERING ATTRIBUTES
    float m_pointsize;
    float m_mainrender3dthreshold;
    float m_mainrender2dthreshold;
    int m_render2DResolution;
    int m_render2dwidth, m_render2dheight;
    int m_render3dresolution;
    int m_render3dwidth, m_render3dheight;
    int m_renderoption;

    int m_snapshotmultiplier;

    // SPRING ATTRIBUTES
    std::vector<Particle::Spring> m_springs;
    int m_firstFreeSpring;
    int m_lastTakenSpring;
    int m_springsize;

    // INTERACTION ATTRIBUTES
    bool m_rain;
    bool m_drawwall;
    bool m_gravity;
    std::vector<Particle *> m_draggedParticles;
    int m_previousmousex, m_previousmousey;

    // FUN PARTICLE TYPES
    /// Determines which particle type to draw, is index of m_particleTypes
    int m_todraw;
    int m_howmanytimesrandomized;

    // 3D ATTRIBUTES
    bool m_3d;
    float m_camerarotatey, m_camerarotatex;
    int m_snapshotMode;

    // BOUNDARYS
    float m_boundaryMultiplier;
    int m_boundaryType;

    MarchingAlgorithms m_marching;
};

#endif // WORLD_H
