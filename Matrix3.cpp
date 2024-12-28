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

#include <iostream>
#include <iomanip>
#include "Matrix3.h"
#include <math.h>

// constructor - default to the zero matrix
Matrix3::Matrix3()
    { // default constructor
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            coordinates[row][col] = 0.0;
    } // default constructor

// equality operator
bool Matrix3::operator ==(const Matrix3 &other) const
    { // operator ==()
    // loop through, testing for mismatches
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            if (coordinates[row][col] != other.coordinates[row][col])
                return false;
    // if no mismatches, matrices are the same
    return true;
    } // operator ==()


// indexing - retrieves the beginning of a line
// array indexing will then retrieve an element
float * Matrix3::operator [](const int rowIndex)
    { // operator *()
    // return the corresponding row
    return coordinates[rowIndex];
    } // operator *()

// similar routine for const pointers
const float * Matrix3::operator [](const int rowIndex) const
    { // operator *()
    // return the corresponding row
    return coordinates[rowIndex];
    } // operator *()

// scalar operations
// multiplication operator (no division operator)
Matrix3 Matrix3::operator *(float factor) const
    { // operator *()
    // start with a zero matrix
    Matrix3 returnMatrix;
    // multiply by the factor
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            returnMatrix.coordinates[row][col] = coordinates[row][col] * factor;
    // and return it
    return returnMatrix;
    } // operator *()

// vector operations on Cartesian coordinates
Cartesian3 Matrix3::operator *(const Cartesian3 &vector) const
    { // cartesian multiplication
    // get a zero-initialised vector
    Cartesian3 productVector;
    
    // now loop, adding products
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            productVector[row] += coordinates[row][col] * vector[col];
    
    // return the result
    return productVector;
    } // cartesian multiplication

// matrix operations
// addition operator
Matrix3 Matrix3::operator +(const Matrix3 &other) const
    { // operator +()
    // start with a zero matrix
    Matrix3 sumMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            sumMatrix.coordinates[row][col] = coordinates[row][col] + other.coordinates[row][col];

    // return the result
    return sumMatrix;
    } // operator +()

// subtraction operator
Matrix3 Matrix3::operator -(const Matrix3 &other) const
    { // operator -()
    // start with a zero matrix
    Matrix3 differenceMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            differenceMatrix.coordinates[row][col] = coordinates[row][col] + other.coordinates[row][col];

    // return the result
    return differenceMatrix;
    } // operator -()

// multiplication operator
Matrix3 Matrix3::operator *(const Matrix3 &other) const
    { // operator *()
    // start with a zero matrix
    Matrix3 productMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            for (int entry = 0; entry < 3; entry++)
                productMatrix.coordinates[row][col] += coordinates[row][entry] * other.coordinates[entry][col];

    // return the result
    return productMatrix;
    } // operator *()

// matrix transpose
Matrix3 Matrix3::transpose() const
    { // transpose()
    // start with a zero matrix
    Matrix3 transposeMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            transposeMatrix.coordinates[row][col] = coordinates[col][row];

    // return the result
    return transposeMatrix;
    } // transpose()

// routine that returns a row vector
Cartesian3 Matrix3::row(int rowNum)
	{ // row()
	// temporary variable
	Cartesian3 returnValue;
	// loop to copy
	for (int column = 0; column < 3; column++)
		returnValue[column] = (*this)[rowNum][column];
	// and return it
	return returnValue;	
	} // row()

// and similar for a column
Cartesian3 Matrix3::column(int colNum)
	{ // column()
	// temporary variable
	Cartesian3 returnValue;
	// loop to copy
	for (int row = 0; row < 3; row++)
		returnValue[row] = (*this)[row][colNum];
	// and return it
	return returnValue;	
	} // column()

// static member functions that create specific matrices
// the zero matrix
Matrix3 Matrix3::Zero()
    { // Zero()
    // create a temporary matrix - constructor will automatically zero it
    Matrix3 returnMatrix;
	// so we just return it
	return returnMatrix;
    } // Zero()

// the identity matrix
Matrix3 Matrix3::Identity()
    { // Identity()
    // create a temporary matrix - constructor will automatically zero it
    Matrix3 returnMatrix;
    // fill in the diagonal with 1's
    for (int row = 0; row < 3; row++)
            returnMatrix.coordinates[row][row] = 1.0;

	// return it
	return returnMatrix;
	} // Identity()

