//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5823M Animation & Simulation
//
//  September, 2020
//
//  ------------------------
//  Matrix3.h
//  ------------------------
//  
//  A minimal class for a 3x3 Cartesian matrix
//  
///////////////////////////////////////////////////

// include guard
#ifndef __MATRIX3_H
#define __MATRIX3_H

#include <iostream>
#include "Cartesian3.h"

#ifndef M_PI
#define M_PI 3.141592
#endif
#define DEG2RAD(x) (M_PI*(float)(x)/180.0)

// forward declaration
class Matrix3;

// the class itself, stored in row-major form
class Matrix3
	{ // Matrix3
	public:
	// the coordinates
	float coordinates[3][3];

	// constructor - default to the zero matrix
	Matrix3();

	// equality operator
	bool operator ==(const Matrix3 &other) const;

	// indexing - retrieves the beginning of a line
	// array indexing will then retrieve an element
	float * operator [](const int rowIndex);

	// similar routine for const pointers
	const float * operator [](const int rowIndex) const;

	// scalar operations
	// multiplication operator (no division operator)
	Matrix3 operator *(float factor) const;

	// vector operations on Cartesian coordinates
	Cartesian3 operator *(const Cartesian3 &vector) const;

	// matrix operations
	// addition operator
	Matrix3 operator +(const Matrix3 &other) const;
	// subtraction operator
	Matrix3 operator -(const Matrix3 &other) const;
	// multiplication operator
	Matrix3 operator *(const Matrix3 &other) const; 

	// matrix transpose
	Matrix3 transpose() const;

	// routine that returns a row vector 
	Cartesian3 row(int rowNum);

	// and similar for a column
	Cartesian3 column(int colNum);

	// methods that return particular matrices
	static Matrix3 Zero();

	// the identity matrix
	static Matrix3 Identity();

	// rotations around main axes
	static Matrix3 RotateX(float degrees);
	static Matrix3 RotateY(float degrees);
	static Matrix3 RotateZ(float degrees);

	// routine to transpose a matrix
	Matrix3 Transpose();
	
	// matrix inverse
	Matrix3 Inverse();
	}; // Matrix3

// scalar operations
// additional scalar multiplication operator
Matrix3 operator *(float factor, const Matrix3 &matrix);

// stream input
std::istream & operator >> (std::istream &inStream, Matrix3 &value);

// stream output
std::ostream & operator << (std::ostream &outStream, const Matrix3 &value);
        
#endif
