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


#include <stdlib.h>
#include "mylib.h"


//------------------------------------------------------------------------------
//
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
	// printf("%.1f %.1f\n", 100*model->p[iPar].r.x, 500-100*model->p[iPar].r.y);
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
	
    // g(x) = pcos(lx)+h --> g(x) = 0.5cos(1.2x)+0.2
    int xVal;
    int amVal = 100;
    float g1, g2;

    for (xVal = 0; xVal < amVal; xVal++)
    {
        g1 = 0.5 * cos(1.2 * (xVal/10.)) + 0.2;
        g2 = 0.5 * cos(1.2 * ((xVal+1)/10.)) + 0.2;
       
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
        //printf("%f %f %f %f\n", 100.*xVal, 100*g1, 100.*(xVal+1), 100*g2);
    }

	wall_y2 = slope * wall_x2 + offset;
	//printf("800 - wall_y2 = %.f * %.f + %.f = %.f\n", slope, wall_x2, offset, 800 - wall_y2);
	//printf("drawing line from (%.1f, %.1f) to (%.1f, %.1f)\n", 0., 0., wall_x2, 800-wall_y2);  
	fprintf(of, "<line x1='%f' y1='%f' x2='%f' y2='%f' stroke='black'/>\n", 0. - 300, 800., wall_x2 - 300, 800-wall_y2);
    fprintf(of,"</g>\n</g>\n</svg>\n");
    fclose(of);
}


//---------------------------------------------------------------------
//
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
  
  fscanf(fp, "%d %d %d", &nPar , &nPres , &nSpr ); /* read from file */
  
  model->nPar   = nPar;
  model->nSpr   = nSpr;


  //printf("List of Particles\nID: rx ry m c vx vy ax ay fx fy\n");
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

	//printf("%d: %.1f %.1f %.1f %d %.1f %.1f %.1f %.1f %.1f %.1f\n", model->p[iPar].parID, model->p[iPar].r.x, model->p[iPar].r.y,
			//model->p[iPar].mass, model->p[iPar].constraint, model->p[iPar].v.x, model->p[iPar].v.y, model->p[iPar].a.x, model->p[iPar].a.y,
			//model->p[iPar].f.x, model->p[iPar].f.y);  
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
    
     /*printf("Spring %d: %.1f %.1f %d %d %.2f \n",
            
            iSpr+1,
            model->s[iSpr].ke,
            model->s[iSpr].kp,
            model->s[iSpr].p1,
            model->s[iSpr].p2,
            model->s[iSpr].length0);*/
  }

  fclose( fp );
}
    

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------


void solve

  ( Model*   model)

