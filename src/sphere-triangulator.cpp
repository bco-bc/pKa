#include "simploce/surface/sphere-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/util/map2.hpp"
#include "simploce/conf.hpp"
#include <tuple>
#include <cmath>
#include <utility>
#include <memory>

namespace simploce {

  using face_t = std::tuple<std::size_t, std::size_t, std::size_t, std::size_t, std::size_t>;
  using dodecahedron_t = std::pair<std::vector<vertex_ptr_t>, std::vector<face_t>>;

  using tri_t = std::tuple<std::size_t, std::size_t, std::size_t>;
  using edges_t = MatrixMap<std::size_t, std::size_t>;
  using tri_srf_t = std::tuple<std::vector<vertex_ptr_t>, std::vector<tri_t>, edges_t>;
  
  /**
   * Sets up the coordinates and faces (pentagons) of a dodecahedron.
   * @param radius - Radius of sphere. Vertices of the dodecahedron will be located on the surface
   * of the sphere.
   * @return Dodecahedron. The faces have the following interpretation: the 5 indices of a face
   * refer to 5 vertices.
   */
  static dodecahedron_t makeDodecahedron_(const radius_t& radius)
  {
    std::vector<vertex_ptr_t> vertices;
    std::vector<face_t> faces;
    
    // First compute the length of a side of a pentagon (a) and a cube (b).
    real_t a = 2.0 * radius / std::sqrt(3.0);
    real_t b = 4.0 * radius/ ( (1.0 + std::sqrt(5.0) ) * std::sqrt(3.0) );

    // Here, d is just for convenience.
    real_t d = b * std::sqrt( ( 3.0 -std::sqrt(5.0) ) / 8.0 );

    // Vertices
    position_t r;
    r[0] = 0.5 * a - d;
    r[1] = 0;
    r[2] = 0.5 * a + 0.5 * b;
    vertices.push_back(Vertex::make(r));
    r[0] = 0.5 * a;
    r[1] = 0.5 * a;
    r[2] = 0.5 * a;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0;
    r[1] = 0.5 * a + 0.5 * b;
    r[2] = 0.5 * a - d;
    vertices.push_back(Vertex::make(r)); 
    r[0] = -0.5 * a;
    r[1] = 0.5 * a;
    r[2] = 0.5 * a;
    vertices.push_back(Vertex::make(r)); 
    r[0] = -0.5 * a + d;
    r[1] = 0;
    r[2] = 0.5 * a + 0.5 * b;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0;
    r[1] = 0.5 * a + 0.5 * b;
    r[2] = -0.5 * a + d;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0.5 * a;
    r[1] = 0.5 * a;
    r[2] = -0.5 * a;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0.5 * a - d;
    r[1] = 0;
    r[2] = -0.5 * a - 0.5 * b;
    vertices.push_back(Vertex::make(r)); 
    r[0] = -0.5 * a + d;
    r[1] = 0;
    r[2] = -0.5 * a - 0.5 * b;
    vertices.push_back(Vertex::make(r)); 
    r[0] = -0.5 * a;
    r[1] = 0.5 * a;
    r[2] = -0.5 * a;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0.5 * a;
    r[1] = -0.5 * a;
    r[2] = -0.5 * a;
    vertices.push_back(Vertex::make(r)); 
    r[0] = -0.5 * a;
    r[1] = -0.5 * a;
    r[2] = -0.5 * a;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0;
    r[1] = -0.5 * a - 0.5 * b;
    r[2] = -0.5 * a + d;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0.5 * a;
    r[1] = -0.5 * a;
    r[2] = 0.5 * a;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0;
    r[1] = -0.5 * a - 0.5 * b;
    r[2] = 0.5 * a - d;
    vertices.push_back(Vertex::make(r)); 
    r[0] = -0.5 * a;
    r[1] = -0.5 * a;
    r[2] = 0.5 * a;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0.5 * a + 0.5 * b;
    r[1] = -0.5 * a + d;
    r[2] = 0;
    vertices.push_back(Vertex::make(r)); 
    r[0] = 0.5 * a + 0.5 * b;
    r[1] = 0.5 * a - d;
    r[2] = 0;
    vertices.push_back(Vertex::make(r)); 
    r[0] = -0.5 * a - 0.5 * b;
    r[1] = -0.5 * a + d;
    r[2] = 0;
    vertices.push_back(Vertex::make(r)); 
    r[0] = -0.5 * a - 0.5 *b;
    r[1] = 0.5 * a - d;
    r[2] = 0;
    vertices.push_back(Vertex::make(r));

    // Faces
    faces.push_back(std::make_tuple(0,1,2,3,4));
    faces.push_back(std::make_tuple(4,3,19,18,15));
    faces.push_back(std::make_tuple(4,15,14,13,0));
    faces.push_back(std::make_tuple(0,13,16,17,1));
    faces.push_back(std::make_tuple(7,6,5,9,8));
    faces.push_back(std::make_tuple(8,9,19,18,11));
    faces.push_back(std::make_tuple(8,11,12,10,7));
    faces.push_back(std::make_tuple(7,10,16,17,6));
    faces.push_back(std::make_tuple(17,6,5,2,1));
    faces.push_back(std::make_tuple(5,9,19,3,2));
    faces.push_back(std::make_tuple(16,13,14,12,10));
    faces.push_back(std::make_tuple(12,11,18,15,14));

    std::clog << "Dodecahedron:" << std::endl;
    std::clog << "Number of vertices: " << vertices.size() << std::endl;
    std::clog << "Number of faces: " << faces.size() << std::endl;

    return std::make_pair(vertices, faces);
  }

