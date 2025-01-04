///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	SceneModel.cpp
//	------------------------
//	
//	The model of the scene
//
//	
///////////////////////////////////////////////////

#include "SceneModel.h"

#include <chrono>
#include <math.h>
#include "Quaternion.h"

// three local variables with the hardcoded file names
const char* flatLandModelName		= "./models/flatland.dem";
const char* stripeLandModelName	= "./models/stripeland.dem";
const char* rollingLandModelName	= "./models/rollingland.dem";
const char* sphereModelName		= "./models/spheroid.face";
const char* dodecahedronModelName	= "./models/dodecahedron.face";

// four type of character's animation
const char* motionBvhStand		= "./models/stand.bvh";
const char* motionBvhRun			= "./models/fast_run.bvh";

// the speed of camera movement
const float cameraSpeed = 5.0;

// this is 60 fps nominal speed
const float frameTime = 0.0166667;

const Homogeneous4 sunDirection(0.5, -0.5, 0.3, 0.0);
const GLfloat groundColour[4] = { 0.2, 0.5, 0.2, 1.0 };
const GLfloat ballColour[4] = { 0.6, 0.6, 0.6, 1.0 };
const GLfloat characterColour[4] = { 1.0, 1.0, 0.0, 1.0 };
const GLfloat sunAmbient[4] = {0.1, 0.1, 0.1, 1.0 };
const GLfloat sunDiffuse[4] = {0.7, 0.7, 0.7, 1.0 };
const GLfloat blackColour[4] = {0.0, 0.0, 0.0, 1.0};

const GLfloat redColor[4] = {1.0, 0.0, 0.0, 1.0};
const GLfloat yellowColor[4] = {1.0, 1.0, 0.0, 1.0};
const GLfloat blueColor[4] = {0.0, 0.0, 1.0, 1.0};

// constructor
SceneModel::SceneModel()
    { // constructor

    // load landscape models from files
    flatLandModel.ReadFileTerrainData(flatLandModelName, 3);
    stripeLandModel.ReadFileTerrainData(stripeLandModelName, 3);
    rollingLandModel.ReadFileTerrainData(rollingLandModelName, 3);

	standSkeletonModel.ReadFileBVH(motionBvhStand);
	runSkeletonModel.ReadFileBVH(motionBvhRun);

	// set the reference for the terrain model to use
    this->activeLandModel = &flatLandModel;
	this->activeSkeletonModel = &standSkeletonModel;
	characterAngle = 90.0;
	isRunning = false;
	xMove = 0.0;
	speed = 0.0;

	interpolationFrames = 0;
	
	// set the initial view matrix
	viewMatrix = Matrix4::Translate(Cartesian3(0.0, 15.0, -10.0));

	// and set the frame number to 0
	frameNumber = 0;
		
	// call the reset routine to initialise the ball position
	ResetPhysics();
	} // constructor

// routine that updates the scene for the next frame
void SceneModel::Update()
	{ // Update()
		// increment the frame number
		frameNumber++;
		// if we are interpolating
		interpolationFrames++;

		if (frameNumber > 15)
		{
			frameNumber = 0;
			// auto time = std::chrono::system_clock::now();
			// print time in milliseconds
			// std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count() << std::endl;
		}
		// std::cout << "Frame: " << frameNumber << std::endl;
	} // Update()`

// routine to tell the scene to render itself
void SceneModel::Render()
{
	// Render()
	// enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	
	// set lighting parameters
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, blackColour);
	glLightfv(GL_LIGHT0, GL_EMISSION, blackColour);
	
	// background is sky-blue
	glClearColor(0.7, 0.7, 1.0, 1.0);

	// clear the buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	// glMatrixMode(GL_PROJECTION);

	// start with the identity
	glLoadIdentity();

	// add the final rotation from z-up to z-backwords
	glRotatef(-90.0, 1.0, 0.0, 0.0);

	// now compute the view matrix by combining camera translation & rotation
	columnMajorMatrix columnMajorViewMatrix = viewMatrix.columnMajor();
	glMultMatrixf(columnMajorViewMatrix.coordinates);

	// set the light position
	glLightfv(GL_LIGHT0, GL_POSITION, &(sunDirection.x));

	// and set a material colour for the ground
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	// render the terrain
	activeLandModel->Render();

	// set a material colour for the character
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, characterColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
	// render the character
	// standSkeletonModel.Render(0);

	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, characterColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	// if (isRunning)
	// {
	// 	if (speed < 0.4)
	// 	{
	// 		speed += 0.005;
	// 	}
	// 	else
	// 	{
	// 		speed = 0.4;
	// 	}
	// }

	if (characterAngle == 90.0 && isRunning)
	{
		xMove += speed;
	}
	else if (characterAngle == -90.0 && isRunning)
	{
		xMove -= speed;
	}

	glTranslatef(xMove, 0.0, activeLandModel->getHeight(xMove, 0.0));
	glRotatef(characterAngle, 0.0, 0.0, 1.0);
	glScalef(0.025f, 0.025f, 0.025f);

	if (interpolationFrames < 5 && isRunning)
	{
		// interpolate the character's pose
		activeSkeletonModel->InterpolateToRun(standSkeletonModel, runSkeletonModel, interpolationFrames);
	}
	else
	{
		if (isRunning)
		{
			// speed slowly increases to 0.4
			if (speed < 0.4)
			{
				speed += 0.005;
			}
			else
			{
				speed = 0.4;
			}
		}


		// render the character
		activeSkeletonModel->Render(frameNumber);
	}

	// activeSkeletonModel->Render(frameNumber);



	glPopMatrix();

    } // Render()

// character control events: W for forward
void SceneModel::EventCharacterForward()
{ // EventCharacterForward()
	characterAngle = 90.0;
	frameNumber = 0;
} // EventCharacterForward()

// character control events: S for backward
void SceneModel::EventCharacterBackward()
{ // EventCharacterBackward()
	characterAngle = -90.0;
	frameNumber = 0;
} // EventCharacterBackward()


void SceneModel::ResetGame()
{ // ResetGame()
	if (isRunning)
	{
		interpolationFrames = 0;
		isRunning = false;
		this->activeSkeletonModel = &standSkeletonModel;
	}
	else
	{
		interpolationFrames = 0;
		speed = 0.0;
		isRunning = true;
		this->activeSkeletonModel = &runSkeletonModel;
	}
	// reset the frame number
	frameNumber = 0;
    this->ResetPhysics();
} // ResetGame()

// routine to reset the simulation
void SceneModel::ResetPhysics()
{ // ResetPhysics()
	std::cout << "Resetting Physics." << std::endl;



} // ResetPhysics()
	
// routine to switch between flat land and rolling land
void SceneModel::SwitchLand()
	{ // SwitchLand()
	// toggle between terrains
	if (activeLandModel == &flatLandModel)
		activeLandModel = &stripeLandModel;
	else if (activeLandModel == &stripeLandModel)
		activeLandModel = &rollingLandModel;
	else if (activeLandModel == &rollingLandModel)
		activeLandModel = &flatLandModel;
	} // SwitchLand()
	
// routine to switch between sphere and dodecahedron
void SceneModel::SwitchModel()
{ // SwitchModel()
	// and reset the physics
	ResetPhysics();
} // SwitchModel()
