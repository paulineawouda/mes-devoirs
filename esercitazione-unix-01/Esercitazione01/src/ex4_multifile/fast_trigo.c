/**
 * University of Parma - Sistemi Operativi.
 * Author: Dario Lodi Rizzini
 * 
 * Example adapted from https://github.com/dlr1516/rofl. 
 */
#include "fast_trigo.h"

double fastCos(double x) {
  double c;
  x *= FACTOR_1_FRAC_2PI;
  c = x - (0.25 + floor(x + 0.25));
  c *= 16.0 * (fabs(c) - 0.5);
  c += 0.225 * c * (fabs(c) - 1.0);
  return c;
}

double fastSin(double x) {
  double s;
  x *= FACTOR_1_FRAC_2PI;
  s = x - floor(x + 0.5);
  s *= 16.0 * (0.5 - fabs(s));
  s += 0.225 * s * (fabs(s) - 1.0);
  return s;
}

double fastAtan(double x) {
  MY_ASSERT(-1.0 <= x && x <= 1.0);
  double x2 = x * x;
  double x4 = x2 * x2;
  return x * (ATAN_A1 + x2 * (ATAN_A3 + ATAN_A7 * x4) + x4 * (ATAN_A5 + ATAN_A9 * x4));
}