Matrix3 Matrix3::RotateX(float degrees)
 	{ // RotateX()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix3 returnMatrix = Identity();

	// now set the four coefficients affected
	returnMatrix.coordinates[1][1] = cos(theta);
	returnMatrix.coordinates[1][2] = sin(theta);
	returnMatrix.coordinates[2][1] = -sin(theta);
	returnMatrix.coordinates[2][2] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateX()
 	
 Matrix3 Matrix3::RotateY(float degrees)
 	{ // RotateY()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix3 returnMatrix = Identity();

	// now set the four coefficients affected
	returnMatrix.coordinates[0][0] = cos(theta);
	returnMatrix.coordinates[0][2] = -sin(theta);
	returnMatrix.coordinates[2][0] = sin(theta);
	returnMatrix.coordinates[2][2] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateY()

 Matrix3 Matrix3::RotateZ(float degrees)
 	{ // RotateZ()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix3 returnMatrix = Identity();

	// now set the four coefficients affected
	returnMatrix.coordinates[0][0] = cos(theta);
	returnMatrix.coordinates[0][1] = sin(theta);
	returnMatrix.coordinates[1][0] = -sin(theta);
	returnMatrix.coordinates[1][1] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateZ()

// scalar operations
// additional scalar multiplication operator
Matrix3 operator *(float factor, const Matrix3 &matrix)
    { // operator *()
    // since this is commutative, call the other version
    return matrix * factor;
    } // operator *()

// routine to transpose a matrix
Matrix3 Matrix3::Transpose()
	{ // Transpose()
	// the copy to return
	Matrix3 transposed;
	
	// loop to copy
	for (int row = 0; row < 3; row++)
		for (int col = 0; col < 3; col++)
			transposed[col][row] = coordinates[row][col];
	
	// and return it
	return transposed;
	} // Transpose()

// stream input
std::istream & operator >> (std::istream &inStream, Matrix3 &matrix)
    { // operator >>()
    // just loop, reading them in
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            inStream >> matrix.coordinates[row][col];   
    // and return the stream
    return inStream;
    } // operator >>()

// stream output
std::ostream & operator << (std::ostream &outStream, const Matrix3 &matrix)
    { // operator <<()
    // just loop, reading them in
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            outStream << std::setw(12) << std::setprecision(5) << std::fixed << matrix.coordinates[row][col] << ((col == 2) ? "\n" : " "); 
    // and return the stream
    return outStream;
    } // operator <<()

Matrix3 Matrix3::Inverse()
	{ // Inverse()
	// create an inverse matrix to fill in
	Matrix3 coMatrix;

	// fill in the individual entries with cofactors
	coMatrix[0][0] = coordinates[1][1] * coordinates[2][2] - coordinates[1][2] * coordinates[2][1];
	coMatrix[0][1] = coordinates[1][2] * coordinates[2][0] - coordinates[1][0] * coordinates[2][2];
	coMatrix[0][2] = coordinates[1][0] * coordinates[2][1] - coordinates[1][1] * coordinates[2][0];

	coMatrix[1][0] = coordinates[2][1] * coordinates[1][0] - coordinates[2][0] * coordinates[1][1];
	coMatrix[1][1] = coordinates[2][2] * coordinates[0][0] - coordinates[2][0] * coordinates[0][2];
	coMatrix[1][2] = coordinates[2][0] * coordinates[0][1] - coordinates[2][1] * coordinates[0][0];

	coMatrix[2][0] = coordinates[0][1] * coordinates[1][2] - coordinates[0][2] * coordinates[1][1];
	coMatrix[2][1] = coordinates[0][2] * coordinates[1][0] - coordinates[0][0] * coordinates[1][2];
	coMatrix[2][2] = coordinates[0][0] * coordinates[1][1] - coordinates[0][1] * coordinates[1][0];

	// we can also use these entries to compute the determinant, which is just a row or column-wise sum of the signed cofactors
	float det = coordinates[0][0] * coMatrix[0][0] + coordinates[0][1] * coMatrix[0][1] + coordinates[0][2] * coMatrix[0][2];
	
	// if the determinant is zero, return a zero matrix
	if (det == 0)
		return Zero();
	// otherwise transpose the comatrix and divide by the determinant
	else
		return (1.0 / det) * coMatrix.Transpose();		
	} // Inverse()

