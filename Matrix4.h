//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  Matrix4.h
//  ------------------------
//  
//  A minimal class for a homogeneous 4x4 matrix
//  
///////////////////////////////////////////////////

// include guard
#ifndef MATRIX4_H
#define MATRIX4_H

#include <iostream>
#include "Cartesian3.h"
#include "Homogeneous4.h"
#include "Matrix3.h"

#ifndef M_PI
#define M_PI 3.141592
#endif
#define DEG2RAD(x) (M_PI*(float)(x)/180.0)

// forward declaration
class Matrix4;

// this allows us to get a matrix in the 
// column-major form preferred by OpenGL
class columnMajorMatrix
    { // class columnMajorMatrix
    public:
    float coordinates[16];
    }; // class columnMajorMatrix
    
// the class itself, stored in row-major form
class Matrix4
	{ // Matrix4
	public:
	// the coordinates
	float coordinates[4][4];

	// constructor - default to the zero matrix
	Matrix4();
	
	// constructor from a Matrix3
	Matrix4(Matrix3 &other);

	// equality operator
	bool operator ==(const Matrix4 &other) const;

	// indexing - retrieves the beginning of a line
	// array indexing will then retrieve an element
	float * operator [](const int rowIndex);

	// similar routine for const pointers
	const float * operator [](const int rowIndex) const;

	// scalar operations
	// multiplication operator (no division operator)
	Matrix4 operator *(float factor) const;

	// vector operations on homogeneous coordinates
	// multiplication is the only operator we use
	Homogeneous4 operator *(const Homogeneous4 &vector) const;

	// and on Cartesian coordinates
	Cartesian3 operator *(const Cartesian3 &vector) const;

	// matrix operations
	// addition operator
	Matrix4 operator +(const Matrix4 &other) const;
	// subtraction operator
	Matrix4 operator -(const Matrix4 &other) const;
	// multiplication operator
	Matrix4 operator *(const Matrix4 &other) const; 

	// matrix transpose
	Matrix4 transpose() const;

	// returns a column-major array of 16 values
	// for use with OpenGL
	columnMajorMatrix columnMajor() const;

	// routine that returns a row vector as a Homogeneous4
	Homogeneous4 row(int rowNum);

	// and similar for a column
	Homogeneous4 column(int colNum);

	// methods that return particular matrices
	static Matrix4 Zero();

	// the identity matrix
	static Matrix4 Identity();
	static Matrix4 Translate(const Cartesian3 &vector);

	// rotations around main axes
	static Matrix4 RotateX(float degrees);
	static Matrix4 RotateY(float degrees);
	static Matrix4 RotateZ(float degrees);

    static Matrix4 GetRotation(const Cartesian3& vector1, const Cartesian3& vector2);

	// routines to retrieve the rotation and translation component
	// NOTE:  NO ERROR CHECKING: assumes only pure rotation plus translation
	Matrix4 GetRotationMatrix();
	Cartesian3 GetTranslationVector();
	
	// routine to retrieve a Matrix3x3
	Matrix3 GetMatrix3();
	
	// routine to transpose a matrix
	Matrix4 Transpose();
	
	}; // Matrix4

// scalar operations
// additional scalar multiplication operator
Matrix4 operator *(float factor, const Matrix4 &matrix);

// stream input
std::istream & operator >> (std::istream &inStream, Matrix4 &value);

// stream output
std::ostream & operator << (std::ostream &outStream, const Matrix4 &value);
        
#endif
