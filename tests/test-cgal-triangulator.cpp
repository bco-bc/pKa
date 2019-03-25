#include "simploce/surface/surface.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/cgal-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/types.hpp"
#include <cmath>
#include <vector>
#include <iostream>

using namespace simploce;

int main()
{
    // Regular tetrahedron with side length 2.
    // std::cout << "Four Edge points of regular tetrahedron:" << std::endl;
    // real_t el = 2.0;
    // std::cout << "Edge length: " << el << std::endl;
    // real_t c = 1.0/std::sqrt(2.0);
    // std::vector<position_t> points;
    // points.push_back(position_t{-1.0, 0.0, -c});
    // points.push_back(position_t{+1.0, 0.0, -c});
    // points.push_back(position_t{0.0, +1.0, +c});
    // points.push_back(position_t{0.0, -1.0, +c});
    // Surface surface{points};
    // std::cout << surface << std::endl;

    // Regular Cube with side length 2.
    std::cout << "Eight Edge points of cube with central point:" << std::endl;
    real_t el = 2.0;
    std::cout << "Edge length: " << el << std::endl;
    std::vector<position_t> points;
    points.push_back(position_t{-1.0, -1.0, -1.0});
    points.push_back(position_t{+1.0, -1.0, -1.0});
    points.push_back(position_t{-1.0, +1.0, -1.0});
    points.push_back(position_t{+1.0, +1.0, -1.0});
    points.push_back(position_t{-1.0, -1.0, +1.0});
    points.push_back(position_t{+1.0, -1.0, +1.0});
    points.push_back(position_t{-1.0, +1.0, +1.0});
    points.push_back(position_t{+1.0, +1.0, +1.0});
    points.push_back(position_t{+0.0, +0.0, +0.0});
    Surface surface{points};
    std::cout << surface << std::endl;

    triangulator_ptr_t triangulator = CgalTriangulator::make();
    TriangulatedSurface tsurface = surface.triangulate(triangulator);
    std::cout << std::endl;
    std::cout << "Surface is triangulated:    " << std::endl;
    std::cout << tsurface << std::endl;  
    std::cout << "Triangulated surface area:   " << tsurface.area() << std::endl;
    std::cout << "Surface cube: " << 6 * el * el << std::endl;

    std::cout << std::endl;
    std::cout << "Triangle normals:" << std::endl;
    for (const auto& t : tsurface.triangles()) {
	normal_t n = t.normal();
	std::cout << n << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Edge lengths:" << std::endl;
    for (const auto& e : tsurface.edges()) {
	std::cout << e.length() << std::endl;
    }
  
    return 0;
}