{
  const double dt2 = DT * DT;
  
  int iPar, iSpr;

  const int nPar = model->nPar;
  const int nSpr = model->nSpr;

  for ( iSpr = 0 ; iSpr < nSpr; iSpr++ )
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

  for ( iPar = 0 ; iPar < nPar ; iPar++ )
  {
    model->p[iPar].r.x += DT * model->p[iPar].v.x + 0.5*dt2*model->p[iPar].a.x;
    model->p[iPar].r.y += DT * model->p[iPar].v.y + 0.5*dt2*model->p[iPar].a.y;
    
    model->p[iPar].v.x += 0.5*DT*model->p[iPar].a.x;
    model->p[iPar].v.y += 0.5*DT*model->p[iPar].a.y;
    
    model->p[iPar].f.x = 0.;
    model->p[iPar].f.y = 0.;

    /*printf("%d %.2f %.2f \n", iPar+1, model->p[iPar].r.x, model->p[iPar].r.y)*/;

  }

  double a = slope, b = offset;
  double kp = model->s[0].kp;
  double ks = model->s[0].ke;

  for ( iSpr = 0; iSpr < nSpr; iSpr++ )
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

    /*printf("%.2f %.2f %.2f %.2f %.2f %.2f\n",

            model->s[iSpr].length,
			eijx,
			eijy,
            model->s[iSpr].uij,
            model->f[iSpr].fs.x,
            model->f[iSpr].fs.y);*/
  }
  
  double p = cosAMP;
  double l = cosPER;
  double h = cosOFF;

  for ( iPar = 0 ; iPar < nPar; iPar++ )
  {
    model->f[iPar].gFuncTop = p * cos(l * model->p[iPar].r.x) + h;
    model->f[iPar].gFuncBttm = p * cos(l * model->p[iPar].r.x) - h;

    if ( model->p[iPar].r.y <= model->f[iPar].gFuncBttm )
    {
        double xi = model->p[iPar].r.x;
        double yi = model->p[iPar].r.y;
        
        double iNR;

        for (iNR = 1; iNR <= 5; iNR++)
        {
        double xn = model->p[iPar].r.x;

        double dEdx = 2 * (xn - xi) - 2 * p * l * (p * cos(l * xn) - h - yi) * sin(l * xn);
        double ddEdxx = 2 - 2 * p * l * l * (p * cos(l * xn) - h - yi) * cos(l * xn) + 2 * (p*l) * (p * l) * sin(l * xn) * sin(l * xn);
        
        xn += - (dEdx / ddEdxx);

        model->f[iPar].xFunc = xn;
        }
        
        model->f[iPar].D = sqrt(
                                  ((model->f[iPar].xFunc - model->p[iPar].r.x) * (model->f[iPar].xFunc - model->p[iPar].r.x))
                                + ((model->f[iPar].gFuncBttm - model->p[iPar].r.y) * (model->f[iPar].gFuncBttm - model->p[iPar].r.y))
                                );

        model->f[iPar].n.x = p * l * sin(l * model->f[iPar].xFunc);
        model->f[iPar].n.y = 1.;

        model->f[iPar].norm = sqrt((model->f[iPar].n.x * model->f[iPar].n.x) + (model->f[iPar].n.y * model->f[iPar].n.y));

        model->f[iPar].fc.x = kp * model->f[iPar].D * model->f[iPar].n.x / model->f[iPar].norm;
        model->f[iPar].fc.y = kp * model->f[iPar].D * model->f[iPar].n.y / model->f[iPar].norm;

        model->p[iPar].f.x += model->f[iPar].fc.x;
        model->p[iPar].f.y += model->f[iPar].fc.y;

        //printf("%.2f %.2f %.2f %.2f \n", xi, yi, model->f[iPar].gFuncBttm, model->f[iPar].xFunc);
    }
    else if ( model->p[iPar].r.y >= model->f[iPar].gFuncTop )
    {
        double xi = model->p[iPar].r.x;
        double yi = model->p[iPar].r.y;
        
        double iNR;

        for (iNR = 1; iNR <= 5; iNR++)
        {
        double xn = model->p[iPar].r.x;

        double dEdx = 2 * (xn - xi) - 2 * p * l * (p * cos(l * xn) + h - yi) * sin(l * xn);
        double ddEdxx = 2 - 2 * p * l * l * (p * cos(l * xn) + h - yi) * cos(l * xn) + 2 * (p*l) * (p * l) * sin(l * xn) * sin(l * xn);
        
        xn += - (dEdx / ddEdxx);

        model->f[iPar].xFunc = xn;
        }
        
        model->f[iPar].D = sqrt(
                                  ((model->f[iPar].xFunc - model->p[iPar].r.x) * (model->f[iPar].xFunc - model->p[iPar].r.x))
                                + ((model->f[iPar].gFuncBttm - model->p[iPar].r.y) * (model->f[iPar].gFuncBttm - model->p[iPar].r.y))
                                );

        model->f[iPar].n.x = p * l * sin(l * model->f[iPar].xFunc);
        model->f[iPar].n.y = 1.;

        model->f[iPar].norm = sqrt((model->f[iPar].n.x * model->f[iPar].n.x) + (model->f[iPar].n.y * model->f[iPar].n.y));

        model->f[iPar].fc.x = kp * model->f[iPar].D * model->f[iPar].n.x / model->f[iPar].norm;
        model->f[iPar].fc.y = kp * model->f[iPar].D * model->f[iPar].n.y / model->f[iPar].norm;

        model->p[iPar].f.x += model->f[iPar].fc.x;
        model->p[iPar].f.y += model->f[iPar].fc.y;
    }

    /*printf("%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f \n",

            model->f[iPar].gFunc,
            model->f[iPar].xFunc,
            model->f[iPar].D,
            model->f[iPar].n.x,
            model->f[iPar].n.y,
            model->f[iPar].norm,
            model->f[iPar].fc.x,
            model->f[iPar].fc.y)*/;
  }
       
  for ( iPar= 0 ; iPar < nPar ; iPar++ )
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

    /*printf("%2f %2f\n", model->p[iPar].f.x, model->p[iPar].f.y);*/   
  }
}

  
//------------------------------------------------------------------------------
//
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
