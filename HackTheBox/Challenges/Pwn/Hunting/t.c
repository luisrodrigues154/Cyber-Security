#include <stdio.h>

const char code[] = "\xb8\x0a\x00\x00\x00\xc3";

int main(int argc, char **argv) {
  int foo_value = 0;

  int (*foo)() = (int(*)())code;
  foo_value = foo();

  printf("%d\n", foo_value);
}