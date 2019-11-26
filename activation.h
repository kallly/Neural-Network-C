#ifndef ACTIVATION_H
#define ACTIVATION_H

#include<math.h>


double sigmoid(double x);
double sigmoid_d(double x);
double heaviside(double x);
double heaviside_d(double x);
double tanh(double x);
double tanh_d(double x);
double arctan(double x);
double arctan_d(double x);
double reLU(double x);
double reLU_d(double x);

#endif
