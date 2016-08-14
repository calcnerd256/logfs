// for size_t
#include <unistd.h>

// common
#define COLS 80

// common
int paranoid_return(int status, void *start, size_t size);

// main() calls the following interfaces
int main_NOP(void);
int main_write(char* path);
int main_read(char* path);
