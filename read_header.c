#include <stdio.h>
#include <stdlib.h>
#define BUFFLEN2 300

void read_header(FILE *fp, int nlines)
{
  int cnt=0;
  char buff[BUFFLEN2];
  for (cnt=0; cnt<nlines; cnt++) fgets(buff,BUFFLEN2,fp);
}
