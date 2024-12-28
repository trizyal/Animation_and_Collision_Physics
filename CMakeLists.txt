cmake_minimum_required(VERSION 3.10)
project(assignment)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -lGL -lGLU")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0 -DDEBUG -Wall -Wextra -lGL -lGLU")

set(OpenGL_GL_PREFERENCE "GLVND")
set(CMAKE_AUTOMOC ON)

add_definitions(-DQT_NO_WARNING_MACRO_COLLISION)

set( SOURCES
    main.cpp
    AnimationCycleWidget.cpp
    BVHData.cpp
    Cartesian3.cpp
    Homogeneous4.cpp
    IndexedFaceSurface.cpp
    Matrix3.cpp
    Matrix4.cpp
    Quaternion.cpp
    SceneModel.cpp
    Terrain.cpp
)

set( HEADERS
    AnimationCycleWidget.h
    BVHData.h
    Cartesian3.h
    Homogeneous4.h
    IndexedFaceSurface.h
    Matrix3.h
    Matrix4.h
    Quaternion.h
    SceneModel.h
    Terrain.h
)

add_executable(assignment ${SOURCES} ${MOC_SOURCES})

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

target_link_libraries(assignment Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)
