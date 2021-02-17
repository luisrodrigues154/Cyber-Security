#include <stdio.h>
#include <stdlib.h>

const char code[] = "\x31\xc0\x31\xdb\xb8\x01\x";

int main(int argc, char **argv) {
  int foo_value = 0;
  char * buf;

  buf = malloc(0x3C);

  read(0, buf, 0x3C);
  //((int(*)())buf)();
  int (*foo)() = (int(*)())code;
  foo_value = foo();
  free(buf);
}
