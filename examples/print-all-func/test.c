#include <stdio.h>


void my_func(void) {
  printf("Hello, world!\n");
}

void my_module(void) {
  printf("This is my module.\n");
  //say_hello();
}

int main() {
  my_module();
  return 0;
}
