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
// const GLfloat characterColour[4] = { 1.0, 1.0, 0.0, 1.0 };
GLfloat characterColour[4] = { 1.0, 1.0, 0.0, 1.0 };
const GLfloat sunAmbient[4] = {0.1, 0.1, 0.1, 1.0 };
const GLfloat sunDiffuse[4] = {0.7, 0.7, 0.7, 1.0 };
const GLfloat blackColour[4] = {0.0, 0.0, 0.0, 1.0};

const GLfloat redColor[4] = {1.0, 0.0, 0.0, 1.0};
const GLfloat yellowColor[4] = {1.0, 1.0, 0.0, 1.0};
const GLfloat blueColor[4] = {0.0, 0.0, 1.0, 1.0};

const float elasticityCoeff = 0.6;
int collisionCount = 0;
float momentofInertia = (39.0 * ((1.0 + sqrt(5.0)) / 2) + 28) / 150;
const float frictionCoeff = 0.1;


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

	characterAngle = 90.0;
	isRunning = false;
	characterXPosition = 0.0;
	characterSpeed = 0.0;

	interpolationFrames = 0;
	interpolationPoint = 0;
	interpolate = false;

	// modelPosition = Cartesian3(10.0, 0.0, 10.0);
	// modelVelocity = Cartesian3(0.0, 0.0, 0.0);
	// modelAngularVelocity = Cartesian3(0.0, 0.0, 0.0);
	// modelOrientationR = Matrix4::Identity();

	Models model;
	model.creationFrame = 0;
	model.position = Cartesian3(10.0, 0.0, 10.0);
	model.linearVelocity = Cartesian3(0.0, 0.0, 0.0);
	model.angularVelocity = Cartesian3(0.0, 0.0, 0.0);
	model.orientationR = Matrix4::Identity();
	models.push_back(model);
	
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

		// if (frameNumber > 15)
		// {
		// 	frameNumber = 0;
		// 	std::cout << "16 frames" << std::endl;
		// }
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

	if (characterAngle == 90.0 && isRunning)
	{
		characterXPosition += characterSpeed;
	}
	else if (characterAngle == -90.0 && isRunning)
	{
		characterXPosition -= characterSpeed;
	}

	auto simpleHeight = activeLandModel->getHeight(characterXPosition, 0.0);
	// std::cout << "Simple Height: " << simpleHeight << std::endl;
	auto myHeight = activeLandModel->getHeightBilinear(characterXPosition, 0.0);
	// std::cout << "My Height: " << myHeight << std::endl;

	// float height = simpleHeight;
	float characterZPosition = myHeight;

	glTranslatef(characterXPosition, 0.0, characterZPosition);
	glRotatef(characterAngle, 0.0, 0.0, 1.0);
	glScalef(0.025f, 0.025f, 0.025f);

	if (interpolationFrames <= 5 && isRunning)
	{
		// interpolate the character's pose
		characterSpeed += 0.02;
		activeSkeletonModel->InterpolateToRun(standSkeletonModel, runSkeletonModel, interpolationFrames);
		frameNumber = 0;
	}
	else if (interpolationFrames <= 5 && interpolate )
	{
		activeSkeletonModel->InterpolateToPose(runSkeletonModel, standSkeletonModel, interpolationFrames, interpolationPoint);
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

	if (this->models.size() > 4)
	{
		std::cout<< "Size: " << this->models.size() << std::endl;
		// delete the first ball
		this->models.erase(this->models.begin());
		std::cout<< "Size: " << this->models.size() << std::endl;
	}


	// every 5 seconds maek a new ball
	if (frameNumber % 72 == 0)
	{
		// random position in x -20 to 20 and z 10 to 20
		float x = (rand() % 40) - 20;
		float z = (rand() % 10) + 10;

		Models model;
		model.creationFrame = frameNumber;
		model.position = Cartesian3(x, 0.0, z);
		model.linearVelocity = Cartesian3(0.0, 0.0, 0.0);
		model.angularVelocity = Cartesian3(0.0, 0.0, 0.0);
		model.orientationR = Matrix4::Identity();
		this->models.push_back(model);
	}


	for (auto& model : this->models)
	{
		// calculate velocity of the ball
		model.linearVelocity = model.linearVelocity + gravity;
		// calculate position of the ball
		model.position = model.position + model.linearVelocity;

		float planeHeight = activeLandModel->getHeightBilinear(model.position.x, model.position.y);
		float radius = 1.0;

		// collision detection with the terrain
		if (model.position.z <= planeHeight + radius && fabs(model.linearVelocity.z) > 0.01)
		{
			model.position.z = planeHeight + radius;

			// calculate the normal of the terrain
			Cartesian3 normal = activeLandModel->getNormal(model.position.x, model.position.y);
			normal = normal.unit();

			float VdotN = model.linearVelocity.dot(normal);
			auto velocity_normal = normal * VdotN;
			auto velocity_tangent = model.linearVelocity - velocity_normal;
			auto impulse = -(1 + elasticityCoeff) * VdotN;
			auto J = impulse * normal * 1.15;

			auto collisionVertex = findCollisionVertex(model);
			auto r = collisionVertex - model.position;
			// auto torque = r.cross(J);
			auto torque = r.cross(J);

			// apply friction
			auto friction = -frictionCoeff * velocity_tangent;
			// J = J + friction;

			model.linearVelocity = model.linearVelocity + J;
			model.angularVelocity = model.angularVelocity + torque; // basically is the angular velocity
		}

		// model.angularVelocity = model.angularVelocity.unit();
		auto halfTheta = model.angularVelocity.length() / 2;
		// model.angularVelocity = model.angularVelocity.unit();
		// std::cout<< "Half Theta: " << halfTheta << std::endl;
		auto w = cos(halfTheta);
		auto x = model.angularVelocity.x * sin(halfTheta);
		auto y = model.angularVelocity.y * sin(halfTheta);
		auto z = model.angularVelocity.z * sin(halfTheta);

		Quaternion rotationQuaternion = Quaternion(x, y, z, w);
		auto rotationMatrix = rotationQuaternion.Unit().GetMatrix();
		model.orientationR = rotationMatrix * model.orientationR;
		// model.orientationR = model.orientationR;

		// std :: cout << "Orientation: " << model.orientationR << std::endl;


		glPushMatrix();

		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ballColour);
		glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
		glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

		glTranslatef(model.position.x, model.position.y, model.position.z);
		// glMultMatrixf(rotationMatrix.GetRotationMatrix().columnMajor().coordinates);
		glMultMatrixf(model.orientationR.GetRotationMatrix().columnMajor().coordinates);
		activeModel->Render();

		glPopMatrix();



		// test collision between the ball and the character
		// horizontal collision
		float dx = model.position.x - characterXPosition;
		float dy = model.position.y - 0.0;
		float distance = sqrt(dx * dx + dy * dy);
		bool hD = distance <= 1.0 + 0.3;

		// vertical collision
		float zBottom = characterZPosition; // beacause the feet are places the terrainheight
		float zTop = characterZPosition + 1.8; // 1.8
		bool vD = model.position.z + 1 >= zBottom && model.position.z - 1 <= zTop;

		if (hD && vD)
		{
			collisionCount++;
			std::cout << "Collision: " << collisionCount << std::endl;
			characterColour[1] = 0.0;
		}
		else
		{
			characterColour[1] = 1.0;
		}

	}


} // Render()


