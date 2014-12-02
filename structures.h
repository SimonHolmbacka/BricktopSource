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
