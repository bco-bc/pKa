#include "simploce/surface/vertex.hpp"
#include "simploce/types.hpp"
#include <iostream>

using namespace simploce;

int main()
{
  position_t r1{1.0, 2.0, 3.0};
  Vertex v1{r1};
  std::cout << v1 <<std::endl;

  position_t r2{-1.0, -2.0, -3.0};
  normal_t n2{0.0, 0.0, 1.00};
  Vertex v2{r2, n2};
  std::cout << v2 << std::endl;
  return 0;
}
