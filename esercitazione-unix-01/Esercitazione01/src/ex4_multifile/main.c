#include <stdio.h>
#include <math.h>
#include "fast_trigo.h"
#include "test_func.h"
#include "util.h"

int main(int argc,char** argv) {
  enum NormType ntype;
  double err, valMin, valInc;
  int valNum;
  
  if (argc < 1) {
    ntype = NORM_INF;
    printf("Use: %s norm_type\nNorm type not provides: using default %d\n",ntype);
  }
  
  valNum = 360;
  valMin = 0.0;
  valInc = 2.0 * M_PI / valNum;
  printf("Testing cos() vs fastCos()\n");
  err = funcCmp(cos, fastCos, 0, valMin, valInc, valNum);
  printf("error: %f", err);
  
  printf("Testing sin() vs fastSin()\n");
  err = funcCmp(sin, fastSin, 0, valMin, valInc, valNum);
  printf("error: %f", err);
  
  valNum = 21;
  valMin = -10.0;
  valInc = 1.0;
  printf("Testing atan() vs fastAtan()\n");
  err = funcCmp(atan, fastAtan, 0, valMin, valInc, valNum);
  printf("error: %f", err);

  return 0;
}
