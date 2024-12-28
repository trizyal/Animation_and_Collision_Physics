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
//  Note: the emphasis here is on clarity, not efficiency
//  A number of the routines could be implemented more
//  efficiently but aren't
//  
///////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include "Matrix4.h"
#include <math.h>

// constructor - default to the zero matrix
Matrix4::Matrix4()
    { // default constructor
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            coordinates[row][col] = 0.0;
    } // default constructor

// constructor from a Matrix3
Matrix4::Matrix4(Matrix3 &other)
	{ // constructor from Matrix3
	// zero it out first
	for (int row = 0; row < 4; row++)
		for (int col = 0; col < 4; col++)
			coordinates[row][col] = 0.0;
	// now copy entries in
	for (int row = 0; row < 3; row++)
		for (int col = 0; col < 3; col++)
			coordinates[row][col] = other[row][col];
	// and set the homogeneous entry to 1	
	coordinates[3][3] = 1.0;
	} // constructor from Matrix3

// equality operator
bool Matrix4::operator ==(const Matrix4 &other) const
    { // operator ==()
    // loop through, testing for mismatches
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            if (coordinates[row][col] != other.coordinates[row][col])
                return false;
    // if no mismatches, matrices are the same
    return true;
    } // operator ==()


// indexing - retrieves the beginning of a line
// array indexing will then retrieve an element
float * Matrix4::operator [](const int rowIndex)
    { // operator *()
    // return the corresponding row
    return coordinates[rowIndex];
    } // operator *()

// similar routine for const pointers
const float * Matrix4::operator [](const int rowIndex) const
    { // operator *()
    // return the corresponding row
    return coordinates[rowIndex];
    } // operator *()

// scalar operations
// multiplication operator (no division operator)
Matrix4 Matrix4::operator *(float factor) const
    { // operator *()
    // start with a zero matrix
    Matrix4 returnMatrix;
    // multiply by the factor
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            returnMatrix.coordinates[row][col] = coordinates[row][col] * factor;
    // and return it
    return returnMatrix;
    } // operator *()

// vector operations on homogeneous coordinates
// multiplication is the only operator we use
Homogeneous4 Matrix4::operator *(const Homogeneous4 &vector) const
    { // operator *()
    // get a zero-initialised vector
    Homogeneous4 productVector;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            productVector[row] += coordinates[row][col] * vector[col];
    
    // return the result
    return productVector;
    } // operator *()

// and on Cartesian coordinates
Cartesian3 Matrix4::operator *(const Cartesian3 &vector) const
    { // cartesian multiplication
    // convert to Homogeneous coords and multiply
    Homogeneous4 productVector = (*this) * Homogeneous4(vector);

    // then divide back through
    return productVector.Point();
    } // cartesian multiplication

// matrix operations
// addition operator
Matrix4 Matrix4::operator +(const Matrix4 &other) const
    { // operator +()
    // start with a zero matrix
    Matrix4 sumMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            sumMatrix.coordinates[row][col] = coordinates[row][col] + other.coordinates[row][col];

    // return the result
    return sumMatrix;
    } // operator +()

// subtraction operator
Matrix4 Matrix4::operator -(const Matrix4 &other) const
    { // operator -()
    // start with a zero matrix
    Matrix4 differenceMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            differenceMatrix.coordinates[row][col] = coordinates[row][col] + other.coordinates[row][col];

    // return the result
    return differenceMatrix;
    } // operator -()

// multiplication operator
Matrix4 Matrix4::operator *(const Matrix4 &other) const
    { // operator *()
    // start with a zero matrix
    Matrix4 productMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            for (int entry = 0; entry < 4; entry++)
                productMatrix.coordinates[row][col] += coordinates[row][entry] * other.coordinates[entry][col];

    // return the result
    return productMatrix;
    } // operator *()

// matrix transpose
Matrix4 Matrix4::transpose() const
    { // transpose()
    // start with a zero matrix
    Matrix4 transposeMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            transposeMatrix.coordinates[row][col] = coordinates[col][row];

    // return the result
    return transposeMatrix;
    } // transpose()

// returns a column-major array of 16 values
// for use with OpenGL
columnMajorMatrix Matrix4::columnMajor() const
    { // columnMajor()
    // start off with an unitialised array
    columnMajorMatrix returnArray;
    // loop to fill in
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            returnArray.coordinates[4 * col + row] = coordinates[row][col];
    // now return the array
    return returnArray;
    } // columnMajor()

// routine that returns a row vector as a Homogeneous4
Homogeneous4 Matrix4::row(int rowNum)
	{ // row()
	// temporary variable
	Homogeneous4 returnValue;
	// loop to copy
	for (int column = 0; column < 4; column++)
		returnValue[column] = (*this)[rowNum][column];
	// and return it
	return returnValue;	
	} // row()

// and similar for a column
Homogeneous4 Matrix4::column(int colNum)
	{ // column()
	// temporary variable
	Homogeneous4 returnValue;
	// loop to copy
	for (int row = 0; row < 4; row++)
		returnValue[row] = (*this)[row][colNum];
	// and return it
	return returnValue;	
	} // column()

// static member functions that create specific matrices
// the zero matrix
Matrix4 Matrix4::Zero()
    { // Zero()
    // create a temporary matrix - constructor will automatically zero it
    Matrix4 returnMatrix;
	// so we just return it
	return returnMatrix;
    } // Zero()

// the identity matrix
Matrix4 Matrix4::Identity()
    { // Identity()
    // create a temporary matrix - constructor will automatically zero it
    Matrix4 returnMatrix;
    // fill in the diagonal with 1's
    for (int row = 0; row < 4; row++)
            returnMatrix.coordinates[row][row] = 1.0;

	// return it
	return returnMatrix;
	} // Identity()

