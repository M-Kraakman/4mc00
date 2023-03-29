/*******************************************************************************
* Catheter:
*
* Multiple particle dynamics simulation
* Structures are represented by particles with mass
* which are connected by elastoplastic springs with a 
* stiffness.
*
* Authors: Mattias Hart and Martijn Kraakman
* Version 3: Both top and bottom sinusoidal wall. Currently not working.
* Date: 29/3/2023
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

#define slope	1.0
#define offset	0.0

#define amp		0.5
#define	per		1.2
#define off		0.2


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
  double    length0;
  double    length;
  double    uij;

  int       p1;
  int       p2;  
} Spring;

//------------------------------------------------------------------------------
// Definition of new type Force for intermediate calculations of the forces
//------------------------------------------------------------------------------

typedef struct
{
  Vec2      n;
  Vec2      fc;
  Vec2      fs;
} Force;


//------------------------------------------------------------------------------
// Definition of new type Model that contains
//   an array of particles and some counter.
//------------------------------------------------------------------------------

typedef struct
{
  Particle p[MAX_POINTS];
  Spring   s[MAX_SPRINGS];
  Force    f[MAX_POINTS];
  int      nPrs;
  int      nPar;
  int      nSpr;
  int      ctype;
} Model;


//------------------------------------------------------------------------------
//  Function to plot SVGs
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
