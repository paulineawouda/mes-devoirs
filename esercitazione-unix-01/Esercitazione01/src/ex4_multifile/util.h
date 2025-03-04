#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MY_ASSERT(X) \
  if (!(X)) { \
    printf("%s,%d: assertion failed %s\n",__FILE__,__LINE__,#X); \
    exit(-1); \
  }
  
#define TIME_BLOCK_EXEC(msg, ...) do {                                       \
    struct timeval start, end;                                               \
    gettimeofday(&start, NULL);                                              \
    __VA_ARGS__                                                              \
    gettimeofday(&end, NULL);                                                \
    printf("%s execution time: %ldms\n", msg, computeElapsedTime(start, end)); \
} while(0); 
  
/** 
 * Returns the elapsed time between two timeval in milliseconds.
 */
long computeElapsedTime(struct timeval start, struct timeval end)
{
  return ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))/1000.0;
}
