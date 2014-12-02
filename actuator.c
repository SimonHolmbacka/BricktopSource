#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <cpufreq.h>
#include "util.h"

#define CPU_INDEX 0

int skip = 0;
static int init_freqs = 0;
double DVFSNORM;
int DVFSSTEPS;
// Hardware related

//Transformerpad
//#define DPMSTEPS 4.0
//#define DPMNORM DPMSTEPS/14
//#define DVFSSTEPS 9.0
//#define DVFSNORM DVFSSTEPS/14

int c_online_prev = 1;

long * dvfs_table;
//Transformerpad
//long dvfs_table[13] = {102000, 204000, 340000, 475000, 640000, 760000, 880000, 1000000, 1100000};

char* readFileString( char* loc ) {
        char *fileDat;
        FILE * pFile;
        long lsize;

        pFile = fopen( loc, "r" );
        // Grab the file size.
        fseek(pFile, 0L, SEEK_END);
        lsize = ftell( pFile );
        fseek(pFile, 0L, SEEK_SET);
        fileDat = calloc( lsize + 1, sizeof(char) );
        fread( fileDat, 1, lsize, pFile );
	fclose(pFile);
        return fileDat;
}


int DVFS_prev = 0;

void actuator ( double* a )
{
    if(!init_freqs){
  	char * cfreqs = (char *)malloc(100*sizeof(char));
		cfreqs = readFileString("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies");
		int c = 0;
		int nfreqs = 0;
		printf("It was %s\n",cfreqs);
		while(cfreqs[c] != '\0'){
		if(cfreqs[c] == ' ')
			nfreqs++;
			c++;
		}
		nfreqs++;
		//printf("we have %d different frequencies\n",nfreqs+1);
		dvfs_table = (long *)malloc(nfreqs*sizeof(long));
		char tmpfreq[10];
		int tmpind = 0;
		int idvfs = 0;
		c = 0;
		while(cfreqs[c] != '\0'){
		tmpfreq[tmpind++] = cfreqs[c];
		//printf("Tmpind is %s\n",tmpfreq);
		if(cfreqs[c] == ' '){
			dvfs_table[idvfs] = atoi(tmpfreq);
			idvfs++;
			tmpind = 0;
		}
		c++;
		}
		dvfs_table[idvfs] = atoi(tmpfreq);
		free(cfreqs);
		init_freqs = 1;
		DVFSSTEPS = nfreqs;
 		DVFSNORM = (double)nfreqs/14.0;
	}

#if 0
  printf("in A0: %f\n",a[0]);
  printf("in A1: %f\n",a[1]);
#endif

  double DVFS = ceil(a[1]*DVFSNORM);
  int i = 0;
  
  CheckError(&DVFS,1.0,DVFSSTEPS);  

#if 0
  printf("DVFS %f \n",DVFS);
#endif

  int DVFS_set = (int)DVFS - 1;
  if(++skip > 4){
   printf("Setting DVFS to %lu\n\n",dvfs_table[DVFS_set]);
   skip = 0;
  }
  if(DVFS_set != DVFS_prev){

//Does not work unless root owns the process
#if 0
    cpufreq_set_frequency(0, dvfs_table[DVFS_set]);
#endif

  char buf[100];
  sprintf(buf,"/system/bin/sh /data/data/org.videolan.vlc/files/setFreq.sh %ld",dvfs_table[DVFS_set]);
  printf("%s\n",buf);
  pid_t pid;int ret;
  switch((pid = fork())){
   case -1:break;
   case 0: execl("/system/bin/sh", "sh","-c",buf,(char *)NULL);
           exit(EXIT_FAILURE); break;
   default: break;
  }

  DVFS_prev = DVFS_set;
  }

}
