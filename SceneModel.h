///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	SceneModel.h
//	------------------------
//	
//	The model of the scene
//	
///////////////////////////////////////////////////

#ifndef __SCENE_MODEL_H
#define __SCENE_MODEL_H

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "IndexedFaceSurface.h"
#include "Terrain.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "BVHData.h"

// struct to hold one model
struct Models
{
	// IndexedFaceSurface *model;
	int creationFrame;
	Cartesian3 position;
	Cartesian3 linearVelocity;
	Cartesian3 angularVelocity;
	Matrix4 orientationR;
};

class SceneModel										
	{ // class SceneModel
	public:	
	// three terrain models
	Terrain flatLandModel;
	Terrain stripeLandModel;
	Terrain rollingLandModel;

	// and a pointer to keep track of the active one
	Terrain *activeLandModel;

	// two character models
	BVHData standSkeletonModel;
	BVHData runSkeletonModel;

	// and a pointer to keep track of the active one
	BVHData *activeSkeletonModel;

	// Movent of the character
	GLfloat characterAngle;
	bool isRunning;
	GLfloat characterXPosition;
	GLfloat characterSpeed;

	// interpolation between the two character models
	int interpolationFrames;
	int interpolationPoint;
	bool interpolate;

	// sphere models
	IndexedFaceSurface sphereModel;
	IndexedFaceSurface dodecahedronModel;

	IndexedFaceSurface *activeModel;

	Cartesian3 const gravity = Cartesian3(0.0, 0.0, -0.07);
	// Cartesian3 modelPosition;
	// Cartesian3 modelVelocity;
	// Cartesian3 modelAngularVelocity;
	// Matrix4 modelOrientationR;

	std::vector<Models> models;

	// the view matrix - updated by the interface code
	Matrix4 viewMatrix;

	// the frame number for use in animating
	unsigned long frameNumber;
	
	// constructor
	SceneModel();

	// routine that updates the scene for the next frame
	void Update();

	// routine to tell the scene to render itself
	void Render();

	// character control events: WASD
	void EventCharacterForward();
	void EventCharacterBackward();
	
	// reset game
	void ResetGame();

	// routine to reset the simulation
	void ResetPhysics();

	// routine to switch between flat land and rolling land
	void SwitchLand();
	
	// routine to switch between sphere and dodecahedron
	void SwitchModel();
	
	// routine to rotate launch direction to the left
	void RotateLaunchLeft();
	
	// and to rotate to right
	void RotateLaunchRight();

	Cartesian3 findCollisionVertex(Models model);

	}; // class SceneModel

#endif
	
