#include <stdio.h>
#include <stdlib.h>


unsigned int fib(unsigned n) {
  // State variable uses hexadecimal numbers
  unsigned state = 0x00;
  // Return value / fibonacci number
  unsigned int result = 0;
  // Fibbonacci formular: f_n = f_{n-1} + f_{n-2}
  unsigned a = 0;       // f_0
  unsigned b = 1;       // f_1
  unsigned int s = b;   // f_n
  // Loop index
  unsigned i = 2;

  while(1) {
    switch(state) {
      // Initial state
      case 0x00:
        if (n == 0) {
          result = a;
          state = 0x99; // exit
        } else {
          if (n == 1) {
            result = b;
            state = 0x99; // exit
          } else {
            state = 0x01; // loop
          }
        }
        break; 

      // Fibbonacci loop
      case 0x01:
        if (i <= n) {
          s = a + b;
          a = b;
          b = s;

          i += 1;
        } else {
          result = s;
          state = 0x99; // exit
        }
        break;

      case 0x99:
        // Exit state
        return result;
    }
  }
}

int main (int argc, char** argv) {
  if (argc < 2) {
    return -1;
  }
  
  unsigned int n = atoi(argv[1]);
  
  printf("result: %u\n", fib(n));
  
  return 0;
}
