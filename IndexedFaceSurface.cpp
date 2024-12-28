///////////////////////////////////////////////////
//
//	Hamish Carr
//	January, 2018
//
//	------------------------
//	IndexedFaceSurface.cpp
//	------------------------
//	
//	This is a modified version of the Geometric Processing
//	GeometricSurfaceFaceDS class that uses Homogeneous4 
// 	instead of Cartesian coordinates and that precomputes
//	normal vectors for all of the triangles
//	This version DOES NOT compute bounding spheres or midpoints
//	ALL transformations are up to the user.
//	
///////////////////////////////////////////////////


#include "IndexedFaceSurface.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <cstring>

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

// constructor will initialise to safe values
IndexedFaceSurface::IndexedFaceSurface()
	{ // IndexedFaceSurface::IndexedFaceSurface()
	// force the size to nil (should not be necessary, but . . .)
	vertices.resize(0);
	normals.resize(0);
	} // IndexedFaceSurface::IndexedFaceSurface()

// read routine returns true on success, failure otherwise
bool IndexedFaceSurface::ReadFileIndexedFace(const char *fileName)
	{ // IndexedFaceSurface::ReadFileIndexedFace()
	// open the input file
	std::ifstream inFile(fileName);
	if (inFile.bad())
		return false;
	
	// assume that file format is ABSOLUTELY predictable and the lines aren't too long
	char lineBuffer[256];

	// spare token to read into
	char token[256];

	// set the number of vertices and faces
	long nTriangles = 0, nVertices = 0;
	
	// the first four lines will be skipped completely
	inFile.getline(lineBuffer, 256);
	inFile.getline(lineBuffer, 256);
	inFile.getline(lineBuffer, 256);
	inFile.getline(lineBuffer, 256);

	// read the only header line we care about
	inFile.getline(lineBuffer, 256);
	sscanf(lineBuffer, "# Surface vertices=%ld faces=%ld", &nVertices, &nTriangles); 

	// the next line is skipped
	inFile.getline(lineBuffer, 256);

	// now allocate space for them all
	vertices.resize(nVertices);
	faceVertices.resize(nTriangles*3);
	
	// now loop to read the vertices in, and hope nothing goes wrong
	for (long vertex = 0; vertex < nVertices; vertex++)
		{ // for each vertex
		// ID of the vertex
		long vertexID;
		// scan in the vertex ID
		inFile >> token >> vertexID;

		// if it didn't scan properly, or was the wrong ID, exit
		if ((strcmp(token, "Vertex") != 0) || (vertexID != vertex))
			{ // scan failed
			printf("Invalid vertex %ld\n", vertex); 
			exit(0); 
			} // scan failed

		// now read in the vertex
		inFile >> vertices[vertex].x >> vertices[vertex].y >> vertices[vertex].z;
		} // for each vertex

	// similarly, loop to read the faceVertices in
	for (long face = 0; face < (long) faceVertices.size()/3; face++)
		{ // for each face
		// ID of the face
		long faceID;
		// scan in the vertex ID
		inFile >> token >> faceID;
		// if it didn't scan, or was the wrong ID, exit
		if ((strcmp("Face", token) != 0) || (faceID != face))
			{ // scan failed
			printf("Invalid face %ld\n", face); 
			exit(0); 
			} // scan failed

		// read in the vertex IDs
		inFile >> faceVertices[3*face] >> faceVertices[3*face+1] >> faceVertices[3*face+2];
		} // for each face		

	// call the routine to compute normals
	ComputeUnitNormalVectors();

	return true;
	} // IndexedFaceSurface::ReadFileIndexedFace()

// routine to compute unit normal vectors
void IndexedFaceSurface::ComputeUnitNormalVectors()
	{ // ComputeUnitNormalVectors()
	// assume that the triangle vertices are set correctly, and allocate one third of that for normals
	normals.resize(faceVertices.size() / 3);
	
	// loop through the triangles, computing normal vectors
	for (int triangle = 0; triangle < (int) normals.size(); triangle++)
		{ // per triangle
		// retrieve the three vertices in Cartesian form
		Cartesian3 vertexP = vertices[faceVertices[3 * triangle		]	];
		Cartesian3 vertexQ = vertices[faceVertices[3 * triangle + 1	]	];
		Cartesian3 vertexR = vertices[faceVertices[3 * triangle + 2	]	];
		// compute two edge vectors
		Cartesian3 vectorU = vertexQ - vertexP;
		Cartesian3 vectorV = vertexR - vertexP;
		// compute a normal with the cross-product
		normals[triangle] = vectorU.cross(vectorV).unit();
		} // per triangle
	} // ComputeUnitNormalVectors()

// routine to render
void IndexedFaceSurface::Render()
	{ // IndexedFaceSurface::Render()
	// walk through the faces rendering each one
	glBegin(GL_TRIANGLES);
	
	// we loop through all of the triangles
	for (int triangle = 0; triangle < (int) normals.size(); triangle++)
		{ // per triangle
		// this works because C++ guarantees that the POD data is in exactly
		// the order stated in the class with no padding.
		glNormal3fv(&normals[triangle].x);
		glVertex3fv(&vertices[faceVertices[3 * triangle		]	].x);
		glVertex3fv(&vertices[faceVertices[3 * triangle + 1	]	].x);
		glVertex3fv(&vertices[faceVertices[3 * triangle + 2	]	].x);
		} // per triangle
	
	glEnd();
	} // IndexedFaceSurface::Render()

// routine to dump out as indexed face file
void IndexedFaceSurface::WriteIndexedFace()
	{ // IndexedFaceSurface::WriteIndexedFace()
	// output the header file
	std::cout << "#" << std::endl;
	std::cout << "# Created for Leeds COMP 5823M Autumn 2023" << std::endl;
	std::cout << "#" << std::endl;
	std::cout << "#" << std::endl;
	std::cout << "# Surface vertices=" << vertices.size() << " faces=" << (faceVertices.size()/3) << std::endl;
	std::cout << "#" << std::endl;
	
	// loop to write the vertices out
	for (long vertex = 0; vertex < (long) vertices.size(); vertex++)
		std::cout << "Vertex " << std::setw(3) << vertex << " " << vertices[vertex] << std::endl;

	// loop to write the normals out
	for (long vertex = 0; vertex < (long) normals.size(); vertex++)
		std::cout << "Normal " << std::setw(3) << vertex << " " << normals[vertex] << std::endl;

	// loop to write the face vertices out
	for (long face = 0; face < (long) faceVertices.size()/3; face++)
		std::cout << "Face " << std::setw(3) << face << " " << faceVertices[3*face] << " " << faceVertices[3*face+1] << " " << faceVertices[3*face+2] << std::endl;
	} // IndexedFaceSurface::WriteIndexedFace()
	
	

