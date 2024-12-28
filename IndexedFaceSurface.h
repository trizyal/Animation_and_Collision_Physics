///////////////////////////////////////////////////
//
//	Hamish Carr
//	December, 2023
//
//	------------------------
//	IndexedFaceSurface.h
//	------------------------
//	
//	This class is modified from previous assignments
//	to use indexed face files rather than triangle 
//	soup, since this makes the task easier
//	
///////////////////////////////////////////////////

#ifndef _INDEXED_FACE_SURFACE_H
#define _INDEXED_FACE_SURFACE_H

#include <vector>

#include "Cartesian3.h"
#include "Matrix3.h"

class IndexedFaceSurface
	{ // class IndexedFaceSurface
	public:
	// vector to hold the vertex indices for each face (in CCW) as usual
	std::vector<int> faceVertices;

	// vector to store vertex
	std::vector<Cartesian3> vertices;

	// vector to hold corresponding normal vectors
	std::vector<Cartesian3> normals;

	// constructor will initialise to safe values
	IndexedFaceSurface();
	
	// read routine returns true on success, failure otherwise
	bool ReadFileIndexedFace(const char *fileName);
	
	// routine to compute unit normal vectors
	void ComputeUnitNormalVectors();
	
	// routine to render
	void Render();
	
	// routine to dump out as indexed face file
	void WriteIndexedFace();

	}; // class IndexedFaceSurface

#endif
