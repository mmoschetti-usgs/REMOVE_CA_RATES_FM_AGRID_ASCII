#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "params.h"


/* Read in a reference/background agrid file and a file with updated agrid values
 * 
 */

// fortran functions
//void Parse_Text(FILE *fp, const char *querystring, const char type, void* result);
int lxyint_(float *x, float *y, float *px, float *py, int *n);
int kpscr_(float *x1, float *y1, float *x2, float *y2);
int read_polygon_files(char *fname, float lon[], float lat[], int *nval);
int read_agrid_vals(char *fname, float lon[], float lat[], float avals[], int *nval);

/*--------------------------------------------------------------------------*/
void print_parameter_file_format(void)
/*--------------------------------------------------------------------------*/
{
//  fprintf(stderr,"--h option not working yet!!\n");
  exit(1);
}


/*--------------------------------------------------------------------------*/
int main (int argc, char *argv[])
/*--------------------------------------------------------------------------*/
{
  FILE *fp;
  int cnt, nAvals=0, nPoly=0;
  float lat, lon;
  float plon[NP], plat[NP];
  float lon_arr[NV], lat_arr[NV], aval_arr[NV];
  char agrid_inF[200], agrid_outF[200], polyf[200];

/* CHECK INPUT ARGUMENTS */
  if ( (argc == 2 ) && STREQ(argv[1],"--h") ) print_parameter_file_format();
  if ( argc != 4 ) {
    fprintf(stderr,"USAGE: %s [input agrid file] [output agrid file] [polygon rate file]\n", argv[0]);
//    fprintf(stderr,"polygon-rate file formatted as: lon lat\n");
    exit(1);
  }
  sscanf(argv[1],"%s",agrid_inF);
  sscanf(argv[2],"%s",agrid_outF);
  sscanf(argv[3],"%s", polyf);

// read polygon 
  read_polygon_files(polyf,plon,plat,&nPoly);

// read agrid values
  read_agrid_vals(agrid_inF,lon_arr,lat_arr,aval_arr,&nAvals);

// loop over points in agrid files, zero rates within CA polygon
// write modified rates to file
  fp=fopen(agrid_outF,"w");
  for(cnt=0; cnt<nAvals; cnt++) {
    lon=lon_arr[cnt];
    lat=lat_arr[cnt];
    if ( lxyint_(&lon,&lat,plon,plat,&nPoly) ) {
      aval_arr[cnt]=0.0;
    }
    fprintf(fp,"%.2f %.2f %.4e\n", lon_arr[cnt], lat_arr[cnt], aval_arr[cnt]);
  }
  fclose(fp);

//
  return 0;
}
