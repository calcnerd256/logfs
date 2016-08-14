#include "./main.h"

#include <stdio.h>
#include <string.h>

#define WRITE_BUFFER_LENGTH ((COLS - 13)/2)

int print_append_buffer(char transaction, char var, char* buffer, size_t length){
  int line_remaining;
  while(length){
    printf("$%c[%c] .= 0x", transaction, var);
    line_remaining = WRITE_BUFFER_LENGTH;
    while(length && line_remaining){
      printf("%02X", *buffer);
      buffer++;
      length--;
      line_remaining--;
    }
    printf("\n");
  }
  return paranoid_return(0, &line_remaining, sizeof(line_remaining));
}

char xor_buffer(char* buffer, size_t length){
  char result = 0;
  while(length--)
    result ^= *(buffer++);
  return result;
}

int main_write(char* path){
  char buffer[WRITE_BUFFER_LENGTH + 1];
  int counter;
  char checksum;
  int pathlen;
  printf("begin $0\ndim $0[1]\ndim $0[2]\nplan: store $0[2] at $0[1]\n");
  pathlen = strlen(path);
  print_append_buffer('0', '1', path, pathlen);
  checksum = xor_buffer(path, pathlen);
  printf("finalize $0[1] ^ 0x%02X", checksum);
  buffer[WRITE_BUFFER_LENGTH] = 0;
  counter = read(0, buffer, WRITE_BUFFER_LENGTH);
  checksum = 0;
  while(WRITE_BUFFER_LENGTH == counter){
    printf("\n$0[2] .= 0x");
    for(counter = 0; counter < WRITE_BUFFER_LENGTH; counter++){
      printf("%02X", buffer[counter]);
      checksum ^= buffer[counter];
    }
    counter = read(0, buffer, WRITE_BUFFER_LENGTH);
  }
  // last line may be shorter
  buffer[counter] = 0;
  counter = 0;
  printf("\n$0[2] .= 0x");
  while(buffer[counter]){
    checksum ^= buffer[counter];
    printf("%02X", buffer[counter++]);
  }
  printf("\nfinalize $0[2] ^ 0x%02X\n", checksum);
  printf("store $0[2] at $0[1]\nforget $0[2]\nforget $0[1]\ncommit $0\n");
  return paranoid_return(0, buffer, WRITE_BUFFER_LENGTH + 1);
}
