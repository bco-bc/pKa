#include "simploce/surface/tetrahedron-triangulator.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"

namespace simploce {

  using result_t = Triangulator::result_t;

  result_t TetrahedronTriangulator::generate(std::vector<vertex_ptr_t>& vertices) const
  {
    if ( vertices.size() != 4) {
      throw std::domain_error("Tetrahedron: Can only be applied to 4 vertices.");
    }
    
    std::vector<Triangle> triangles;
    std::vector<Edge> edges;

    // 6 edges.
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
    Triangle t1{vertices[0], vertices[1], vertices[2]};
    Triangle t2{vertices[0], vertices[1], vertices[3]};
    Triangle t3{vertices[0], vertices[2], vertices[3]};
    Triangle t4{vertices[1], vertices[2], vertices[3]};
    triangles.push_back(t1);
    triangles.push_back(t2);
    triangles.push_back(t3);
    triangles.push_back(t4);
        
    return std::make_pair(triangles, edges);
    
  }
}
