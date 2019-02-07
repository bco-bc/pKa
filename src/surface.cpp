#include "simploce/surface/surface.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangulator.hpp"
#include <stdexcept>
#include <iostream>

namespace simploce {
 
  Surface::Surface(const std::vector<vertex_t>& vertices) :
    vertices_{}, triangles_{}, edges_{}, triangulated_{false}
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

  void Surface::triangulate(const Triangulator& triangulator)
  {
    if ( vertices_.size() < 4) {
      throw std::domain_error("Triangulated surface: a minimum of 4 vertices are required.");
    }
    
    // Triangulate.
    auto pair = triangulator.generate(vertices_);
    triangles_ = pair.first;
    edges_ = pair.second;

    // Check.
    this->validate();

    triangulated_ = true;
  }

  bool Surface::isTriangulated()
  {
    return triangulated_;
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

  area_t Surface::area() const
  {
    area_t area{0};
    for (auto t : triangles_) {
      area += t.area();
    }
    return area;
  }

  const std::vector<Edge>& Surface::edges() const
  {
    return edges_;
  }

  const std::vector<Triangle>& Surface::triangles() const
  {
    return triangles_;
  }

  std::ostream& Surface::writeTo(std::ostream& stream) const
  {
    stream.setf(std::ios::scientific);
    stream.precision(PRECISION);
    stream << vertices_.size() << std::endl;
    for (auto v : vertices_) {
      stream << *v << std::endl;
    }
    stream << triangles_.size();
    if ( triangles_.size() > 0 ) {
      stream << std::endl;
      for (auto t : triangles_) {
	stream << t << std::endl;
      }
      stream << edges_.size() << std::endl;
      for (auto iter = edges_.begin(); iter != edges_.end() - 1; ++iter) {
	const Edge& e = *iter;
	stream << e << std::endl;
      }
      const Edge& e = *(edges_.end() - 1);
      stream << e;
    }

    return stream;
  }

  std::ostream& operator << (std::ostream& stream, const Surface& surface)
  {
    return surface.writeTo(stream);
  }
			 
}
