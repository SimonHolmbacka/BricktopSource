#include <stdio.h> 
#include <sys/types.h>
#include <sys/msg.h>
#include "structures.h"
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_BUF 32

int init = 0;

//volatile sig_atomic_t print_flag = false;
volatile int time_flag = 0;

void handle_alarm( int sig ) {
  printf("Alarm %d\n",time_flag);
  if(time_flag++ >= 5){
      pid_t pid;
      int ret;  
      switch((pid = fork())){
      case -1:
        perror("fork");
        break;
      case 0: //child
        ret = execl("/system/bin/sh", "sh", "-c", "/system/bin/sh /data/data/org.videolan.vlc/files/setOndemand.sh", (char *)NULL);
        exit(EXIT_FAILURE);
        break;
      default:
        break;
    }
 printf("Shutting down\n");
 exit(0);
 }
  alarm(1);
}

msgpl getmsg()
{
  msgpl retblock;

  if(!init){ 
    signal( SIGALRM, handle_alarm ); // Install handler first,
    alarm( 1 ); // before scheduling it to be called
    init = 1;
  }

  msgp * retmsg = (msgp *)malloc(1*sizeof(msgp));
  int fd;
  char * myfifo = "/data/data/org.videolan.vlc/fifo";
  //char * myfifo = "/tmp/fifo";
  double buf[MAX_BUF];
  buf[0] = -1;

  do{  
    fd = open(myfifo, O_RDONLY); 
    read(fd, buf, MAX_BUF);   
    close(fd);
  usleep(1000);
  //printf("Buf was %f\n", buf[0]);
  }while (buf[0] == -1); 
  //printf("Got valid message %f\n",buf[0]);
  time_flag = 0;
  retmsg[0].error = buf[0];
  retmsg[0].retardo = 2;
  retmsg[0].ID = 1;
  retmsg[0].p = 1.0;
  retblock.data = retmsg;
  retblock.len = 1;
  return retblock;
}
