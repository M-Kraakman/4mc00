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

#include "mylib.h"

int main( int argc , char *argv[])

{  
  int          iCyc  = 0;            // Cycle counter
  int          iPlot = 0;            // Plot counter
  char         svgfile[20];          // File name for output
  
  Model        model;                // List with particles and springs
     
  readModel( "geom01.dat" , &model );
 
  while( iCyc < 1) //300000 )
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

