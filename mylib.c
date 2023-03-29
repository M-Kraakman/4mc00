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


#include <stdlib.h>
#include "mylib.h"


//------------------------------------------------------------------------------
// Function to plot SVGs
//------------------------------------------------------------------------------


void plot

  ( char*           name  ,
    Model*          model )

{
	FILE *of;
	int iPar;
	int iSpr;

	int parID1;
	int parID2;

	float wall_x2 = 800;
	float wall_y2;

	of=fopen(name,"w");

	fprintf(of,"<?xml version='1.0' standalone='no'?>\n");
	fprintf(of,"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'\n");
	fprintf(of,"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n");

	fprintf(of,"<svg width='1200px' height='800px' style='background-color: white' version='1.1'\n");
	fprintf(of,"xmlns='http://www.w3.org/2000/svg'>\n");
	fprintf(of,"<g transform='translate(300,-150)'>\n");
	fprintf(of,"<g transform='scale(1.5)'>\n");

	for ( iPar = 0 ; iPar < model->nPar ; iPar++ )
	{
	fprintf(of,"<circle cx='%f' cy='%f' r='%f' fill='%s'/>\n",
	        100*model->p[iPar].r.x,
	        500-100*model->p[iPar].r.y,
	        2.0 ,
	        "#000000");
	}

	for ( iSpr = 0 ; iSpr < model->nSpr ; iSpr++)
	{
		parID1 = model->s[iSpr].p1 - 1;
		parID2 = model->s[iSpr].p2 - 1;

		fprintf(of, "<line x1='%f' y1='%f' x2='%f' y2='%f' stroke='black'/>\n",
				100*model->p[parID1].r.x,
		        500-100*model->p[parID1].r.y,
				100*model->p[parID2].r.x,
		        500-100*model->p[parID2].r.y
				);
	}
	
    int xVal;
    int amVal = 100;
    float g1, g2;

    for (xVal = 0; xVal < amVal; xVal++)
    {
        g1 = amp * cos(per * (xVal/10.)) + off;
        g2 = amp * cos(per * ((xVal+1)/10.)) + off;
       
        fprintf(of, "<line x1='%f' y1='%f' x2='%f' y2='%f' stroke='red'/>\n",
                100*(xVal/10.),
                560 - 100*g1,
                100*((xVal + 1)/10.),
                560 - 100*g2);

        fprintf(of, "<line x1='%f' y1='%f' x2='%f' y2='%f' stroke='red'/>\n",
                100*(xVal/10.),
                520 - 100*g1,
                100*((xVal + 1)/10.),
                520 - 100*g2);
    }

    fprintf(of,"</g>\n</g>\n</svg>\n");
    fclose(of);
}


//---------------------------------------------------------------------
// Function to import data
//---------------------------------------------------------------------


void readModel
  
  ( char*           name  ,
    Model*          model )
  
{
  FILE         *fp;
  float        x,y,mass;
  int          iPar = 0, iSpr=0;
  int          nPar,nPres,nSpr,parID,parID1,parID2;

  if((fp=fopen(name,"r")) == NULL) {
    printf("Cannot open file.\n");
  }
  
  fscanf(fp, "%d %d %d", &nPar , &nPres , &nSpr );
  
  model->nPar   = nPar;
  model->nSpr   = nSpr;


  for ( iPar = 0 ; iPar < model->nPar ; iPar++ )
  {    
    fscanf(fp,"%e %e %e",&x,&y,&mass);
    
	model->p[iPar].parID = iPar+1;

    model->p[iPar].r.x = x;
    model->p[iPar].r.y = y;
        
    model->p[iPar].mass = mass;
    model->p[iPar].constraint = 0;      
   
    model->p[iPar].v.x = 0.0;
    model->p[iPar].v.y = 0.0;    
    
    model->p[iPar].a.x = 0.0;
    model->p[iPar].a.y = 0.0;  
    
    model->p[iPar].f.x = 0.0;
    model->p[iPar].f.y = 0.0;
  }
  
  for ( int iPar = 0 ; iPar < nPres ; iPar++ )
  {
    fscanf(fp,"%d",&parID);
    
    model->p[parID-1].constraint = 1;
    
    model->p[parID-1].v.x = 0.01;

  }

  for ( int iSpr = 0 ; iSpr < nSpr; iSpr++ )
  {
    fscanf(fp, "%d %d", &parID1, &parID2);
    
    model->s[iSpr].ke = 100.0;
    model->s[iSpr].kp = 10.0;

    model->s[iSpr].p1 = parID1;
    model->s[iSpr].p2 = parID2;
  }

  fclose( fp );
}
    

