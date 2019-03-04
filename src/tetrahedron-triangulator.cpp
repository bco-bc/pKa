#include "simploce/surface/tetrahedron-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/vertex.hpp"
#include <stdexcept>
#include <iostream>

namespace simploce {

  TriangulatedSurface
  TetrahedronTriangulator::generate(const std::vector<position_t>& points,
				    bool spherical,
				    const radius_t& radius) const
  {
    if ( points.size() != 4) {
      throw std::domain_error("Tetrahedron: Can only be applied to 4 surface points.");
    }
    std::clog << "WARNING: value of 'spherical' is ignored." << std::endl;
    std::clog << "WARNING: value of 'radius' is ignored." << std::endl;

    // 4 vertices.
    std::vector<vertex_ptr_t> vertices;
    for (auto p : points) {
      vertices.push_back(std::make_shared<vertex_t>(p));
    }

    // 6 edges.
    std::vector<Edge> edges;
    Edge e1{vertices[0], vertices[1]};
    Edge e2{vertices[0], vertices[2]};
    Edge e3{vertices[0], vertices[3]};
    Edge e4{vertices[1], vertices[2]};
    Edge e5{vertices[1], vertices[3]};
    Edge e6{vertices[2], vertices[3]};
    edges.push_back(e1);
    edges.push_back(e2);
    edges.push_back(e3);
    edges.push_back(e4);
    edges.push_back(e5);
    edges.push_back(e6);

    // 4 triangles.
    std::vector<Triangle> triangles;
    Triangle t1{vertices[0], vertices[1], vertices[2]};
    Triangle t2{vertices[0], vertices[1], vertices[3]};
    Triangle t3{vertices[0], vertices[2], vertices[3]};
    Triangle t4{vertices[1], vertices[2], vertices[3]};
    triangles.push_back(t1);
    triangles.push_back(t2);
    triangles.push_back(t3);
    triangles.push_back(t4);
        
    return TriangulatedSurface{vertices, triangles, edges};    
  }

  triangulator_ptr_t TetrahedronTriangulator::make()
  {
    return std::make_shared<TetrahedronTriangulator>();
  }
}
