#include <stdlib.h>
#include <stdio.h>
#include "verror.h"

double weights[40] = {15.0, 18.0, 23.0, 29.0, 36.0, 45, 56, 70, 87, 110, 137, 172, 215, 272,335, 423, 526, 655, 820, 1024, 1277, 1586, 1991, 2501, 3121,3906, 4904, 6100, 7620, 9548, 11916, 14949, 18705, 23254, 29154, 36291, 46273, 56483, 71755, 88761};

double * getverror(double * errors, int * priorities, int napps){
  double * verrors = (double *)malloc(sizeof(double)*napps);
  double weightsum = 0.0;
  int i,j;
  
  for(i=0; i<napps; i++)
    verrors[i] = errors[i];


  for(j=0; j<napps; j++){
    weightsum = 0.0;
    for(i=0; i<napps; i++){
	weightsum += weights[priorities[i]];
    }
    verrors[j] = 2*verrors[j]*weights[priorities[j]]/weightsum;
  }
//printf("1 %f\n", verrors[0]);
//printf("2 %f\n", verrors[1]);
  return verrors;
}
