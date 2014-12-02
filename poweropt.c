/***********************************************************/
/*    Sample 1 from CFSQP distribution			   */
/*    Problem 32 from Hock/Schittkowski 1981               */
/***********************************************************/

#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <malloc.h>
#include "poweropt.h"
#define TIMEON 0

   int skip2 = 0;
   double P_gain = 0.5;
   double I_gain = 0.05;
   double I_prev = 0.0;
   double * P;
   double * ERR;
   int napps;
   double * actuators;
   double * x;
   double CurrentActuators[2] = {0.0, 0.0};

int gnparam=0;

double * PowerOpt(optmsg * Amsg, int len, double * C){

#if 0
printf("Error %f\n",Amsg[0].error);
#endif
#if 0
   P = (double *)malloc(len*sizeof(double));
   ERR = (double *)malloc(len*sizeof(double));
#endif
   actuators = (double *)calloc(2,sizeof(double));
#if 0   
  for(i=0;i<len;i++){
    ERR[i] = Amsg[i].error;
    P[i] = Amsg[i].p;
  }
#endif
int i;
  for(i=0;i<2;i++){
    CurrentActuators[i] = C[i];
  }

#if 0
   if(++skip2 > 4){
     printf("Current error %f\n",Amsg[0].error);
     printf("Current I error %f\n", I_prev);
     skip2 = 0;
   }
#endif
 
   #define ILIMIT 0.5
   double out;
   double P = Amsg[0].error*P_gain;
   double I = Amsg[0].error*I_gain;
   I += I_prev;
   I_prev = I;
   if(I_prev > ILIMIT)
	I_prev = ILIMIT;
   if(I_prev < -ILIMIT)
	I_prev = -ILIMIT;
   out = P+I;

   if(out < 0)
	out *= 0.5;

   actuators[0] = 14;
   actuators[1] = CurrentActuators[1]+out;
   return actuators;
}
   

