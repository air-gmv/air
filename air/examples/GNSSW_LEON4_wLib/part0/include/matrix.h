//*********************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  SIRIA
// File:                  matrix.h
// Version:                  1.0
// Date (YY/MM/DD):            2005 / 08 / 02
// Component:
// Purpose:   matrix API interface.
// Language:               C.
//
// History:
//   1.0   -   2009/04/20   -   JJNC   -   R1.0_FUN_LINUX
//*********************************************************************************
#ifndef CMATRIX_H
#define CMATRIX_H

#include <string.h>
#include "Defs_nav.h"



//*********************************************************************************
//                        DEFINITIONS
//*********************************************************************************
/** Error code returned if an input or output variable is a NULL pointer */
#define GL_MATRIX_NULL_POINTER_T  (1)

/** Error code returned if input dimensions of matrices are incorrect */
#define GL_MATRIX_BAD_DIM_T       (2)

/** Error code returned if a variable of type GL_SymType_t
 * has an invalid value */
#define GL_MATRIX_BAD_SYM_TYPE_T  (3)

/** Error code returned if the zero vector is present when not expected */
#define GL_MATRIX_ZERO_VEC_T      (4)

/** Error code returned if a matrix is not positive definite when expected */
#define GL_MATRIX_NOT_POS_DEF_T   (5)

/** Error code returned if a not symmetrical matrix
 *  is converted into symmetrical format */
#define GL_MATRIX_NOT_SYM_T       (6)

/** Error code returned if a not symmetrical matrix
 *  is converted into symmetrical format */
#define GL_MATRIX_NEG_TOL_T       (7)

/** Error code returned if a GL_SideMul_t input is neither GL_SIDE_RIGHT_E nor
 *  GL_SIDE_LEFT_E */
#define GL_MATRIX_NOT_SIDE_MULT_T (8)

/** Error code returned if a matrix is not regular when expected */
#define GL_MATRIX_NOT_INV_T       (9)

/** Error code returned if maximum iterations are
 *  reach in iterative method */
#define GL_MATRIX_NUM_ITER_EXCEED_T (10)



//*********************************************************************************
//                     STRUCTURES DEFINITION
//*********************************************************************************
typedef double Matrix[MAXNUMCHAN][MAXNUMCHAN];  /* change it for double matrixes */
typedef double Matrix4x4[4][4];  /* change it for double matrixes */
typedef double Vector[MAXNUMCHAN];

//*********************************************************************************
//                        FUNCTIONS INTERFACE
//*********************************************************************************
void transpose(Matrix A, Matrix AT, int dim1, int dim2);
enum matrixResult { matrixOK, matrixOutOfMem, matrixSingular };
enum matrixResult matrixInv( Matrix a, Matrix inv, const int n );
void matrixMul( Matrix a, Matrix b, Matrix c, int l, int m, int n );
void matrixVecMul( Matrix a, Vector b, Vector c, int m, int n );
double VectNorm(Vector a, int m);
void svdcmp(float **a, int m, int n, float w[], float **v);
void RemoveFromList(int *N, int list[MAXNUMSAT],int element);


#endif
