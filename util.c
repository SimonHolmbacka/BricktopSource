#include <stdio.h>
#include <stdlib.h>
#include <math.h>



void CheckError(double * e, double l, double u){
  if(*e > u)
    *e = u;
  if(*e < l)
    *e = l;
}
