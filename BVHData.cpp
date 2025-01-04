#include "BVHData.h"

BVHData::BVHData()
	{ // constructor
	} // constructor

// read data from bvh file
bool BVHData::ReadFileBVH(const char* fileName)
{ // ReadFileBVH()
	// open the file & test success
	this->bvh_path = fileName;
	std::ifstream inFile(fileName);
	if (inFile.bad())
		return false;
	
	// load vertex, normal, face, weight
	std::vector<std::string> tokens;
	std::string line;
	while (std::getline(inFile, line))
	{  // per line
		// not an empty line
		if (!line.empty())
		{ // non-empty line
			StringSplit(line, tokens);
			if (tokens[0] == "HIERARCHY")
			{ // HIERARCHY line
				NewLine(inFile, tokens);
				Joint joint;
				ReadHierarchy(inFile, tokens, joint, -1);
				this->root = joint;
			} // HIERARCHY line
			else if (tokens[0] == "MOTION")
			{ // MOTION line
				ReadMotion(inFile);
				break;
			} // MOTION line
		} // non-empty line
	} // per line
		
	// get all joints in a sequence by searching the tree structure and store it into this class
	GetAllJoints(this->root, this->all_joints);
	// load all rotation and translation data into this class
	loadAllData(this->boneRotations, this->boneTranslations, this->frames);
	return true;
} // ReadFileBVH()

// move the file stream pointer to next line
void BVHData::NewLine(std::ifstream& inFile, std::vector<std::string>& tokens)
	{ // NewLine()
	std::string line;
	tokens.clear();
	std::getline(inFile, line);
	StringSplit(line, tokens);
	} // NewLine()

// split string with the given key character
void BVHData::StringSplit(std::string input, std::vector<std::string>& tokens)
	{ // StringSplit()
	tokens.clear();
	std::istringstream iss(input);
	while (!iss.eof()) 
		{ // not EOF
		std::string token;
		iss >> token;
		tokens.push_back(token);
		} // not EOF
	} // StringSplit()

// check whether the given string is a number
bool BVHData::isNumeric(const std::string& str)
	{ // isNumeric()
	for (char c : str)
		{ // per character
		if (!std::isdigit(c))
			return false;
		} // per character
	return true;
	} // isNumeric()

// read bvh hierarchy in a recusive way
void BVHData::ReadHierarchy(std::ifstream& inFile, std::vector<std::string>& line, Joint& joint, int parent)
	{ // ReadHierarchy()
	joint.id = this->Bones.size();
	joint.joint_name = line[1];
	this->Bones.push_back(joint.joint_name);
	this->parentBones.push_back(parent);
	int id = this->parentBones.size() - 1;
	NewLine(inFile, line);
	if (line[0] == "{")
		{ // not { line
		NewLine(inFile, line);
		while (line[0] != "}")
			{ // not } line
			// read offset by key word
			if (line[0] == "OFFSET")
				{
				joint.joint_offset[0] = std::stof(line[1]);
				joint.joint_offset[1] = std::stof(line[2]);
				joint.joint_offset[2] = std::stof(line[3]);
				}
			// read channels by key word
			else if (line[0] == "CHANNELS")
				{
				for (int i = 0; i < std::stoi(line[1]); i++)
					joint.joint_channel.push_back(line[i + 2]);
				}
			// read joint by key word
			else if (line[0] == "JOINT")
				{
				Joint child;
				ReadHierarchy(inFile, line, child, id);
				joint.Children.push_back(child);
				}
			// read end by key word
			else if (line[0] == "End")
				{
				for (int i = 0; i < 3; i++) 
					NewLine(inFile, line);
				}
			NewLine(inFile, line);
			} // not } line
		} // not { line
	} // ReadHierarchy()

// read motion(frames) from file
void BVHData::ReadMotion(std::ifstream& inFile)
	{ // ReadMotion()
	std::string line;
	std::vector<std::string> tokens;
	// Frames
	NewLine(inFile, tokens);
	this->frame_count = std::stoi(tokens[1]);
	// Frame Time:
	NewLine(inFile, tokens);
	this->frame_time = std::stof(tokens[2]);
	while (std::getline(inFile, line))
		{ // per line
		if (line.size() != 0)
			{ // non-empty line
			StringSplit(line, tokens);
			std::vector<float> frame;
			for (size_t i = 0; i < tokens.size(); i++)
				{ // per token
				if (!isNumeric(tokens[i]))
					frame.push_back(std::stof(tokens[i]));
				} // per token
			// store frame into this class
			this->frames.push_back(frame);
			} // non-empty line
		} // per line
	} // ReadMotion()

// get all joints in a sequence by searching the tree structure and store it into this class
void BVHData::GetAllJoints(Joint& joint, std::vector<Joint*>& joint_list)
	{ // GetAllJoints()
	joint_list.push_back(&joint);
	if (joint.Children.size() == 0)
		return;
	else
		for (size_t i = 0; i < joint.Children.size(); i++)
			GetAllJoints(joint.Children[i], joint_list);
	} // GetAllJoints()

