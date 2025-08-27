#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <chrono>
#include <thread>

//sample function 
double dydx(double t , double y){
  return((t-y)/2);
}
// Here y is an unknown function(scalar or vector) of time t, which we would like to approximate;
// the dy/dt, the rate at which y changes, is a function  of t and of y itself.
//
// Compute single step
double rk4_step(double x, double y, double h, double (*func)(double, double)){

  double k1 = func(x,y);
  double k2 = func(x + h/2, y + h*(k1/2));
  double k3 = func(x + h/2, y + h*(k2/2));
  double k4 = func(x + h, y + h*k3);
  
  return y + (h/6.0)*(k1 + 2*k2 + 2*k3 + k4);
}
std::vector<std::pair<double, double> > solve_range(double x0, double y0, double x_max, double h, double (*func)(double, double)){
  std::vector<std::pair<double, double> > results;
  results.emplace_back(x0,y0);

  double x = x0;
  double y = y0;
  while(x < x_max){
    if(x + h > x_max){
      h = x_max - x;
    }
     y = rk4_step(x, y, h, func);
     x += h;
     results.emplace_back(x,y);
  }
  return results;
}
void animateSolution(const std::vector<std::pair<double,double>> &data, int width, int height) {
    // Determine scaling based on data range
    double maxX = data.back().first;
    double maxY = data[0].second;
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i].second > maxY) maxY = data[i].second;
    }

    double scaleX = (width - 1) / maxX;
    double scaleY = (height - 1) / maxY;

    for (size_t i = 0; i < data.size(); ++i) {
        int col = static_cast<int>(data[i].first * scaleX);
        int row = static_cast<int>((height - 1) - data[i].second * scaleY); // invert Y for top-down grid

        // Clear screen
        std::cout << "\033[2J\033[H";

        // Draw grid with dot
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                if (r == row && c == col)
                    std::cout << "o"; // dot
                else
                    std::cout << ".";
            }
            std::cout << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}
int  main (int argc, char *argv[]) {
  //Drive this function for testing
  double x0 = 0, y0 = 1, x_max = 2 , h = 0.2;
  auto data = solve_range(x0, y0, x_max, h, dydx);
  animateSolution(data, 40, 20);
  //  for(auto &point : data){
  //  std::cout << "x = " << point.first << ", y = " << point.second << "\n";
  //}
  return 0;
}

