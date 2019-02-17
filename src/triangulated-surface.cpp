#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/conf.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>

namespace simploce {

  static int eulerCharacteristic(const std::vector<vertex_ptr_t>& vertices,
				 const std::vector<Triangle>& triangles,
				 const std::vector<Edge>& edges)
  {
    return vertices.size() + triangles.size() - edges.size();
  }

  TriangulatedSurface::TriangulatedSurface(const std::vector<vertex_ptr_t>& vertices,
					   const std::vector<Triangle>& triangles,
					   const std::vector<Edge>& edges) :
    vertices_{vertices}, triangles_{triangles}, edges_{edges}
  {

    int euler = eulerCharacteristic(vertices_, triangles_, edges_);
    if ( euler != 2 ) {
      std::string v = boost::lexical_cast<std::string, int>(euler);
      std::string msg = 
	"TriangulatedSurface: Euler characteristic is not equals to 2. Value is " + v + ".";
      //throw std::domain_error(msg);
      std::clog << "WARNING. " << msg << std::endl;
    }
  }

  const std::vector<vertex_ptr_t> TriangulatedSurface::vertices() const
  {
    return vertices_;
  }

  const std::vector<Triangle>& TriangulatedSurface::triangles() const
  {
    return triangles_;
  }

  const std::vector<Edge>& TriangulatedSurface::edges() const
  {
    return edges_;
  }

  area_t TriangulatedSurface::area() const
  {
    area_t area{0};
    for (auto t : triangles_) {
      area += t.area();
    }
    return area;
  }
  
  std::ostream& TriangulatedSurface::writeTo(std::ostream& stream) const
  {
    stream.setf(std::ios::scientific);
    stream.precision(PRECISION);
    
    stream << vertices_.size() << std::endl;
    for (auto v : vertices_) {
      stream << *v << std::endl;
    }
    stream << triangles_.size() << std::endl;
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
  
    return stream;
  }

  std::ostream& operator << (std::ostream& stream, const TriangulatedSurface& surface)
  {
    return surface.writeTo(stream);
  }
}
