#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------*/
int read_polygon_files(char *fname, float lon[], float lat[], int *nval)
/*--------------------------------------------------------------------------*/
{
  FILE *fppoly;
  int cnt=0;
  char buff[300];
//  fprintf(stderr,"Reading/open polygon file - %s\n", fname);

// open polygon file
  if((fppoly = fopen(fname, "r"))==NULL) {
     fprintf(stderr,"Cannot open polygon file - %s\n", fname);
     return 0;
   }
   
// read header line
//   fgets(buff,300,fppoly);
// loop through coords
   while( fgets(buff,300,fppoly) ) {
     sscanf(buff,"%f %f", &lon[cnt], &lat[cnt]);
     cnt++;
   }
   *nval=cnt; 

   return 1;
}
