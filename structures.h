 
#ifndef STRUCTURES
#define STRUCTURES

typedef struct optmsg{
 double error;
 double p;
}optmsg;


typedef struct msgp{
 long retardo;
 unsigned int ID;
 double error;
 double p;
}msgp;

typedef struct msgpl{
  msgp * data;
  unsigned int len;
}msgpl;

#endif