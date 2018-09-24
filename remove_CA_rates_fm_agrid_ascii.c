#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "params.h"


/* Read in a reference/background agrid file and a file with updated agrid values
 * 
 */

// fortran functions
//void read_header(FILE *fp, int nlines);
void read_parameter_file(char *paramf, struct src_params* pars);
void write_results_to_file(char *bsnm, float a_wt[], float lon[], float lat[], int ncnt);
void Parse_Text(FILE *fp, const char *querystring, const char type, void* result);
int lxyint_(float *x, float *y, float *px, float *py, int *n);
int kpscr_(float *x1, float *y1, float *x2, float *y2);
int read_polygon_files(char *fname, float lon[], float lat[], int *nval);


/*--------------------------------------------------------------------------*/
void write_results_to_file(char *bsnm, float a_wt[], float lon[], float lat[], int ntot)
/*--------------------------------------------------------------------------*/
{ 
  FILE *fpoutWt, *fpoutaWt;
  char agridf_wt[200], agridf_wt_ascii[200];
  int cnt, nwritten;
 
// output file names
  sprintf(agridf_wt,"%s.out", bsnm);
  sprintf(agridf_wt_ascii,"%s.txt", bsnm);

// WRITE RESULTS TO FILE
// uniform-weighting
  fpoutaWt=fopen(agridf_wt_ascii,"w");
  for(cnt=0; cnt<ntot; cnt++) {
    fprintf(fpoutaWt,"%.2f %.2f %.4e\n", lon[cnt], lat[cnt], a_wt[cnt]);
  }
  fclose(fpoutaWt);
// BINARY OUTPUT
// uniform-weighting
  fpoutWt=fopen(agridf_wt,"wb+");
  nwritten = fwrite(a_wt,4,ntot,fpoutWt);
  fclose(fpoutWt);

// print summary to screen
  fprintf(stderr,"Modified 10^a values written to files.  \nUniform weighting, binary: %s, ascii: %s\n", agridf_wt, agridf_wt_ascii);
}


/*--------------------------------------------------------------------------*/
void read_parameter_file(char *paramf, struct src_params* pars)
/*--------------------------------------------------------------------------*/
{
  FILE *fpparam;

// open file
  if( (fpparam = fopen(paramf, "r"))==NULL) {
    fprintf(stderr,"Parameter file not found, %s\n", paramf);
    exit(1);
  }

// get parameters
  Parse_Text(fpparam,"SOURCE_MIN_LON",'f',&(pars->min_lon));
  Parse_Text(fpparam,"SOURCE_MAX_LON",'f',&(pars->max_lon));
  Parse_Text(fpparam,"SOURCE_INCR_LON",'f',&(pars->inc_lon));
  Parse_Text(fpparam,"SOURCE_MIN_LAT",'f',&(pars->min_lat));
  Parse_Text(fpparam,"SOURCE_MAX_LAT",'f',&(pars->max_lat));
  Parse_Text(fpparam,"SOURCE_INCR_LAT",'f',&(pars->inc_lat));

// 
  pars->nlon=(int)((pars->max_lon-pars->min_lon)/pars->inc_lon+1);
  pars->nlat=(int)((pars->max_lat-pars->min_lat)/pars->inc_lat+1);
  pars->ntot=pars->nlat* pars->nlon;
// check array size
  if ( pars->ntot > NV) {
    fprintf(stderr,"Too many points (%d) for array size, NV=%d\n", pars->ntot, (int)NV);
    fprintf(stderr,"nlon/nlat: %d %d\n", pars->nlon, pars->nlat);
    exit(1);
  }

}

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
  FILE *fp_ref, *fppolylist;
  int ntot, nread, npoly=0, np;
  int cnt=0, cnt1=0, cnt2=0, cnt3, cnt4;
  float wtavg, lat, lon, floorv;
  float agrid_wt[NV], lon_arr[NV], lat_arr[NV], agrid_ref[NV];
  int n_polypts_arr[NP];
  float plon[NP], plat[NP];
  float wt_arr[NTOT];
  float lon_poly_arr[NTOT][NP], lat_poly_arr[NTOT][NP];
  char paramf[200], agrid_bsnm_f[200], agrdf_ref[200], listpolyf[200], polyf[200];
  char buff[BUFFLEN];
  struct src_params src;

/* CHECK INPUT ARGUMENTS */
  if ( (argc == 2 ) && STREQ(argv[1],"--h") ) print_parameter_file_format();
  if ( argc != 5 ) {
    fprintf(stderr,"USAGE: %s [parameter file] [background agrd file] [polygon rate file] [agrid output base-name]\n", argv[0]);
    fprintf(stderr,"Output basename used to write [_adapt/_wt][.out/.txt] files.\n");
    fprintf(stderr,"polygon-rate file formatted as: name of polygon file, weighting to apply within polygon\n");
    fprintf(stderr,"Background weight is weight to apply to zone-averaged rates.\n");
    fprintf(stderr,"Enter '--h' for required parameter file values\n");
    exit(1);
  }
  sscanf(argv[1],"%s",paramf);
  sscanf(argv[2],"%s",agrdf_ref);
  sscanf(argv[3],"%s", listpolyf);
  sscanf(argv[4],"%s",agrid_bsnm_f);

