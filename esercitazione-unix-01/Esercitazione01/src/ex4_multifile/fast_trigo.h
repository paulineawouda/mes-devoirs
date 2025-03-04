/**
 * University of Parma - Sistemi Operativi.
 * Author: Dario Lodi Rizzini
 * 
 * Example adapted from https://github.com/dlr1516/rofl. 
 */
#include <math.h>
#include "util.h"

#define FACTOR_1_FRAC_2PI 0.159154943091895
#define ATAN_A1 0.9998660
#define ATAN_A3 -0.3302995
#define ATAN_A5 0.1801410
#define ATAN_A7 -0.0851330
#define ATAN_A9 0.0208351

/**
 * Computes fast parabolic approximation of cos(x).
 * In particular, it is based on the parabolic apprximation of sine as: 
 * 
 *    y = 16.0 * xn * (abs(xn) - 0.5)
 *
 * where xn = x/(2*M_PI) is the normalized value of x over the period.
 * @param x the argument of the function. 
 * @return the cos(x)
 */
double fastCos(double x);

/**
 * Computes fast parabolic approximation of sin(x).
 * In particular, it is based on the parabolic apprximation of sine as: 
 * 
 *    y = 16.0 * xn * (abs(xn) - 0.5)
 *
 * where xn = x/(2*M_PI) is the normalized value of x over the period.
 * @param x the argument of the function. 
 * @return the sin(x)
 */
double fastSin(double x);

/**
 * Computes fast polynomial approximation of atan() on interval [-1,1]. See:
 *
 *  Abramowitz, Stegun, "Handbook of Mathematical Functions", 1965
 *
 * @param x the argument that must be in interval [-1.0, 1.0]. 
 * @return the computed atan(x)
 */
double fastAtan(double x);

