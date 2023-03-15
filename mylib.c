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
 
  of=fopen(name,"w");

  fprintf(of,"<?xml version='1.0' standalone='no'?>\n");
  fprintf(of,"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'\n");
  fprintf(of,"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n");
  
  fprintf(of,"<svg width='1200px' height='800px' style='background-color: white' version='1.1'\n");
  fprintf(of,"xmlns='http://www.w3.org/2000/svg'>\n");
  fprintf(of,"<g transform='translate(300,0)'>\n");
  fprintf(of,"<g transform='scale(1.5)'>\n");
  
  for ( iPar = 0 ; iPar < model->nPar ; iPar++ )
  {
    fprintf(of,"<circle cx='%f' cy='%f' r='%f' fill='%s'/>\n",
            100*model->p[iPar].r.x,
            500-100*model->p[iPar].r.y,
            2.0 ,
            "#000000");
  }
  
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
  
  for ( iPar = 0 ; iPar < model->nPar ; iPar++ )
  {    
    fscanf(fp,"%e %e %e",&x,&y,&mass);
    
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

  for ( int iSpr = 0 ; iSpr < nSpr ; iSpr++ )
  {
    fscanf(fp, "%d %d", &parID1, &parID2);
    
    model->s[iSpr].ke = 100.0;
    model->s[iSpr].kp = 10.0;

    model->s[iSpr].p1 = parID1;
    model->s[iSpr].p2 = parID2;
    
    //printf("Spring %d: %.1f %.1f %d %d \n", iSpr, model->s[iSpr].ke, model->s[iSpr].kp, model->s[iSpr].p1, model->s[iSpr].p2);
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
  
  int iPar;

  const int nPar = model->nPar;

  for ( iPar = 0 ; iPar < nPar ; iPar++ )
  {
    model->p[iPar].r.x += DT * model->p[iPar].v.x + 0.5*dt2*model->p[iPar].a.x;
    model->p[iPar].r.y += DT * model->p[iPar].v.y + 0.5*dt2*model->p[iPar].a.y;
    
    model->p[iPar].v.x += 0.5*DT*model->p[iPar].a.x;
    model->p[iPar].v.y += 0.5*DT*model->p[iPar].a.y;
    
    model->p[iPar].f.x = 0.;
    model->p[iPar].f.y = 0.;

  }

  double a = 1., b = 0.;
  double kp = model->s[0].kp;

  for ( iPar = 0 ; iPar < nPar; iPar++ )
  {
    model->f[iPar].gFunc = a * model->p[iPar].r.x + b;
    model->f[iPar].xFunc = (model->p[iPar].r.x + a*model->p[iPar].r.y - a*b) / (1 + a * a);
    model->f[iPar].D = sqrt(
                              (model->f[iPar].xFunc - model->p[iPar].r.x) * (model->f[iPar].xFunc - model->p[iPar].r.x)
                            + (model->f[iPar].gFunc - model->p[iPar].r.y) * (model->f[iPar].gFunc - model->p[iPar].r.y)
                            );

    model->f[iPar].n.x = -a;
    model->f[iPar].n.y = 1.;

    model->f[iPar].fc.x = kp * model->f[iPar].D * model->f[iPar].n.x / abs(model->f[iPar].n.x * model-> f[iPar].n.y);
    model->f[iPar].fc.y = kp * model->f[iPar].D * model->f[iPar].n.y / abs(model->f[iPar].n.x * model-> f[iPar].n.y);

    printf("%.1f and %.1f \n", model->f[iPar].fc.x, model->f[iPar].fc.y);
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
