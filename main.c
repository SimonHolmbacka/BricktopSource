 
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include "poweropt.h"
#include "PowerFuzzy.h"
#include "DPMCompensate.h"
#include "structures.h"
#include "mpiserver.h"
#include "actuator.h"
#include "util.h"
#include "kalman.h"
#include "verror.h"

#if 1
long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
    ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}
#endif

void sort(optmsg * s, int len){
 int c,d;
 optmsg t;
 
 for (c = 1 ; c <= len - 1; c++) {
    d = c;
    while ( (d > 0) && (s[d].p > s[d-1].p)) {
      t          = s[d];
      s[d]   = s[d-1];
      s[d-1] = t;
      d--;
    }
  } 
}

int main(int argc, char *argv[])
{
  //printf("echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");

//This is because android is fucked up
  pid_t pid;
  int ret;
  switch((pid = fork())){
    case -1:
        perror("fork");
        break;
    case 0: //child
        ret = execl("/system/bin/sh", "sh", "-c", "/system/bin/sh /data/data/org.videolan.vlc/files/setCPU.sh", (char *)NULL);
        exit(EXIT_FAILURE);
        break;
    default:
	break;
  }

  int i,j,index;
  double MyErrors[2] = {0,0};
  double MyErrorSum[100];
  for(i=0;i<100;i++)
    MyErrorSum[i] = 0;
  double CurrentActuators[2] = {1.0,14.0};
  double * pfuzzy = (double *)malloc(1*sizeof(double));
  //double * popt;
  
  while(1){
    msgpl mainmsg;

    mainmsg = getmsg();		//Get all messages in queue (different applications)
#if 0
    for(i=0;i<mainmsg.len;i++)
      printf("Index %d has raw error %f\n",i,mainmsg.data[i].error);
#endif

    if(mainmsg.data[0].error != -100){  //If we have something to process   
      for(i=0;i<mainmsg.len;i++){
	CheckError(&mainmsg.data[i].error,-1.0,1.0);
	CheckError(&MyErrorSum[mainmsg.data[i].ID],-14.0,14.0);    
	/*Fuzzy Controller returns a value [-1.5 1.5] where -1.5 means too many resources*/
// 	printf("Going fuzzy with error %f and sum %f\n", mainmsg.data[i].error, MyErrorSum[mainmsg.data[i].ID]);
	PowerFuzzyInferenceEngine(mainmsg.data[i].error, MyErrorSum[mainmsg.data[i].ID], pfuzzy);	
  //       printf("pfuzzy is %f\n",*pfuzzy);
	MyErrorSum[mainmsg.data[i].ID] += mainmsg.data[i].error;
	mainmsg.data[i].error = *pfuzzy;
#if 0
        printf("Error for %d after fuzzy was %f\n",i,mainmsg.data[i].error);
        printf("Error sum after fuzzy was %f\n",MyErrorSum[mainmsg.data[i].ID]);
#endif
      }
    
    index = 0;
    for(i=0;i<mainmsg.len;i++){
      for(j=0;j<mainmsg.len;j++){
	if((i != j) && (mainmsg.data[j].ID != 0)){
	  if(mainmsg.data[i].ID == mainmsg.data[j].ID){ //Check if the same application has several inputs
	    mainmsg.data[i].error = (mainmsg.data[i].error+mainmsg.data[j].error)/2.0;
#if 0
	    printf("Now the error %f is at index %d\n",mainmsg.data[i].error,i);
#endif
	    mainmsg.data[j].error = 99; //Then ignore this value
	    mainmsg.data[j].ID = 0;
          }
	}
      }
    }
#if 0
    for(i=0;i<mainmsg.len;i++){
      printf("Index %d with error %f and p %f\n",i,mainmsg.data[i].error,mainmsg.data[i].p);
      printf("ID %d\n",mainmsg.data[i].ID);
    }
#endif
    
    int ID_levels=0;
    for(i=0;i<mainmsg.len;i++){
      if(mainmsg.data[i].error != 99)
	ID_levels++;
    }
//printf("ID_levels %d\n",ID_levels);
    optmsg * myoptmsg = (optmsg *)malloc(ID_levels*sizeof(optmsg));
    int * ID_list = (int *)malloc(sizeof(int)*ID_levels);
    for(i=0,j=0;i<mainmsg.len;i++){
      if(mainmsg.data[i].error != 99){
	ID_list[j] = mainmsg.data[i].ID;
	//printf("Adding to ID list %d\n",mainmsg.data[i].ID);
	myoptmsg[j].error = mainmsg.data[i].error*2;
	myoptmsg[j++].p = mainmsg.data[i].p;
      }
    }

#if 1
  if(ID_levels >= 2){	//if we need priorities
    double * reterrors = (double *)malloc(sizeof(double)*ID_levels);
    double * senderrors = (double *)malloc(sizeof(double)*ID_levels);

    for(i=0;i<ID_levels;i++){
      //printf("Pre Error for ID %d is %f\n",ID_list[i], myoptmsg[i].error);
      senderrors[i] = myoptmsg[i].error;

    }
    //ta ut errors och prioriteter ur mainmsg
    reterrors = getverror(senderrors, ID_list, ID_levels);
    //skriv över mainmsg med nya resultst
   for(i=0;i<ID_levels;i++){
     myoptmsg[i].error = reterrors[i];
     CheckError(&myoptmsg[i].error,-1.5,1.5);
   }
   
#if 0
   for(i=0;i<ID_levels;i++){
     printf("Post Error for ID %d is %f\n",ID_list[i], myoptmsg[i].error);
   }
   printf("\n");
#endif
   free(reterrors);
   free(senderrors); 
   free(ID_list);

  }
#endif
    /* 
     * Sort value so that the highest parallelism comes first
     * This will give better optimization results
     */
    free(mainmsg.data);
    sort(myoptmsg,ID_levels);

#if 0
    for(i=0;i<ID_levels;i++){
      printf("app %d with error %f and p %f\n",i,myoptmsg[i].error, myoptmsg[i].p);
    }
#endif   
    if(ID_levels > 0){
#if 0
      printf("\n-----------New run--------------\n");
      printf("Current0 is %f\nCurrent1 is %f\n",CurrentActuators[0],CurrentActuators[1]);
#endif
      double oldDPM = CurrentActuators[0];
      double * popt = (double *)malloc(ID_levels*2*sizeof(double));
      popt = PowerOpt(myoptmsg,ID_levels,CurrentActuators); //skicka current actuators som upper limit (och cores)
      double xodd = 0;
      double xeven = 0;

      for(i=0;i<ID_levels*2;i++){
      if(i%2 !=0){		//odd
	xodd += popt[i];
      }
      else{			//even
	xeven += popt[i];
      }
      }
      CurrentActuators[0] = xeven;
      CurrentActuators[1] = xodd;
      CheckError(&CurrentActuators[0],0.0,14.0);
      CheckError(&CurrentActuators[1],2.0,14.0);
#if 0
      printf("---Final results---\n");
//      printf("DPM: %f\n",CurrentActuators[0]);
      printf("DVFS: %f\n",CurrentActuators[1]);
#endif

      actuator(CurrentActuators);
      free(popt);
      free(myoptmsg);

    }
    else{
    }

    }
    usleep(50*1000); //40//150
  }
  free(pfuzzy);
  return 0;
}
