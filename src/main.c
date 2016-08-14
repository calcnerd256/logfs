#include "./main.h"

// for printf
#include <stdio.h>

// for memset
#include <string.h>


int paranoid_return(int status, void* start, size_t size){
  if(start)
    memset(start, 0, size);
  return status;
}

int main(int arfc, char* *arfv){
  if(2 == arfc){
    if('N' == *arfv[1])
      if('O' == arfv[1][1])
	if('P' == arfv[1][2])
	  if(0 == arfv[1][3])
	    return main_NOP();
  }
  if(3 == arfc){
    if('w' == *arfv[1])
      if('r' == arfv[1][1])
	if('i' == arfv[1][2])
	  if('t' == arfv[1][3])
	    if('e' == arfv[1][4])
	      if(0 == arfv[1][5])
		return main_write(arfv[2]);
    if('r' == *arfv[1])
      if('e' == arfv[1][1])
	if('a' == arfv[1][2])
	  if('d' == arfv[1][3])
	    if(0 == arfv[1][4])
	      return main_read(arfv[2]);
  }
  printf("%d\n", arfc);
  printf("%s\n", *arfv);
  return 0;
}
