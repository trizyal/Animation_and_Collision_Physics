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

// this is 24 fps nominal speed
float frameTime = 0.0416667;

const Homogeneous4 sunDirection(0.5, -0.5, 0.3, 0.0);
const GLfloat groundColour[4] = { 0.2, 0.5, 0.2, 1.0 };
const GLfloat ballColour[4] = { 0.6, 0.6, 0.6, 1.0 };
const GLfloat characterColour[4] = { 1.0, 1.0, 0.0, 1.0 };
const GLfloat characterCollisionColour[4] = { 1.0, 0.0, 0.0, 1.0 };
auto activeCharacterColour = characterColour;
const GLfloat sunAmbient[4] = {0.1, 0.1, 0.1, 1.0 };
const GLfloat sunDiffuse[4] = {0.7, 0.7, 0.7, 1.0 };
const GLfloat blackColour[4] = {0.0, 0.0, 0.0, 1.0};

const float elasticityCoeff = 0.6;
int collisionCount = 0;
float momentofInertia = (39.0 * ((1.0 + sqrt(5.0)) / 2) + 28) / 150;
const float frictionCoeff = 0.1;
const float minVelocity = 0.01;
const float minAngularVelocity = 0.001;

auto previoustime = std::chrono::system_clock::now();


// constructor
SceneModel::SceneModel()
    { // constructor

    // load landscape models from files
    flatLandModel.ReadFileTerrainData(flatLandModelName, 3);
    stripeLandModel.ReadFileTerrainData(stripeLandModelName, 3);
    rollingLandModel.ReadFileTerrainData(rollingLandModelName, 3);

	standSkeletonModel.ReadFileBVH(motionBvhStand);
	runSkeletonModel.ReadFileBVH(motionBvhRun);

	sphereModel.ReadFileIndexedFace(sphereModelName);
	dodecahedronModel.ReadFileIndexedFace(dodecahedronModelName);

	// set the reference for the terrain model to use
    // this->activeLandModel = &flatLandModel;
    this->activeLandModel = &stripeLandModel;
	this->activeSkeletonModel = &standSkeletonModel;
	this->activeModel = &dodecahedronModel;
	// this->activeModel = &sphereModel;

	characterOrientation = lookingAhead;
	isRunning = false;
	characterXPosition = 0.0;
	characterSpeed = 0.0;

	interpFrameNumber = 0;
	interpFramePoint = 0;
	isStopping = false;

	// initiallising the models vector to store the ball information
	Models model;
		model.creationFrame = 0;
		model.position = Cartesian3(10.0, 0.0, 10.0);
		model.linearVelocity = Cartesian3(0.0, 0.0, 0.0);
		model.angularVelocity = Cartesian3(0.5, 0.0, 0.0);
		model.orientationR = Matrix4::Identity();
	models.push_back(model);
	
	// set the initial view matrix
	viewMatrix = Matrix4::Translate(Cartesian3(0.0, 15.0, -10.0));

	// and set the frame number to 0
	frameNumber = 0;

	// set initial time
	previoustime = std::chrono::system_clock::now();
		
	// call the reset routine to initialise the ball position
	ResetPhysics();
	} // constructor

