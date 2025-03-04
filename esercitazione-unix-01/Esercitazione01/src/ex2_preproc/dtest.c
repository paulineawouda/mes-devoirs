#include <stdio.h>

#ifdef DEBUG
  #define VARINT(X) printf("%s,%d: %s = %d\n", __FILE__, __LINE__, #X, (X)) 
#else 
  #define VARINT(X) 
#endif
 

int main()
{
  int i =2;
#ifdef TEST
  printf ("Test mode\n");
#endif
  printf ("Running...\n");
  VARINT(i);
  return 0;
}
