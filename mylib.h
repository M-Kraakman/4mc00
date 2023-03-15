/*******************************************************************************
* Catheter:
*
* Multiple particle dynamics simulation
* Structures are represented by particles with mass
* which are connected by elastoplastic springs with a 
* stiffness.
*
* Version | Description                               | Author | Date
* 0.1     | Initial version, read and write particles | JR     | 100122
*         |                                           |        |
*
*******************************************************************************/

#ifndef MYLIB_H
#define MYLIB_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MAX_POINTS  1000
#define MAX_SPRINGS 1000
#define DT          0.001
#define PI_CONST    3.1415


//------------------------------------------------------------------------------
// Definition of new type Vec2
//------------------------------------------------------------------------------


typedef struct
{
	double   x;
	double   y;
} Vec2;


//------------------------------------------------------------------------------
// Definition of new type Particle
//------------------------------------------------------------------------------


typedef struct
{
	int		parID;

	Vec2     r;
	Vec2     v;
	Vec2     a;

	Vec2     f;

	double   mass;

	int      constraint;
} Particle;


//------------------------------------------------------------------------------
// Definition of new type Spring
//------------------------------------------------------------------------------


typedef struct
{
  double    kp;
  double    ke;

  int       p1;
  int       p2;  
} Spring;


//------------------------------------------------------------------------------
// Definition of new type Model that contains
//   an array of particles and some counter.
// 
//   an array of spring should be added here!
//------------------------------------------------------------------------------

typedef struct
{
  Particle p[MAX_POINTS];
  Spring   s[MAX_SPRINGS];
  int      nPrs;
  int      nPar;
  int      nSpr;
  int      ctype;
} Model;


//------------------------------------------------------------------------------
//  Function to plt SVG
//------------------------------------------------------------------------------


void plot

  ( char*            name  ,
    Model*           m     );

//------------------------------------------------------------------------------
//  Function to read the geometry
//------------------------------------------------------------------------------


void readModel

  ( char*           name   ,
    Model*          m      );


//------------------------------------------------------------------------------
//  Function to solve 1 time step using Verlets integration scheme
//------------------------------------------------------------------------------


void solve

  ( Model*           m    );


//------------------------------------------------------------------------------
//  Creates a filename (name) for the SVG file in the format outputXXX.svg
//  where k is the file number.
//------------------------------------------------------------------------------

  
void get_filename

  ( char*      name ,
    int        k    );

#endif
