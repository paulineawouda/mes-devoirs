#include <stdio.h>

int main()
{
#ifdef TEST
  printf ("Test mode\n");
#endif
  printf ("Running...\n");
  return 0;
}
