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

int main( int argc , char *argv[]){  
	int          iCyc  = 0;            // Cycle counter
	int          iPlot = 0;            // Plot counter
	char         svgfile[20];          // File name for output

	double		Ek_total, Es_total;

	Model        model;                // List with particles and springs
	readModel("geom01.dat" , &model);

	FILE * fpt;
	fpt = fopen("Ekin.csv", "w+");
	fprintf(fpt, "Step, Ekin\n");

	while(iCyc < 30){
    	iCyc++;
    	Ek_total, Es_total = solve( &model );
      
    	if(iCyc%10 == 0){
	  		// Save kinetic energy
			fprintf(fpt, "%d, %lf\n", iCyc, (Ek_total+Es_total)*1000000000);

			//Plot
      		iPlot++;
			get_filename( svgfile , iPlot );
			plot( svgfile , &model );
    	}
  	}
	fclose(fpt);
	printf("Simulation terminated successfully\n");
	return 0;
}

