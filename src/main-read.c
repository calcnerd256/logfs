#include "./main.h"

#include <stdio.h>

int read_line(char* buffer, size_t maxbytes, size_t* bytes_read_out){
  // basically gets() or fgets(), in the errcode monad
  size_t numbytes;
  size_t bytes_read = 0;
  while(maxbytes--){
    numbytes = read(0, buffer, 1);
    bytes_read += numbytes;
    if(1 != numbytes)
      return paranoid_return(1, &bytes_read, sizeof(bytes_read));
    if('\n' == *(buffer++)){
      if(bytes_read_out)
	*bytes_read_out = bytes_read;
      *buffer = 0;
      return paranoid_return(0, &bytes_read, sizeof(bytes_read));
    }
  }
  return paranoid_return(2, &bytes_read, sizeof(bytes_read));
}

int compare_string_to_length(char* str, char* goal, size_t len){
  while(1){
    if(!len--)
      return 0;
    if(!*str) // case: goal has "any" where str ends
      return 1;
    if(!*goal) continue;
    if(*(str++) != *(goal++))
      return 1;
  }
}

int expect_line_template(char* goal, size_t len){
  /*
    errno monad:
     0: success
     1: wrong length
     2: mismatch
     3: read failure
  */
  char line[COLS];
  size_t numbytes;
  if(read_line(line, COLS, &numbytes))
    return paranoid_return(3, line, COLS);
  if(len != numbytes)
    return paranoid_return(1, line, COLS);
  if(compare_string_to_length(line, goal, len))
    return paranoid_return(2, line, COLS);
  return paranoid_return(0, line, COLS);
}

int read_hex_byte(char* str, char* value_out){
  char c = *(str++);
  char value = 0;
  if(c < '1' && c != '0')
    return paranoid_return(1, &c, sizeof(c));
  if(c > '9' && c != '0' && c < 'A')
    return paranoid_return(2, &c, sizeof(c));
  if(c > 'F')
    return paranoid_return(3, &c, sizeof(c));
  if(c < 'A') value = (c - '0') ? (1 + c - '1') : 0;
  else value = 10 + c - 'A';
  value <<= 4;
  c = *str;
  if(c < '1' && c != '0')
    return paranoid_return(4, &c, sizeof(c));
  if(c > '9' && c != '0' && c < 'A')
    return paranoid_return(5, &c, sizeof(c));
  if(c > 'F')
    return paranoid_return(6, &c, sizeof(c));
  if(c < 'A') value += (c - '0') ? (1 + c - '1') : 0;
  else value += 10 + c - 'A';
  *value_out = value;
  return paranoid_return(0, &c, sizeof(c));
}

int read_journal_compare_hex_string(char* goal){
  char line[COLS];
  size_t numbytes;
  char *cursor;
  char value;
  char checksum = 0;
  while(1){
    if(read_line(line, COLS, &numbytes))
      return paranoid_return(1, line, COLS);
    cursor = line;
    if(compare_string_to_length(line, "$\0[\0] .= 0x", 11)) break;
    cursor += 11;
    numbytes -= 11;
    while(--numbytes){
      if(read_hex_byte(cursor, &value))
	return paranoid_return(2, line, COLS);
      numbytes--;
      cursor += 2;
      if(*(goal++) - value)
	return paranoid_return(3, line, COLS);
      checksum ^= value;
    }
  }
  if(compare_string_to_length(line, "finalize $\0[\0] ^ 0x\0\0\n", 22))
    return paranoid_return(1, line, COLS);
  if(read_hex_byte(line + 19, &value))
    return paranoid_return(1, line, COLS);
  if(value != checksum)
    return paranoid_return(3, line, COLS);
  return paranoid_return(0, line, COLS);
}

int read_journal_print_hex_string(void* context){
  char line[COLS];
  size_t numbytes;
  char *cursor;
  char value;
  char checksum = 0;
  while(1){
    if(read_line(line, COLS, &numbytes))
      return paranoid_return(1, line, COLS);
    cursor = line;
    if(compare_string_to_length(line, "$\0[\0] .= 0x", 11)) break;
    cursor += 11;
    numbytes -= 11;
    while(--numbytes){
      if(read_hex_byte(cursor, &value))
	return paranoid_return(2, line, COLS);
      numbytes--;
      cursor += 2;
      printf("%c", value);
      checksum ^= value;
    }
  }
  if(compare_string_to_length(line, "finalize $\0[\0] ^ 0x\0\0\n", 22))
    return paranoid_return(1, line, COLS);
  if(read_hex_byte(line + 19, &value))
    return paranoid_return(1, line, COLS);
  if(value != checksum)
    return paranoid_return(3, line, COLS);
  return paranoid_return(0, line, COLS);
}

int main_read(char* path){
  int status;
  if(expect_line_template("begin $\0\n", 9))
    return 1;
  if(expect_line_template("dim $\0[\0]\n", 10))
    return 1;
  if(expect_line_template("dim $\0[\0]\n", 10))
    return 1;
  if(expect_line_template("plan: store $\0[\0] at $\0[\0]\n", 27))
    return 1;
  status = read_journal_compare_hex_string(path);
  if(status)
    return 2;
  status = read_journal_print_hex_string(0);
  if(status)
    return 3;
  if(expect_line_template("store $\0[\0] at $\0[\0]\n", 21))
    return 1;
  if(expect_line_template("forget $\0[\0]\n", 13))
    return 1;
  if(expect_line_template("forget $\0[\0]\n", 13))
    return 1;
  if(expect_line_template("commit $\0\n", 10))
    return 1;
  return 0;
}