// routine that updates the scene for the next frame
void SceneModel::Update()
	{ // Update()
		// increment the frame number
		frameNumber++;
		// if we are interpolating
		interpFrameNumber++;
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



	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, activeCharacterColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);


	if (characterOrientation == lookingAhead && isRunning)
	{
		characterXPosition += characterSpeed;
	}
	else if (characterOrientation == lookingBehind && isRunning)
	{
		characterXPosition -= characterSpeed;
	}

	float characterZPosition = activeLandModel->getHeight(characterXPosition, 0.0);
	glTranslatef(characterXPosition, 0.0, characterZPosition);
	glRotatef(characterOrientation, 0.0, 0.0, 1.0);
	glScalef(0.025f, 0.025f, 0.025f);

	if (interpFrameNumber <= 5 && isRunning)
	{
		// interpolate to the characters running pose at frame = 0;
		characterSpeed += 0.02;
		activeSkeletonModel->InterpolateToRun(standSkeletonModel, runSkeletonModel, interpFrameNumber);
		frameNumber = 0;
	}
	else if (interpFrameNumber <= 10 && isStopping )
	{
		activeSkeletonModel->InterpolateToPose(runSkeletonModel, standSkeletonModel, interpFrameNumber, interpFramePoint);
	}
	else
	{
		if (isRunning)
		{
			// speed slowly increases to 0.4
			if (characterSpeed < 0.4)
				characterSpeed += 0.05;
			else
				characterSpeed = 0.4;
		}
		// render the character
		activeSkeletonModel->Render(frameNumber%16);
	}


	glPopMatrix();

	int maxBallCount = 4;
	if (this->models.size() > maxBallCount)
	{
		// delete the first ball
		this->models.erase(this->models.begin());
	}


	// every 5 seconds make a new ball
	if (frameNumber % 24 == 0 && frameNumber > 0)
	{
		// random position in x -20 to 20 and z 10 to 20
		float x = (rand() % 40) - 20;
		float z = (rand() % 10) + 10;

		Models model;
		model.creationFrame = frameNumber;
		model.position = Cartesian3(x, 0.0, z);
		model.linearVelocity = Cartesian3(0.0, 0.0, 0.0);
		model.angularVelocity = Cartesian3(0.1, 0.0, 0.0);
		model.orientationR = Matrix4::Identity();
		this->models.push_back(model);
	}

	// flag to check if there is a collision
	// set before all the models
	// as it should be flagged if ANY of the balls
	// collide with the character
	bool collision = false;

	auto currentTime = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previoustime);
	auto delT = duration.count() / 1000.0;
	previoustime = currentTime;

	int gravity_scale_up = 3;

	for (auto& model : this->models)
	{
		// calculate velocity of the ball
		// v = u + at
		// model.linearVelocity = model.linearVelocity + gravity * delT;
		// gravity is scaled up for better effect
		model.linearVelocity = model.linearVelocity + gravity * gravity_scale_up * delT;

		// calculate position of the ball
		// x = x + vt
		model.position = model.position + model.linearVelocity * delT;

		float planeHeight = activeLandModel->getHeight(model.position.x, model.position.y);

		// collision detection with the terrain
		if (model.position.z <= planeHeight + ballRadius && fabs(model.linearVelocity.z) > minVelocity)
		{
			model.position.z = planeHeight + ballRadius;

			// calculate the normal of the terrain
			Cartesian3 normal = activeLandModel->getNormal(model.position.x, model.position.y);
			normal = normal.unit();

			float VdotN = model.linearVelocity.dot(normal);
			auto impulse = -(1 + elasticityCoeff) * VdotN;
			auto J = impulse * normal * 1.15;

			auto collisionVertex = findCollisionVertex(model);
			auto r = collisionVertex - model.position;
			auto torque = r.cross(J);

			model.linearVelocity = model.linearVelocity + J; // mass is assumed to be 1
			model.angularVelocity = model.angularVelocity + torque*delT; // mass is assumed to be 1

			// special case if the ball is stuck in the terrain
			if (model.position.z + model.linearVelocity.z <= planeHeight + ballRadius)
			{
				model.linearVelocity = Cartesian3(0, 0, 0);
				model.angularVelocity = Cartesian3(0, 0, 0);
			}
		}

		// calculate orientation/rotation of the ball from angular velocity
		if (model.angularVelocity.length() > minAngularVelocity)
		{
			auto halfTheta = model.angularVelocity.length() / 2;
			if (halfTheta > 0.2)
				halfTheta = 0.2;

			model.angularVelocity = model.angularVelocity.unit();
			auto w = cos(halfTheta);
			auto x = model.angularVelocity.x * sin(halfTheta);
			auto y = model.angularVelocity.y * sin(halfTheta);
			auto z = model.angularVelocity.z * sin(halfTheta);

			Quaternion rotationQuaternion = Quaternion(x, y, z, w);
			auto rotationMatrix = rotationQuaternion.Unit().GetMatrix();
			model.orientationR = rotationMatrix * model.orientationR;
		}

		glPushMatrix();

		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ballColour);
		glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
		glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

		glTranslatef(model.position.x, model.position.y, model.position.z);
		glMultMatrixf(model.orientationR.columnMajor().coordinates);

		activeModel->Render();

		glPopMatrix();



		// test collision between the ball and the character
		// horizontal collision
		float dx = model.position.x - characterXPosition;
		float dy = model.position.y - 0.0; // the character is always at y = 0
		float distance = sqrt(dx * dx + dy * dy);
		bool horizontalOverlap = distance <= ballRadius + characterWidth + 0.4; // 0.4 to account for the movement of the character per frame

		// vertical collision
		float characterBottom = characterZPosition; // because the feet are placed at the terrainheight
		float characterTop = characterZPosition + characterHeight;
		bool verticalOverlap = model.position.z + ballRadius >= characterBottom && model.position.z - ballRadius <= characterTop;

		if (horizontalOverlap && verticalOverlap)
		{
			collision = true;
			collisionCount++;
			std::cout << "Collision: " << collisionCount << std::endl;
		}
	}
	// collision with any ball recolors the character
	activeCharacterColour = collision ? characterCollisionColour : characterColour;

} // Render()


