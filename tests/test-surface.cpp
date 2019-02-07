#include "simploce/surface/surface.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/tetrahedron-triangulator.hpp"
#include "simploce/types.hpp"
#include <cmath>
#include <vector>
#include <iostream>

using namespace simploce;

int main()
{
  // Regular tetrahedron with side length 2.
  std::cout << "Regular tetrahedron:" << std::endl;
  real_t el = 2.0;
  real_t c = 1.0/std::sqrt(2.0);
  std::vector<vertex_t> vertices;
  position_t r1{-1.0, 0.0, -c};
  position_t r2{+1.0, 0.0, -c};
  position_t r3{0.0, +1.0, +c};
  position_t r4{0.0, -1.0, +c};
  vertices.push_back(vertex_t{r1});
  vertices.push_back(vertex_t{r2});
  vertices.push_back(vertex_t{r3});
  vertices.push_back(vertex_t{r4});
  std::cout << "Edge length: " << el << std::endl;

  Surface surface{vertices};

  std::cout << std::endl;
  std::cout << "Surface regular tetrahedron is not triangulated:" << std::endl;
  std::cout << surface << std::endl;

  TetrahedronTriangulator tri;
  surface.triangulate(tri);
  std::cout << std::endl;
  std::cout << "Surface is triangulated:    " << std::endl;
  std::cout << surface << std::endl;  
  std::cout << "Triangulated surface area:   " << surface.area() << std::endl;
  std::cout << "Surface regular tetrahedron: " << std::sqrt(3.0) * el * el << std::endl;

  std::cout << std::endl;
  std::cout << "Triangle normals:" << std::endl;
  for (const auto& t : surface.triangles()) {
    normal_t n = t.normal();
    std::cout << n << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Edge lengths:" << std::endl;
  for (const auto& e : surface.edges()) {
    std::cout << e.length() << std::endl;
  }
  
  return 0;
}