Cartesian3 SceneModel::findCollisionVertex(Models model)
{
	Cartesian3 collisionVertex = Cartesian3(0.0, 0.0, 0.0);
	float smallestDistance = 1000000.0;
	int index = -1;

	// std::cout << "Reached 1" << std::endl;

	for ( auto i = 0; i < activeModel->vertices.size(); ++i)
    {
		auto vertex = model.orientationR * activeModel->vertices[i] + model.position;
		float terrainZ = activeLandModel->getHeight(model.position.x, model.position.y);

		float distance = vertex.z - terrainZ;

		if (distance < smallestDistance)
	    {
	        smallestDistance = distance;
	        collisionVertex = vertex;
			index = i;
	    }

    }
	// std::cout << "Index: " << index << std::endl;
	return collisionVertex;
}




// character control events: W for forward
void SceneModel::EventCharacterForward()
{ // EventCharacterForward()
	characterAngle = 90.0;
	// interpolationFrames = 0;
} // EventCharacterForward()

// character control events: S for backward
void SceneModel::EventCharacterBackward()
{ // EventCharacterBackward()
	characterAngle = -90.0;
	// interpolationFrames = 0;
} // EventCharacterBackward()


void SceneModel::ResetGame()
{ // ResetGame()
	if (isRunning)
	{
		interpolationFrames = 0;
		isRunning = false;
		interpolate = true;
		interpolationPoint = frameNumber;
		this->activeSkeletonModel = &standSkeletonModel;
	}
	else
	{
		std::cout << "Running" << std::endl;
		interpolationFrames = 0;
		characterSpeed = 0.0;
		isRunning = true;
		this->activeSkeletonModel = &runSkeletonModel;
	}
	// reset the frame number
	interpolationFrames = 0;
	frameNumber = 0;
    this->ResetPhysics();
} // ResetGame()

// routine to reset the simulation
void SceneModel::ResetPhysics()
{ // ResetPhysics()
	std::cout << "Resetting Physics." << std::endl;
	std::cout << "Collision: (in how many frames) " << collisionCount << std::endl;
	collisionCount = 0;
	// reset the ball position




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

	models.clear();
	Models model;
	model.creationFrame = frameNumber;
	model.position = Cartesian3(10.0, 0.0, 10.0);
	model.linearVelocity = Cartesian3(0.0, 0.0, 0.0);
	model.angularVelocity = Cartesian3(0.0, 0.0, 0.0);
	model.orientationR = Matrix4::Identity();
	models.push_back(model);

	ResetPhysics();
	} // SwitchLand()
	
// routine to switch between sphere and dodecahedron
void SceneModel::SwitchModel()
{ // SwitchModel()
	if (activeModel == &sphereModel)
		activeModel = &dodecahedronModel;
	else if (activeModel == &dodecahedronModel)
        activeModel = &sphereModel;

	models.clear();
	Models model;
	model.creationFrame = frameNumber;
	model.position = Cartesian3(10.0, 0.0, 10.0);
	model.linearVelocity = Cartesian3(0.0, 0.0, 0.0);
	model.angularVelocity = Cartesian3(0.0, 0.0, 0.0);
	model.orientationR = Matrix4::Identity();
	models.push_back(model);

	// and reset the physics
	ResetPhysics();
} // SwitchModel()
