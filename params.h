#define R 6371
#define BUFFLEN 300
#define NP 100
#define NV 175000
#define NTOT 10
#define THRV 0.01
#define STREQ(s1,s2) (strcmp(s1,s2) == 0)

struct src_params {
  int nlon, nlat, ntot;
  float max_lat, min_lat, inc_lat;
  float max_lon, min_lon, inc_lon;
  float rate_ann;
} ;

/*
struct agrid_vals {
  int ntot;
  float lat[NV], lon[NV], avals[NV];
} ;
*/