//  fprintf(stderr,"\nApplying weight of %.3f to seismicity rates, where local rate is below average.\n", wtavg);

// read parameters
  read_parameter_file(paramf,&src);
  fprintf(stderr,"\nSource parameters:\n");
  fprintf(stderr," lon: %.2f %.2f %.2f (%d pts)\n", src.max_lon, src.min_lon, src.inc_lon, src.nlon);
  fprintf(stderr," lat: %.2f %.2f %.2f (%d pts) \n", src.max_lat, src.min_lat, src.inc_lat, src.nlat);

// read all polygon files
  if((fppolylist = fopen(listpolyf, "r"))==NULL) {
    fprintf(stderr,"Cannot open list of polygon files - %s\n", listpolyf);
    exit(1);
  }
//  fprintf(stderr,"Opened list of polygon files - %s\n", listpolyf);
  npoly=0;
  while(fgets(buff,BUFFLEN,fppolylist) ) {
    sscanf(buff,"%s %f", polyf, &wt_arr[npoly]);
    fprintf(stderr,"Read polygon file, %s, %.5f ...\n", polyf, wt_arr[npoly]);
    read_polygon_files(polyf,plon,plat,&np);
    for (cnt=0; cnt<np; cnt++){
      lon_poly_arr[npoly][cnt]=plon[cnt];
      lat_poly_arr[npoly][cnt]=plat[cnt];
      n_polypts_arr[npoly]=np;
    }
    npoly++;
    if (npoly>NTOT) {
      fprintf(stderr,"Array only permits %d polygon files. Modify code and re-start.\n", (int)NTOT);
      exit(1);
    }
  }
  fprintf(stderr,"Read weights associated with %d polygon files.\n", npoly);

// read smoothed seismicity rates
  ntot=src.ntot;
  fp_ref=fopen(agrdf_ref,"rb");
  nread = fread(agrid_ref,4,ntot,fp_ref);
  fclose(fp_ref);
  fprintf(stderr,"Read %d (of %d) from file, %s\n", nread, ntot, agrdf_ref);

// assign lon/lat values
  cnt=0;
//  need this ordering to write a-values to binary in the order expected by hazgrid codes
FILE *fpp;
int whichpoly;
  fpp=fopen("f.tmp","w");
  fprintf(stderr,"Searching over %d polygon files...\n", npoly);
  for(cnt2=src.nlat-1; cnt2>=0; cnt2--) {
    for(cnt1=0; cnt1<src.nlon; cnt1++) {
      floorv=0.0;
      whichpoly=-1;
      lat_arr[cnt]=src.min_lat+src.inc_lat*cnt2;
      lon_arr[cnt]=src.min_lon+src.inc_lon*cnt1;
      lat=lat_arr[cnt];
      lon=lon_arr[cnt];
      
//lon=-115.0;
//lat=40.0;
//      fprintf(stderr,"%.3f %.3f\n", lon, lat);
// loop over all polygons
      for(cnt4=0; cnt4<npoly; cnt4++) {
//fprintf(stderr,"polygon number: %d (%d)\n", cnt4, npoly);
        np=n_polypts_arr[cnt4];
        for(cnt3=0; cnt3<np; cnt3++) {
          plon[cnt3]=lon_poly_arr[cnt4][cnt3];
          plat[cnt3]=lat_poly_arr[cnt4][cnt3];
        }
// check if point within polygon
//for(cnt3=0; cnt3<np; cnt3++) fprintf(stderr,"%.2f %.2f\n", plon[cnt3], plat[cnt3]);
        wtavg=1.0;
        if ( lxyint_(&lon,&lat,plon,plat,&np) ) {
          wtavg=wt_arr[cnt4];
        }
//fprintf(stderr,"%.2f %.2f %.2f\n", lon, lat, wtavg);
//fprintf(stderr,"np=%d, floorv/whichpoly: %.5f %d\n\n", np, floorv, whichpoly);
      }
      agrid_wt[cnt]=agrid_ref[cnt]*wtavg;
      fprintf(fpp,"%.3f %.3f %.5f %.5f %.5f\n", lon, lat, wtavg, agrid_ref[cnt],agrid_wt[cnt]);
      cnt++;
    }
  }
fclose(fpp);

// WRITE RESULTS TO FILE
  write_results_to_file(agrid_bsnm_f,agrid_wt,lon_arr,lat_arr,ntot);

  return 0;
}
