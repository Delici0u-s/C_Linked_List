#include <stdio.h>

int *a(int **b) {
  int *c = *b;
  *b = NULL;
  return c;
}

int main(void) {
  int b = 2;
  int *c = &b;
  printf("%p : %i\n", (void *)c, *c);

  int *d = a(&b);

  // Always check for NULL before dereferencing
  if (c == NULL) {
    printf("%p : (null)\n", (void *)c);
  } else {
    printf("%p : %i\n", (void *)c, *c);
  }

  printf("%p : %i\n", (void *)d, *d);

  puts("hi");
  return 0;
}
