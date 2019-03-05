#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Core>

using namespace Eigen;

typedef Matrix<double, Dynamic, Dynamic> matrix_t;

int main()
{
  matrix_t m(2,2);
  m(0,0) = 3;
  m(1,0) = 2.5;
  m(0,1) = -1;
  m(1,1) = m(1,0) + m(0,1);
  std::cout << m << std::endl;  
}
