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

#include "mylib.h"

int main( int argc , char *argv[])

{  
  int          iCyc  = 0;            // Cycle counter
  int          iPlot = 0;            // Plot counter
  char         svgfile[20];          // File name for output
  
  Model        model;                // List with particles and springs
     
  readModel( "geom01.dat" , &model );

  while( iCyc < 300000 )
  {
    iCyc++;
         
    solve( &model );
      
    if ( iCyc%1000 == 0 )
    {
      iPlot++;

      get_filename( svgfile , iPlot );
      
      plot( svgfile , &model );
    }
  }
  
  printf("Simulation terminated successfully\n");

  return 0;
}

