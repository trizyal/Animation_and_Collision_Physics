To compile, you will need to do the following:
qmake -project "QT += core gui widgets opengl openglwidgets" "LIBS += -lGL -lGLU"
qmake
make

You may see a compiler warning about a macro collision between Qt and OpenGL, which can be ignored.

CONTROLS:
=========
- space: pause/unpause the character movement. Does not reset the ball/dodecahedron physics.
- w: move the character right/forward.
- s: move the character left/backward.
- m: switch between the ball and dodecahedron. This resets the ball physics.
- l: switch between the 3 land models. This resets the ball physics.


PHYSICS:
========
- gravity is a constant acceleration downwards. g = (0.0, 0.0, -9.8) because the z-axis is up.
- hence the velocity is calculated as v = v + g * dt, where dt is the time step in seconds.
- the position is calculated as p = p + v * dt.
- velocity from collision is calculated using impulse and restitution,
    but also a facter of 1.15 is multiplied to the impulse.
- angular velocity is calculated as l = l + torque * dt, where torque is the torque vector.
- orientation is calculated from the angular velocity directly by using Quaternions.