Matrix4 Matrix4::Translate(const Cartesian3 &vector)
    { // Translation()
    // create a temporary matrix  and set to identity
    Matrix4 returnMatrix = Identity();

    // put the translation in the w column
    for (int entry = 0; entry < 3; entry++)
        returnMatrix.coordinates[entry][3] = vector[entry];

    // return it
    return returnMatrix;
    } // Translation()

 Matrix4 Matrix4::RotateX(float degrees)
 	{ // RotateX()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix4 returnMatrix = Identity();

	// now set the four coefficients affected
	returnMatrix.coordinates[1][1] = cos(theta);
	returnMatrix.coordinates[1][2] = sin(theta);
	returnMatrix.coordinates[2][1] = -sin(theta);
	returnMatrix.coordinates[2][2] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateX()
 	
 Matrix4 Matrix4::RotateY(float degrees)
 	{ // RotateY()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix4 returnMatrix = Identity();

	// now set the four coefficients affected
	returnMatrix.coordinates[0][0] = cos(theta);
	returnMatrix.coordinates[0][2] = -sin(theta);
	returnMatrix.coordinates[2][0] = sin(theta);
	returnMatrix.coordinates[2][2] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateY()

 Matrix4 Matrix4::RotateZ(float degrees)
 	{ // RotateZ()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix4 returnMatrix = Identity();

	// now set the four coefficients affected
	returnMatrix.coordinates[0][0] = cos(theta);
	returnMatrix.coordinates[0][1] = sin(theta);
	returnMatrix.coordinates[1][0] = -sin(theta);
	returnMatrix.coordinates[1][1] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateZ()

Matrix4 Matrix4::GetRotation(const Cartesian3& vector1, const Cartesian3& vector2)
{
    Cartesian3 c = vector1.cross(vector2).unit();
    float cos = vector1.unit().dot(vector2.unit());
    float sin = sqrt(1 - pow(cos, 2));
    Matrix4 rot = Matrix4::Identity();
    rot.coordinates[0][0] = cos + (1 - cos) * pow(c.x, 2);
    rot.coordinates[0][1] = (1 - cos) * c.x * c.y - sin * c.z;
    rot.coordinates[0][2] = (1 - cos) * c.x * c.z + sin * c.y;
    rot.coordinates[1][0] = (1 - cos) * c.y * c.x + sin * c.z;
    rot.coordinates[1][1] = cos + (1 - cos) * pow(c.y, 2);
    rot.coordinates[1][2] = (1 - cos) * c.y * c.z - sin * c.x;
    rot.coordinates[2][0] = (1 - cos) * c.z * c.x - sin * c.y;
    rot.coordinates[2][1] = (1 - cos) * c.z * c.y + sin * c.x;
    rot.coordinates[2][2] = cos + (1 - cos) * pow(c.z, 2);
    return rot;
}

// routines to retrieve the rotation and translation component
// NOTE:  NO ERROR CHECKING: assumes only pure rotation plus translation
Matrix4 Matrix4::GetRotationMatrix()
	{ // GetRotationMatrix()
	// start with a duplicate copy
	Matrix4 returnMatrix = *this;

	// and set the final row and column's entries to 0 (except [3][3]
	returnMatrix.coordinates[0][3] = 0.0;
	returnMatrix.coordinates[1][3] = 0.0;
	returnMatrix.coordinates[2][3] = 0.0;
	returnMatrix.coordinates[3][0] = 0.0;
	returnMatrix.coordinates[3][1] = 0.0;
	returnMatrix.coordinates[3][2] = 0.0;

	// and return it
	return returnMatrix;
	} // GetRotationMatrix()
	
Cartesian3 Matrix4::GetTranslationVector()
	{ // GetTranslationVector()
	// exploit existing routines - it's just column 3 turned into a vector
	return column(3).Vector();
	} // GetTranslationVector()

// routine to retrieve a Matrix3x3
Matrix3 Matrix4::GetMatrix3()
	{ // GetMatrix3()
	Matrix3 returnMatrix;
	for (int row = 0; row < 3; row++)
		for (int col = 0; col < 3; col++)
			returnMatrix[row][col] = coordinates[row][col];
			
	return returnMatrix;
	} // GetMatrix3()
	 
// scalar operations
// additional scalar multiplication operator
Matrix4 operator *(float factor, const Matrix4 &matrix)
    { // operator *()
    // since this is commutative, call the other version
    return matrix * factor;
    } // operator *()

// routine to transpose a matrix
Matrix4 Matrix4::Transpose()
	{ // Transpose()
	// the copy to return
	Matrix4 transposed;
	
	// loop to copy
	for (int row = 0; row < 4; row++)
		for (int col = 0; col < 4; col++)
			transposed[col][row] = coordinates[row][col];
	
	// and return it
	return transposed;
	} // Transpose()

// stream input
std::istream & operator >> (std::istream &inStream, Matrix4 &matrix)
    { // operator >>()
    // just loop, reading them in
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            inStream >> matrix.coordinates[row][col];   
    // and return the stream
    return inStream;
    } // operator >>()

// stream output
std::ostream & operator << (std::ostream &outStream, const Matrix4 &matrix)
    { // operator <<()
    // just loop, reading them in
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            outStream << std::setw(12) << std::setprecision(5) << std::fixed << matrix.coordinates[row][col] << ((col == 3) ? "\n" : " "); 
    // and return the stream
    return outStream;
    } // operator <<()