  /**
   * Creates 60 triangles from the 12 faces of a dodecahedron.
   */
  static tri_srf_t make60(const radius_t& radius, const dodecahedron_t& dodecahedron)
  {
    std::vector<vertex_ptr_t> vertices = dodecahedron.first;
    const std::vector<face_t>& faces = dodecahedron.second;
    std::vector<tri_t> triangles;
    edges_t edges;

    // For all 12 pentagones
    for (const face_t& face : faces) {

      // Find the midpoint or center of the pentagon.
      position_t center;
      std::size_t i0 = std::get<0>(face);
      std::size_t i1 = std::get<1>(face);
      std::size_t i2 = std::get<2>(face);
      std::size_t i3 = std::get<3>(face);
      std::size_t i4 = std::get<4>(face);
      position_t r0 = vertices[i0]->position();
      position_t r1 = vertices[i1]->position();
      position_t r2 = vertices[i2]->position();
      position_t r3 = vertices[i3]->position();
      position_t r4 = vertices[i4]->position();
      center = (r0 + r1 + r2 + r3 + r4) / 5.0;
      length_t R = norm<length_t>(center);
      center *= radius / R;

      // Add new vertex.
      vertices.push_back(Vertex::make(center));
      std::size_t index = vertices.size() - 1;

      // Make 5 triangles.      
      triangles.push_back(std::make_tuple(i0, i1, index));      			    
      triangles.push_back(std::make_tuple(i1, i2, index));
      triangles.push_back(std::make_tuple(i2, 13, index));
      triangles.push_back(std::make_tuple(i3, 14, index));
      triangles.push_back(std::make_tuple(i4, i0, index));

      // Add edges, if nonexistent. The map value is always 1.
      if ( !edges.contains(i0, i1) && !edges.contains(i1, i0) ) {
	edges.add(i0, i1, 1);
      }
      if (!edges.contains(i1, index) && !edges.contains(index, i1)) {
	edges.add(i1, index, 1);
      }
      if (!edges.contains(index, i0) && !edges.contains(i0, index)) {
	edges.add(index, i0, 1);
      }
      
      if ( !edges.contains(i1, i2) && !edges.contains(i2, i1) ) {
	edges.add(i1, i2, 1);
      }
      if (!edges.contains(i2, index) && !edges.contains(index, i2)) {
	edges.add(i2, index, 1);
      }
      if (!edges.contains(index, i1) && !edges.contains(i1, index)) {
	edges.add(index, i1, 1);
      }
      
      if ( !edges.contains(i2, i3) && !edges.contains(i3, i2) ) {
	edges.add(i2, i3, 1);
      }
      if (!edges.contains(i3, index) && !edges.contains(index, i3)) {
	edges.add(i3, index, 1);
      }
      if (!edges.contains(index, i2) && !edges.contains(i2, index)) {
	edges.add(index, i2, 1);
      }
      
      if ( !edges.contains(i3, i4) && !edges.contains(i4, i3) ) {
	edges.add(i3, i4, 1);
      }
      if (!edges.contains(i4, index) && !edges.contains(index, i4)) {
	edges.add(i4, index, 1);
      }
      if (!edges.contains(index, i3) && !edges.contains(i3, index)) {
	edges.add(index, i3, 1);
      }
      
      if ( !edges.contains(i4, i0) && !edges.contains(i0, i4) ) {
	edges.add(i4, i0, 1);
      }
      if (!edges.contains(i0, index) && !edges.contains(index, i0)) {
	edges.add(i0, index, 1);
      }
      if (!edges.contains(index, i4) && !edges.contains(i4, index)) {
	edges.add(index, i4, 1);
      }
      
    }

    std::clog << "Spherical triangulated surface:" << std::endl;
    std::clog << "Number of vertices: " << vertices.size() << std::endl;
    std::clog << "Number of triangles: " << triangles.size() << std::endl;
    std::clog << "Number of edges: " << edges.keyPairs().size() << std::endl;
    
    return std::make_tuple(vertices, triangles, edges);
  }

