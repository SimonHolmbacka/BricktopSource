/*********************************************************************************
 This file is part of Bricktop Power Manager.

    Bricktop Power Manager is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bricktop Power Manager is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bricktop Power Manager.  If not, see <http://www.gnu.org/licenses/>. 
***********************************************************************************/

#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <malloc.h>
#include "poweropt.h"

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

  actuators = (double *)calloc(2,sizeof(double));
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

	if(out < 0)		//Saturation limit
		out *= 0.5;

   actuators[0] = 14;
   actuators[1] = CurrentActuators[1]+out;
   return actuators;
}
   

