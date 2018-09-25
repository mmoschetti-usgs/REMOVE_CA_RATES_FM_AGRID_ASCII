#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------*/
int read_agrid_vals(char *fname, float lon[], float lat[], float avals[], int *nval)
/*--------------------------------------------------------------------------*/
{
  FILE *fpAgrid;
  int cnt=0;
  char buff[300];
//  fprintf(stderr,"Reading/open polygon file - %s\n", fname);

// open polygon file
  if((fpAgrid = fopen(fname, "r"))==NULL) {
     fprintf(stderr,"Cannot open agrid file - %s\n", fname);
     return 0;
   }
   
// read header line
//   fgets(buff,300,fppoly);
// loop through coords
   while( fgets(buff,300,fpAgrid) ) {
     sscanf(buff,"%f %f %f", &lon[cnt], &lat[cnt], &avals[cnt]);
     cnt++;
   }
   *nval=cnt; 

   return 1;
}