  static TriangulatedSurface makeFrom_(const tri_srf_t& srf)
  {
    const std::vector<vertex_ptr_t> vertices = std::get<0>(srf);
    const std::vector<tri_t>& tris = std::get<1>(srf);
    const edges_t& sides = std::get<2>(srf);

    std::vector<Triangle> triangles;
    for (const auto& tri : tris) {
      std::size_t i0 = std::get<0>(tri);
      std::size_t i1 = std::get<0>(tri);
      std::size_t i2 = std::get<0>(tri);
      Triangle triangle{vertices[i0], vertices[i1], vertices[i2]};
      triangles.push_back(triangle);
    }
    std::vector<Edge> edges;
    std::vector<std::pair<std::size_t, std::size_t>> pairs = sides.keyPairs();
    for (const auto& pair : pairs) {
      std::size_t i0 = pair.first;
      std::size_t i1 = pair.second;
      Edge edge{vertices[i0], vertices[i1]};
      edges.push_back(edge);
    }

    return TriangulatedSurface{vertices, triangles, edges};
  }
  

  /**
   * Returns a spherical triangulated surface.
   * @param radius - Radius in nm.
   * @param ntriangles - Desired number of triangles. Either 60, 240 or 960. Default is 60.
   */
  static TriangulatedSurface generateSphericalTriangulatedSurface_(const radius_t& radius,
								   std::size_t ntriangles = 60)
  {
    dodecahedron_t dodecahedron = makeDodecahedron_(radius);
    tri_srf_t srf = make60(radius, dodecahedron);
    return makeFrom_(srf);
  }

  SphereTriangulator::SphereTriangulator(std::size_t ntriangles) :
    ntriangles_{ntriangles}
  {
    if ( !(ntriangles_ == 60 || ntriangles_ == 240 || ntriangles_ == 960) ) {
      throw std::domain_error("Can only generate 60, 240, or 960 triangles.");
    }
  }

  TriangulatedSurface SphereTriangulator::generate(const std::vector<position_t>& points) const
  {
    radius_t radius{0};
    for (position_t r : points) {
      radius_t R = norm<radius_t>(r);
      if ( R > radius ) {
	radius = R;
      }
    }

    // Add water radius (specified in conf.hpp).
    radius += R_WATER;
    return generateSphericalTriangulatedSurface_(radius, ntriangles_);
  }

  triangulator_ptr_t SphereTriangulator::make(std::size_t ntriangles)
  {
    return std::make_shared<SphereTriangulator>(ntriangles);
  }
  
}
