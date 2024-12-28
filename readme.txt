# University of Leeds 2024-25
# COMP 5823M Assignment 2 Bouncing Dodecahedra of Doom

UNIVERSITY LINUX:
=================

To compile on the University Linux machines, you will need to do the following:

qmake -project "QT += core gui widgets opengl openglwidgets" "LIBS += -lGL -lGLU"^C

You should see a compiler warning about a macro collision between Qt and OpenGL, which can be ignored.


