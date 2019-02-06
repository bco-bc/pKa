#include "simploce/surface/surface.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"
#include <stdexcept>
#include <iostream>

namespace simploce {

  static std::pair<std::vector<Triangle>,std::vector<Edge>>
    makeTetrahedron(const std::vector<vertex_ptr_t>& vertices)
  {
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

  Surface::Surface(const std::vector<vertex_t>& vertices) :
    vertices_{}, triangles_{}, edges_{}
  {
    for (auto v : vertices) {
      vertex_ptr_t vertex = std::make_shared<vertex_t>(v);
      vertices_.push_back(vertex);
    }
    if ( vertices_.empty() ) {
      throw std::domain_error("Surface: vertices must be provided.");
    }
  }

  Surface::~Surface()
  {
  }

  void Surface::triangulate()
  {
    if ( vertices_.size() < 4) {
      throw std::domain_error("Triangulated surface: a minimum of 4 vertices are required.");
    }
    if ( vertices_.size() == 4) {
      auto pair = makeTetrahedron(vertices_);
      triangles_ = pair.first;
      edges_ = pair.second;
    }
    this->validate();
  }

  bool Surface::isTriangulated()
  {
    return false;
  }

  void Surface::validate()
  {
    if ( this->isTriangulated() ) {
      std::size_t ec = vertices_.size() + triangles_.size() - edges_.size();
      if ( ec != 2 ) {
	throw std::domain_error("Triangulated surface: Euler characteristic != 2.");
      }
    }
  }
			 
}