// load all rotation and translation data into this class
void BVHData::loadAllData(std::vector<std::vector<Cartesian3>>& rotations, std::vector<Cartesian3>& translations, std::vector<std::vector<float>>& frames)
	{ // loadAllData()
	//store all rotations
	for (size_t i = 0; i < frames.size(); i++) // for each frame
		{ // per frame
		std::vector<Cartesian3> frame_rotations;
		loadRotationData(frame_rotations, frames[i]);
		rotations.push_back(frame_rotations);
		} // per frame
	//store all offsets/translations
	for (size_t i = 0; i < this->all_joints.size(); i++)// for each joints
		{ // per joint
		float x = this->all_joints[i]->joint_offset[0];
		float y = this->all_joints[i]->joint_offset[1];
		float z = this->all_joints[i]->joint_offset[2];
		translations.push_back(Cartesian3(x, y, z));
		} // per joint
	} // loadAllData()

// load all rotation data into this class
void BVHData::loadRotationData(std::vector<Cartesian3>& rotations, std::vector<float>& frames)
	{ // loadRotationData()
	for (size_t j = 0, j_c = 0; j < frames.size(); j_c++) // for each bone
		{ // per frame
		float rotation[3] = { 0, 0, 0 };
		for (size_t k = 0; k < this->all_joints[j_c]->joint_channel.size(); k++) // for each channel
			{ // per joint
			// load data to data structure
			if (this->all_joints[j_c]->joint_channel[k].substr(1) == "rotation")
				{
				int rotation_id = BVH_CHANNEL[this->all_joints[j_c]->joint_channel[k]];
				rotation[rotation_id - 3] = frames[j + k];
				}
			} // per joint
		Cartesian3 rot_3(rotation[0], rotation[1], rotation[2]);
		rotations.push_back(rot_3);
		j += this->all_joints[j_c]->joint_channel.size();
		} // per frame
	} // loadRotationData()



void BVHData::InterpolateToRun()
{

}




// render the bvh animation per frame
void BVHData::Render(int frame)
{ // Render()
	if (frame >= this->frame_count)
        frame = 0;
	auto rotation = this->boneRotations[frame];

	auto root = this->root;

	glPushMatrix();

	glRotatef(90, 1.0, 0.0, 0.0);
	glTranslatef(root.joint_offset[0], root.joint_offset[1], root.joint_offset[2]);

	RenderJoints(root, rotation);

	glPopMatrix();
} // Render()



void BVHData::RenderJoints(Joint joint, std::vector<Cartesian3> rotation)
{
	for (auto child : joint.Children)
	{
		glPushMatrix();

		glRotatef(rotation[joint.id].x, 1.0, 0.0, 0.0);
		glRotatef(rotation[joint.id].y, 0.0, 1.0, 0.0);
		glRotatef(rotation[joint.id].z, 0.0, 0.0, 1.0);

		// drawLine(Cartesian3(0, 0, 0), child.joint_offset);
		RenderBone(Cartesian3(0, 0, 0), Cartesian3(child.joint_offset));

		glTranslatef(child.joint_offset[0], child.joint_offset[1], child.joint_offset[2]);
		RenderJoints(child, rotation);

		glPopMatrix();
	}
}


void BVHData::RenderBone(Cartesian3 start, Cartesian3 end)
{
	float radius = 1.5;
	int segments = 10;

	// Cartesian3 v = end - start;
	float height = end.length();
	GLUquadric *quadric = gluNewQuadric();

	glPushMatrix();

	glRotatef(acos(end.z / height) * 180.0 / 3.1415926, -end.y, end.x, 0.0);
	gluCylinder(quadric, radius, radius, height, segments, segments);
	gluQuadricDrawStyle(quadric, GLU_FILL);

	glPopMatrix();
	gluDeleteQuadric(quadric);
}



// Debugging functions
void BVHData::drawSphere(const Cartesian3 point)
{
	GLUquadric *quadric = gluNewQuadric();
	glPushMatrix();
	glTranslatef(point.x, point.y, point.z);
	gluSphere(quadric, 0.7, 10, 10);
	glPopMatrix();
	gluDeleteQuadric(quadric);
}


void BVHData::drawLine(Cartesian3 start, Cartesian3 end)
{

	glBegin(GL_LINES);
	glVertex3f(start.x, start.y, start.z);
	glVertex3f(end.x, end.y, end.z);
	glEnd();

}



void BVHData::printJoints()
{
	for (auto joint : this->all_joints)
	{
		std::cout << joint->joint_name << std::endl;
		std::cout << joint->id	<< std::endl;
		std::cout << joint->joint_offset[0] << " " << joint->joint_offset[1] << " " << joint->joint_offset[2] << std::endl;
		std::cout << "Children Size: " << joint->Children.size() << std::endl;
		std::cout << "Parent: " << this->parentBones[joint->id] << std::endl << std::endl;
	}
	exit(0);
}



void BVHData::printFrames()
{
	for (auto frame : this->frames)
	{
		for (auto f : frame)
		{
			std::cout << f << std::endl;
		}
		std::cout << std::endl;
	}

	std::cout << this->frames.size() << std::endl;

	exit(0);
}




