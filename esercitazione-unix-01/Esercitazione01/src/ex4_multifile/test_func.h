/**
 * University of Parma - Sistemi Operativi.
 * Author: Dario Lodi Rizzini
 * 
 * Example adapted from https://github.com/dlr1516/rofl. 
 */
#include <math.h>
#include "util.h"

enum NormType { NORM_INF, NORM_1, NORM_2 };

/**
 * Compares a current function func1 to a function func2 in several 
 * scalar points and returns the error. 
 * Functions must be in the form: double func(double).
 * The evaluation points are: 
 *   x = xMin + xInc * i   with i=0, ..., num-1 
 * @param func1 a pointer to the first function to be evaluated
 * @param func2 a pointer to the second function to be evaluated
 * @param xMin minimum value of evaluation interval
 * @param xInc interval between two values of evaluation interval
 * @param xNum numer of evaluation points
 * @param ntype the type of the norm to be used in computation of error
 * @return the error 
 */
double funcCmp(double(*func1)(double), double(*func2)(double), 
               double xMin, double xInc, int xNum, enum NormType ntype) {
  double x, y1, y2, err;
  int i;
  err = 0; 
  MY_ASSERT(xNum > 0);
  for (i = 0; i < xNum; ++i) {
    x = xMin + xInc * i;
    y1 = (*func1)(x);
    y2 = (*func2)(x);
    err = updateErr(err, y1, y2, ntype); 
  }
  if (ntype == NORM_1 || ntype == NORM_2) {
    err = err / xNum;
  }
  return err; 
}

/**
 * Updates the error on a sequence of numeric values. 
 * @param errPrev previous value of error
 * @param val1 the first value to be compared
 * @param val2 the second value to be compared
 * @param ntype the type of the norm to be used in computation of error
 */ 
double updateErr(double errPrev, double val1, double val2, enum NormType ntype) {
  double diff, errCurr;
  diff = fabs(val1 - val2); 
  switch (ntype) {
    case NORM_INF: if (diff > errPrev) errCurr = diff; 
      break;
    case NORM_1: errCurr = errPrev + diff; 
      break;
    case NORM_2: 
    default: 
      errCurr = sqrt(errPrev*errPrev + diff*diff);  
  }
  return errCurr; 
}
