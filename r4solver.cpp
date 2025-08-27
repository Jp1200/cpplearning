#include <iostream>
#include <cmath>
#include <vector>


//sample function 
double dydx(double t , double y){
  return((t-y)/2);
}
// Here y is an unknown function(scalar or vector) of time t, which we would like to approximate;
// the dy/dt, the rate at which y changes, is a function  of t and of y itself.
//
double rk4(double x0, double y0, double x, double step_size, double (*func)(double, double)){
  double k1, k2, k3, k4;
  int n = (int)((x-x0) / step_size); 
  //initial values of time x, and value y 
  double y = y0;
  
  for(int i = 1; i <= n; ++i){
    
    k1 = func(x0,y);
    k2 = func(x0 + step_size/2, y + step_size*(k1/2));
    k3 = func(x0 + step_size/2, y + step_size*(k2/2));
    k4 = func(x0 + step_size, y + step_size*k3);
    
  //update next value
  y = y + (step_size/6.0)*(k1 + 2*k2 + 2*k3 + k4);
  //update x 
  x0 = x0 + step_size;
  }
  return y;
}
int  main (int argc, char *argv[]) {
  //Drive this function for testing
  double x0 = 0, y = 1, x = 2 , h = 0.2;
  std::vector<double> dubVector(3);
  std::cout << "The value of y at x is: " << rk4(x0,y,x,h,dydx) << std::endl;
  return 0;
}