//------------------------------------------------------------------------------
// Function to solve the algorithm
//------------------------------------------------------------------------------


void solve

  ( Model*   model)

{
  const double dt2 = DT * DT;
  
  int iPar, iSpr;

  const int nPar = model->nPar;
  const int nSpr = model->nSpr;

  for ( iSpr = 0 ; iSpr < nSpr; iSpr++ ) // Calculate old spring length
  {
    int parID1 = model->s[iSpr].p1;
    int parID2 = model->s[iSpr].p2;

    model->s[iSpr].length0 =sqrt(
                             (  (model->p[parID1 - 1].r.x - model->p[parID2 - 1].r.x)
                             *  (model->p[parID1 - 1].r.x - model->p[parID2 - 1].r.x))
                             +( (model->p[parID1 - 1].r.y - model->p[parID2 - 1].r.y)
                             *  (model->p[parID1 - 1].r.y - model->p[parID2 - 1].r.y))
                             );

  }

  for ( iPar = 0 ; iPar < nPar ; iPar++ ) // Determine particle speed and acceleration
  {
    model->p[iPar].r.x += DT * model->p[iPar].v.x + 0.5*dt2*model->p[iPar].a.x;
    model->p[iPar].r.y += DT * model->p[iPar].v.y + 0.5*dt2*model->p[iPar].a.y;
    
    model->p[iPar].v.x += 0.5*DT*model->p[iPar].a.x;
    model->p[iPar].v.y += 0.5*DT*model->p[iPar].a.y;
    
    model->p[iPar].f.x = 0.;
    model->p[iPar].f.y = 0.;
  }

  double kp = model->s[0].kp;
  double ks = model->s[0].ke;

  for ( iSpr = 0; iSpr < nSpr; iSpr++ ) // Caclulate spring deflection and spring forces
  {
	double ex = model->p[model->s[iSpr].p2 - 1].r.x - model->p[model->s[iSpr].p1 - 1].r.x;
	double ey = model->p[model->s[iSpr].p2 - 1].r.y - model->p[model->s[iSpr].p1 - 1].r.y;
    
	model->s[iSpr].length = sqrt(ex * ex + ey * ey);

	double eijx = ex / model->s[iSpr].length;
	double eijy = ey / model->s[iSpr].length;

    model->s[iSpr].uij = model->s[iSpr].length - model->s[iSpr].length0;

    model->f[iSpr].fs.x = ks * model->s[iSpr].uij * eijx;
    model->f[iSpr].fs.y = ks * model->s[iSpr].uij * eijy;

    model->p[model->s[iSpr].p1 -1].f.x += model->f[iSpr].fs.x;
    model->p[model->s[iSpr].p1 -1].f.y += model->f[iSpr].fs.y;
    model->p[model->s[iSpr].p2 -1].f.x += - model->f[iSpr].fs.x;
    model->p[model->s[iSpr].p2 -1].f.y += - model->f[iSpr].fs.y;
  }

  for ( iPar = 0 ; iPar < nPar; iPar++ ) // Calculate contact forces using Penalty method
  {
    double gTop = amp * cos(per * model->p[iPar].r.x) + off;
	double gBot = amp * cos(per * model->p[iPar].r.x) - off;
	double dEdx, d2Edx2;
	double xn, yn, xi, Dx, Dy, D;
	int try;

    if ( model->p[iPar].r.y >= gTop && model->p[iPar].r.x > 0)
    {
		xn = model->p[iPar].r.x;
		yn = model->p[iPar].r.y;
		xi = xn;
		dEdx = 1;
		try = 0;

		while(abs(dEdx) > 0.01)
				{
				dEdx = 2*(xi-xn)-2*amp*per*(amp*cos(per*xi)+off-yn)*sin(per*xi);
				d2Edx2 = 2-2*amp*per*per*(amp*cos(per*xi)+off-yn)*cos(per*xi)+2*(amp*per)*(amp*per)*sin(per*xi)*sin(per*xi);
				xi = xi - dEdx/d2Edx2;

				try++;
				if (try > 50)
					{
					xi = xn;
					dEdx = 0;
					}
				}
		Dx = (xi - model->p[iPar].r.x);
		Dy = (gTop - model->p[iPar].r.y);
        D = sqrt(Dx*Dx + Dy*Dy);

        model->f[iPar].n.x = amp*per*sin(per*xi);
        model->f[iPar].n.y = -1.;

        double norm = sqrt((model->f[iPar].n.x * model->f[iPar].n.x) + (model->f[iPar].n.y * model->f[iPar].n.y));

        model->f[iPar].fc.x = kp * D * model->f[iPar].n.x / norm;
        model->f[iPar].fc.y = kp * D * model->f[iPar].n.y / norm;

        model->p[iPar].f.x += model->f[iPar].fc.x;
        model->p[iPar].f.y += model->f[iPar].fc.y;

    }else if (model->p[iPar].r.y <= gBot && model->p[iPar].r.x > 0){		
		xn = model->p[iPar].r.x;
		yn = model->p[iPar].r.y;
		xi = xn;
		dEdx = 1;
		try = 0;

		while(abs(dEdx) > 0.01){
				dEdx = 2*(xi-xn)-2*amp*per*(amp*cos(per*xi)-off-yn)*sin(per*xi);
				d2Edx2 = 2-2*amp*per*per*(amp*cos(per*xi)-off-yn)*cos(per*xi)+2*(amp*per)*(amp*per)*sin(per*xi)*sin(per*xi);

				xi = xi - dEdx/d2Edx2;

				try++;
				if (try > 50){
					xi = xn;
					dEdx = 0;
					}
				}
		Dx = (xi - model->p[iPar].r.x);
		Dy = (gBot - model->p[iPar].r.y);
        D = sqrt(Dx*Dx + Dy*Dy);

        model->f[iPar].n.x = amp*per*sin(per*xi);
        model->f[iPar].n.y = 1.;

        double norm = sqrt((model->f[iPar].n.x * model->f[iPar].n.x) + (model->f[iPar].n.y * model->f[iPar].n.y));

        model->f[iPar].fc.x = kp * D * model->f[iPar].n.x / norm;
        model->f[iPar].fc.y = kp * D * model->f[iPar].n.y / norm;

        model->p[iPar].f.x += model->f[iPar].fc.x;
        model->p[iPar].f.y += model->f[iPar].fc.y;
		}
  }
       
  for ( iPar= 0 ; iPar < nPar ; iPar++ ) // Calculate new particle speed and acceleration
  { 
    if ( model->p[iPar].constraint == 0 )
    {
      model->p[iPar].a.x = model->p[iPar].f.x/model->p[iPar].mass;
      model->p[iPar].a.y = model->p[iPar].f.y/model->p[iPar].mass;
    }
    else
    {  
      model->p[iPar].a.x = 0.0;
      model->p[iPar].a.y = 0.0;
    }
    
    model->p[iPar].v.x += 0.5*DT*model->p[iPar].a.x;
    model->p[iPar].v.y += 0.5*DT*model->p[iPar].a.y;
	if (iPar==13){
	} 
  }
}

  
//------------------------------------------------------------------------------
// Function for storing SVGs
//------------------------------------------------------------------------------


void get_filename

  ( char*  name   ,
    int    k      )

{
  int m;
  char cc[4];
  
  strcpy(name,"output000.svg");
  
  m = sprintf(cc,"%d",k);
    
  if ( k < 10 )
  {
    name[8]=cc[0];
  }
  else  if ( k < 100 )
  { 
    name[7]=cc[0];
    name[8]=cc[1];
  }
  else
  {
    name[6]=cc[0];
    name[7]=cc[1];
    name[8]=cc[2];
  }
}
