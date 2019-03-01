#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/conf.hpp"
#include <stdexcept>
#include <string>

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
      std::clog << "WARNING: The triangulated surface's Euler characteristic is not equal to 2. "
		<< "Value is " << euler << "." << std::endl;
    }
    this->orientOutward();
  }

  void TriangulatedSurface::orientOutward()
  {
    for ( Triangle& triangle : triangles_ ) {
      // Get the vertices.
      std::tuple<vertex_ptr_t, vertex_ptr_t, vertex_ptr_t> vertices = triangle.vertices();

      // Create three position vectors on a unit sphere, each representing a vertex.
      position_t v1 = std::get<0>(vertices)->position();
      v1 /= norm<real_t>(v1);
      position_t v2 = std::get<1>(vertices)->position();
      v2 /= norm<real_t>(v2);
      position_t v3 = std::get<2>(vertices)->position();
      v3 /= norm<real_t>(v3);

      // Compute a normal vector.
      dist_vect_t r12 = v2 - v1;  // From 1 to 2.
      dist_vect_t r13 = v3 - v1;  // From 1 to 3.
      normal_t normal = cross(r12, r13);

      // Check orientation.
      normal_t aveNormal = (v1 + v2 + v3 ) / 3.0;
      real_t ip = inner<real_t>(normal,  aveNormal);
      if ( ip < 0.0 ) {
	// Pointing inward.
	triangle.swapTwoVertices();
      }
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

  tri_surface_ptr_t TriangulatedSurface::make(std::istream& stream)
  {
    std::vector<vertex_ptr_t> vertices{};
    std::vector<Triangle> triangles{};
    std::vector<Edge> edges{};
    
    // Buffer to read EOL.
    std::string buffer;

    // Vertices.
    std::size_t nve, id;
    stream >> nve;
    std::getline(stream, buffer);        // Reads EOL.
    real_t x,y,z;
    real_t nx, ny, nz;
    for (std::size_t i = 0; i != nve; ++i) {
      stream >> id >> x >> y >> z >> nx >> ny >> nz;
      position_t r{x, y, z};
      normal_t n{nx, ny, nz};
      vertices.push_back(Vertex::make(r, n));
      std::getline(stream, buffer);        // Reads EOL.
    }
    
    // Triangles.
    std::size_t ntr;
    stream >> ntr;
    for (std::size_t i = 0; i != ntr; ++i) {
      stream >> id;
      id -= 1;
      vertex_ptr_t v1 = vertices[id];
      stream >> id;
      id -= 1;
      vertex_ptr_t v2 = vertices[id];
      stream >> id;
      id -= 1;
      vertex_ptr_t v3 = vertices[id];
      triangles.push_back(Triangle{v1, v2, v3});
      std::getline(stream, buffer);        // Reads EOL.
    }

    // Edges.
    std::size_t nedges;
    stream >> nedges;
    for (std::size_t i = 0; i != nedges; ++i) {
      stream >> id;
      id -= 1;
      vertex_ptr_t v1 = vertices[id];
      stream >> id;
      id -= 1;
      vertex_ptr_t v2 = vertices[id];
      edges.push_back(Edge{v1, v2});
      std::getline(stream, buffer);        // Reads EOL.
    }
    
    return std::make_shared<TriangulatedSurface>(vertices, triangles, edges);
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
