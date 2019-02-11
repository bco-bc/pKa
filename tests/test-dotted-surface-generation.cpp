#include "simploce/surface/dotted-surface-generator.hpp"
#include "simploce/surface/surface.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/tetrahedron-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"

using namespace simploce;

int main()
{
  // Regular tetrahedron with side length 2.
  std::clog << "Four Edge points of regular tetrahedron:" << std::endl;
  real_t el = 2.0;
  std::clog << "Edge length: " << el << std::endl;
  real_t c = 1.0/std::sqrt(2.0);
  std::clog << "Value of c: " << c << std::endl;

  std::vector<particle_t> particles;
  particle_t p1 = std::make_tuple(position_t{-1.0, 0.0, -c}, 1.5, 0.0);
  particles.push_back(p1);
  particle_t p2 = std::make_tuple(position_t{+1.0, 0.0, -c}, 1.5, 0.0);
  particles.push_back(p2);
  particle_t p3 = std::make_tuple(position_t{0.0, +1.0, -c}, 1.5, 0.0);
  particles.push_back(p3);
  particle_t p4 = std::make_tuple(position_t{0.0, -1.0, -c}, 1.5, 0.0);
  particles.push_back(p4);

  Surface surface = generateDottedSurface(particles, 600);
  std::cout << surface << std::endl;

  return 0;
}