Cartesian3 SceneModel::findCollisionVertex(Models model)
{
	Cartesian3 collisionVertex = Cartesian3(0.0, 0.0, 0.0);
	float smallestDistance = 1000000.0;

	for ( auto i = 0; i < activeModel->vertices.size(); ++i)
    {
		// convert the vertex position to world coordinates
		auto vertex = model.orientationR * activeModel->vertices[i] + model.position;
		float landHeight = activeLandModel->getHeight(model.position.x, model.position.y);
		float distance = vertex.z - landHeight;

		if (distance < smallestDistance)
	    {
	        smallestDistance = distance;
	        collisionVertex = vertex;
	    }
    }
	return collisionVertex;
}




// character control events: W for forward
void SceneModel::EventCharacterForward()
{ // EventCharacterForward()
	characterOrientation = lookingAhead;
	// interpolationFrames = 0;
} // EventCharacterForward()

// character control events: S for backward
void SceneModel::EventCharacterBackward()
{ // EventCharacterBackward()
	characterOrientation = lookingBehind;
	// interpolationFrames = 0;
} // EventCharacterBackward()


void SceneModel::ResetGame()
{ // ResetGame()
	if (isRunning)
	{
		characterSpeed = 0.0;
		interpFrameNumber = 0;
		isRunning = false;
		isStopping = true;
		interpFramePoint = frameNumber%16;
		this->activeSkeletonModel = &standSkeletonModel;
	}
	else
	{
		std::cout << "Running" << std::endl;
		interpFrameNumber = 0;
		characterSpeed = 0.0;
		isStopping = false;
		isRunning = true;
		this->activeSkeletonModel = &runSkeletonModel;
	}
	// reset the frame number
	interpFrameNumber = 0;
	frameNumber = 0;
    // this->ResetPhysics();
} // ResetGame()

// routine to reset the simulation
void SceneModel::ResetPhysics()
{ // ResetPhysics()
	std::cout << "Resetting Physics." << std::endl;
	std::cout << "Collision: (in how many frames) " << collisionCount << std::endl;
	collisionCount = 0;
	// reset the ball position

	models.clear();
	Models model;
		model.creationFrame = frameNumber;
		model.position = Cartesian3(10.0, 0.0, 10.0);
		model.linearVelocity = Cartesian3(0.0, 0.0, 0.0);
		model.angularVelocity = Cartesian3(0.0, 0.0, 0.0);
		model.orientationR = Matrix4::Identity();
	models.push_back(model);

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

	ResetPhysics();
} // SwitchLand()
	
// routine to switch between sphere and dodecahedron
void SceneModel::SwitchModel()
{ // SwitchModel()
	if (activeModel == &sphereModel)
		activeModel = &dodecahedronModel;
	else if (activeModel == &dodecahedronModel)
        activeModel = &sphereModel;

	// and reset the physics
	ResetPhysics();
} // SwitchModel()
