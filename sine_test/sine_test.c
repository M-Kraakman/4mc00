#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

void plot();

int main(void){
	plot();
}

void plot(){
	float amp = 10;
	float spc = 1;
	float frq = 0.1;
	float phs = 0;

	int i = 0;
	double x1, y1, x2, y2;
	
	FILE *of;
	of=fopen(name,"w");

	/*fprintf(of,"<?xml version='1.0' standalone='no'?>\n");
	fprintf(of,"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'\n");
	fprintf(of,"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n");

	fprintf(of,"<svg width='1200px' height='800px' style='background-color: white' version='1.1'\n");
	fprintf(of,"xmlns='http://www.w3.org/2000/svg'>\n");
	fprintf(of,"<g transform='translate(300,0)'>\n");
	fprintf(of,"<g transform='scale(1.5)'>\n");

	/*for(i = 0; i<1000, i++){
		x1 = (i - 1) * spc;
		y1 = sin(frq * (i - 1 + phs) * amp);

		x2 = i * spc;
		y2 = sin(frq * (i + phs) * amp);

		fprintf(of, "<line x1='%f' y1='%f' x2='%f' y2='%f' stroke='black'/>\n", x1, y1, x2, y2);
	}*/

	//fprintf(of,"</g>\n</g>\n</svg>\n");
	fclose(of);
	//printf("yo\n");
}
