# ParticlePanic
A 2D physics engine which can simulate fluid-like particles.

The first aim of this project is to simulate fluid-like behavior with point-like circle particles.
Completed 30 March 2016

Then as I move on I will implement a nice visual style involving marching squares.
Completed 13 April 2016

A stretch goal would be to implement it in 3D. 
Completed 23 April 2016

--------------------HOW TO USE----------------
The icons at the top are as follows:
(1) Draw: click this to be able to draw water by click and dragging in window below icons.
(2) Erase: click this to be able to erase particles by left clicking and dragging
(3) Hand: click this to be able to drag clumps of particles around
(4) Tap: click this to turn on/off the tap.
(5) Gravity: click this to turn on/off gravity.
(6) Rubbish: click this to clear the world of all particles.
(7) Help: click to display help much like this
(8) Dropdown Menu: click to select which particle type to draw / tap with
(9) Seed box: type in numbers from your keyboard to write your own random seed
(10) Seed generator: click this to generate a random seed.

(11) Snapshot : located in bottom right only in 3D mode. When pressed a high resolution
	        marching cubes polygons are rendered. Click and drag to rotate them.

There are also keyboard shortcuts:
'w' : able to draw walls
'r' : change the render option
'g' : turn gravity on/off
'p' : 3D mode! Clicking and dragging in this mode rotates the camera.
'o' : 2D mode.
arrow up : increase marching squares resolution
arrow down: decrease marching squares resolution

MUST-TRY: 
Select slime in dropdown menu and then click 'c' on your keyboard.
A kind of square squishy object will appear which you can drag around.

--------------------ALGORITHMS----------------

I used this SIGGRAPH paper's algorithms to simulate fluids: 

Simon Clavet, Philippe Beaudoin, and Pierre Poulin, Particle-based Viscoelastic Fluid Simulation [online], 2005, Available from:
https://pdfs.semanticscholar.org/6090/b0c36b0867ee249648072caf641a0fb37009.pdf [accessed April 2016]

The bulk of their algorithms is inside World.cpp 's update() function. I implemented most of what they
wrote in the paper: double density relaxation, viscosity and springs.
I did not implement particle-body interactions or stickiness.

Using marching squares wikipedia page I implemented my own algorithm for linear interpolation
of marching cubes. They work quite well and fast. 

Using Paul Bourke's website I implemented a marching cubes algorithm. It is much slower than
marching squares. However you can get a nice result using the snapshot button.

Bourke, Paul, Polygonising a scalar field [online], 1994, [Accessed 2016].
Available from: <http://paulbourke.net/geometry/polygonise/>.







