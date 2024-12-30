///////////////////////////////////////////////////
//
//  University of Leeds
//  Animation and Simulation
//  Xiaoyuan Yang
//	November, 2023
//
//	------------------------
//	BVHData.h
//	------------------------
//	
//	A class for loading, rendering bvh animation from file 
//	
///////////////////////////////////////////////////
#ifndef _BVHDATA_H
#define _BVHDATA_H

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

#include <vector>
#include <string>
#include <sstream>
#include "Cartesian3.h"
#include "Matrix4.h"
#include <fstream>
#include <map>
#include <math.h>

// A struct for each joint
struct Joint
{ // struct Joint
	// joint id
	int id;
	// joint name
	std::string joint_name;
	// joint offset
	float joint_offset[3] = {0.0f, 0.0f, 0.0f};
	// joint channel
	std::vector<std::string> joint_channel;
	// joint's children
	std::vector<Joint> Children;
}; // struct Joint

// bvh data class
class BVHData
{ // class BVHData
	public:
		// rest mode usually
		enum MODE
		{
			REST_MODE,
			POSE_MODE
		};
		// the file path of bvh
		std::string bvh_path;
		// the root joint of armature
		Joint root;
		// bvh frame count
		int frame_count;
		// frame rate of the animation
		float frame_time;

		// a vector to store all bones' name
		std::vector<std::string> Bones;
		// a vector to store all joints
		std::vector<Joint*> all_joints;
		// a vector to store the parent joint's id of each joint
		std::vector<int> parentBones;

		// a vector to store all frames
		std::vector<std::vector<float>> frames;

		// a vector to store all bones' offsets
		std::vector<Cartesian3> boneTranslations;
		// a vector to store all bones' rotations for each frame
		std::vector<std::vector<Cartesian3>> boneRotations;

	private:
		// id for each channel
		std::map<std::string, int> BVH_CHANNEL =
			{ // BVH_CHANNEL
				{"Xposition", 0},
				{"Yposition", 1},
				{"Zposition", 2},
				{"Xrotation", 3},
				{"Yrotation", 4},
				{"Zrotation", 5}
			}; // BVH_CHANNEL

	public:
		BVHData();
		// read data from bvh file
		bool ReadFileBVH(const char* fileName);
		// move the file stream pointer to next line
		void NewLine(std::ifstream&, std::vector<std::string>&);
		// split string with the given key character
		void StringSplit(std::string, std::vector<std::string>&);
		// read bvh hierarchy in a recusive way
		void ReadHierarchy(std::ifstream&, std::vector<std::string>&, Joint&, int parent);
		// read motion(frames) from file
		void ReadMotion(std::ifstream&);
		// get all joints in a sequence by searching the tree structure and store it into this class
		void GetAllJoints(Joint&, std::vector<Joint*>&);
		// load all rotation and translation data into this class
		void loadAllData(std::vector<std::vector<Cartesian3>>& rotations, std::vector<Cartesian3>& translations, std::vector<std::vector<float>>& frames);
		// load all rotation data into this class
		void loadRotationData(std::vector<Cartesian3> &rotations, std::vector<float>& frames);
		// check whether the given string is a number
		bool isNumeric(const std::string&);

		// render the bvh animation
		void Render();

		void drawCylinder(Cartesian3, Cartesian3);

		void drawLine(Cartesian3, Cartesian3);

		void printJoints();

};

#endif
