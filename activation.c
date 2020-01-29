#include "activation.h"

double sigmoid(double x)
{ return 1/(1+exp(-x)); }

double sigmoid_d(double x)
{ return x*(1-x); }
	
double heaviside(double x)
{ return (x<0)?0:1; }

double heaviside_d(double x)
{ return (x!=0)?0:1; }

double tanh(double x)
{ return ( 2/(1+exp(-2*x)) ) - 1; }

double tanh_d(double x)
{ return 1-(x*x); }

double arctan(double x)
{ return atan(x); }

double arctan_d(double x)
{ return 1/((x*x)+1); }

double reLU(double x)
{ return (x<=0)?0:x; }

double reLU_d(double x)
{ return (x<=0)?0:1; }
