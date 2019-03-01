#include "simploce/surface/sphere-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/util/map2.hpp"
#include "simploce/conf.hpp"
#include "simploce/util/util.hpp"
#include "simploce/util/entity-range.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <tuple>
#include <cmath>
#include <utility>
#include <memory>
#include <cstdint>
#include <thread>
#include <future>

namespace simploce {

  using face_t = std::tuple<std::size_t, std::size_t, std::size_t, std::size_t, std::size_t>;
  using dodecahedron_t = std::pair<std::vector<vertex_ptr_t>, std::vector<face_t>>;

  using tri_t = std::tuple<std::size_t, std::size_t, std::size_t>;
  using tri_srf_t = std::pair<std::vector<vertex_ptr_t>, std::vector<tri_t>>;

  using entity_range_t = EntityRange<vertex_ptr_t>;

  struct MappingResult {
    MappingResult(std::vector<vertex_ptr_t> vertices,
		  const entity_range_t& range) : vertices_{vertices}, range_{range} {}
    std::vector<vertex_ptr_t> vertices_;
    entity_range_t range_;
  };
  
  static const std::size_t NA = SIZE_MAX;

  /**
   * @param vertices - Original set of vertices.
   * @param range - Range of vertices to be mapped/replaced by surface points.
   * @return Mapped vertices. Contains only mapped vertices.
   */
  static MappingResult mapper_(std::vector<vertex_ptr_t>& vertices,
			       const entity_range_t& range,
			       const std::vector<position_t>& points)
  {
    static const real_t MIN = 0.5 * std::sqrt(2);
    static const real_t MAX = 1.0;
    
    std::vector<vertex_ptr_t> mapped{};
    for (std::size_t k = range.start(); k != range.end(); ++k ) {
      bool isMapped{false};
      const vertex_ptr_t& vertex = vertices[k];
      position_t r = vertex->position();
      length_t R = norm<length_t>(r);
      position_t rMapped = r;
      length_t dmin2{LARGE};
      for (const position_t& point : points) {
	length_t Rp = norm<real_t>(point);
	real_t cosine = inner<real_t>(r, point)/(R * Rp);
	if ( cosine >= MIN && cosine <= MAX ) {
	  length_t b = Rp * cosine;
	  real_t d2 = Rp * Rp - b * b;
	  if ( d2 < dmin2 ) {
	    dmin2 = d2;
	    rMapped = point;
	    isMapped = true;
	  }
	}
      }
      if ( !isMapped ) {
	std::clog << "WARNING: Vertex with id " << vertex->id()
		  << " could not be mapped onto dotted surface." << std::endl;
      }
      vertex->reset(rMapped);
      mapped.push_back(vertex);
    }
    return MappingResult{vertices, range};
  }

  /**
   * Sets up the coordinates and faces (pentagons) of a dodecahedron.
   * @param radius - Radius of sphere. Vertices of the dodecahedron will be located on the surface
   * of the sphere.
   * @return Dodecahedron. The faces have the following interpretation: the 5 indices of a face
   * refer to 5 vertices.
   */
  static dodecahedron_t makeDodecahedron_(const radius_t& radius)
  {
    std::vector<vertex_ptr_t> vertices{};
    std::vector<face_t> faces{};
    
    // First compute the length of a side of a pentagon (a) and a cube (b).
    real_t a = 2.0 * radius / std::sqrt(3.0);
    real_t b = 4.0 * radius/ ( (1.0 + std::sqrt(5.0) ) * std::sqrt(3.0) );

    // Here, d is just for convenience.
    real_t d = b * std::sqrt( ( 3.0 -std::sqrt(5.0) ) / 8.0 );

    // 20 Vertices
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

    r[0] = -0.5 * a - 0.5 * b;
    r[1] = 0.5 * a - d;
    r[2] = 0;
    vertices.push_back(Vertex::make(r));   

    // 12 Faces. Indices refer to the elements of vertices.
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
   * Creates 60 triangles from the 12 faces of a dodecahedron. Will create 60 triangles, 32 
   * vertices, and 90 edges.

   */
  static tri_srf_t make60(const radius_t&radius, const dodecahedron_t& dodecahedron)
  {
    std::vector<vertex_ptr_t> vertices = dodecahedron.first;
    const std::vector<face_t> faces = dodecahedron.second;
    std::vector<tri_t> triangles{};

    // For all 12 pentagones
    for (const face_t& face : faces) {

      // Find the midpoint or center of the pentagon.
      static position_t center{};
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
      center *= radius/R;

      // Add new vertex.
      vertices.push_back(Vertex::make(center));
      std::size_t index = vertices.size() - 1;

      // Make 5 triangles.      
      triangles.push_back(std::make_tuple(i0, i1, index));      			    
      triangles.push_back(std::make_tuple(i1, i2, index));
      triangles.push_back(std::make_tuple(i2, i3, index));
      triangles.push_back(std::make_tuple(i3, i4, index));
      triangles.push_back(std::make_tuple(i4, i0, index));
    }

    std::clog << "Spherical triangulated surface:" << std::endl;
    std::clog << "Number of vertices: " << vertices.size() << std::endl;
    std::clog << "Number of triangles: " << triangles.size() << std::endl;
    
    return std::make_pair(vertices, triangles);
  }

  /**
   * Subdivides each edge of each triangle to generate 4 new triangles.
   */
  static tri_srf_t refine_(const radius_t& radius, const tri_srf_t& srf)
  {
    std::vector<vertex_ptr_t> vertices = std::get<0>(srf);
    const std::vector<tri_t> tris = std::get<1>(srf);
    std::vector<tri_t> triangles{};

    // Current number of vertices.
    std::size_t nve = vertices.size();

    // Keeps track of existing edges already split in two.
    boost::numeric::ublas::matrix<bool> split(nve, nve);
    for (std::size_t i = 0; i != nve; ++i) {
      for (std::size_t j = 0; j != nve; ++j) {
	split(i,j) = false;
      }
    }

    // Keeps track of assiged new vertex indices, a midpoint between two already existing vertices.
    boost::numeric::ublas::matrix<std::size_t> assigned(nve, nve);
    for (std::size_t i = 0; i != nve; ++i) {
      for (std::size_t j = 0; j != nve; ++j) {
	assigned(i,j) = NA;
      }
    }

    // Generate from each triangles four new triangles. Discards the existing one.
    for (std::size_t n = 0; n != tris.size(); ++n) {
      //    for (const tri_t& tri : tris) {

      const tri_t& tri = tris[n];

      // Get three vertex indices.
      std::size_t i0 = std::get<0>(tri);
      std::size_t i1 = std::get<1>(tri);
      std::size_t i2 = std::get<2>(tri);

      // Get three vertex positions.
      position_t r0 = vertices[i0]->position();
      position_t r1 = vertices[i1]->position();
      position_t r2 = vertices[i2]->position();

      // Add at most three more vertices. Midpoint of an old edge serves as a new vertex.      
      std::size_t i01, i12, i20;
      if ( !split(i0, i1) ) {
	// New midpoint. Create new vertex and corresponding vertex index.
	dist_vect_t d = r1 - r0;
	position_t r01 = r0 + d / 2.0;
	length_t R = norm<length_t>(r01);
	r01 *= radius/R;
	vertices.push_back(Vertex::make(r01));
	i01 = vertices.size() - 1;
	split(i0, i1) = true;
	split(i1,i0) = true;
	assigned(i0, i1) = i01;
	assigned(i1,i0) = i01;
      } else {
	// Existing midpoint. Get its vertex index.
	i01 = assigned(i0,i1);
      }
      if ( !split(i1,i2) ) {
	dist_vect_t d = r2 - r1;
	position_t r12 = r1 + d / 2.0;
	length_t R = norm<length_t>(r12);
	r12 *= radius/R;
	vertices.push_back(Vertex::make(r12));
	i12 = vertices.size() - 1;
	split(i1,i2) = true;
	split(i2,i1) = true;
	assigned(i1,i2) = i12;
	assigned(i2,i1) = i12;
      } else {
	i12 = assigned(i1, i2);
      }
      if ( !split(i2,i0) ) {
	dist_vect_t d = r0 - r2;
	position_t r20 = r2 + d / 2.0;
	length_t R = norm<length_t>(r20);
	r20 *= radius/R;
	vertices.push_back(Vertex::make(r20));
	i20 = vertices.size() - 1;
	split(i2,i0) = true;
	split(i0,i2) = true;
	assigned(i2,i0) = i20;
	assigned(i0,i2) = i20;
      } else {
	i20 = assigned(i2,i0);
      }

      // Form 4 new triangles.
      triangles.push_back(std::make_tuple(i0, i01, i20));
      triangles.push_back(std::make_tuple(i1, i12, i01));
      triangles.push_back(std::make_tuple(i2, i20, i12));
      triangles.push_back(std::make_tuple(i01, i12, i20));

    }

    std::clog << "Spherical triangulated surface:" << std::endl;
    std::clog << "Number of vertices: " << vertices.size() << std::endl;
    std::clog << "Number of triangles: " << triangles.size() << std::endl;
    
    // Done.
    return std::make_pair(vertices, triangles);     
  }

  static tri_srf_t mapToDottedSurface_(const tri_srf_t& srf, const std::vector<position_t>& points)
  {
    std::clog << "Mapping spherical triangulated surface onto dotted surface..." << std::endl;
    
    std::vector<vertex_ptr_t> vertices = std::get<0>(srf);
    const std::vector<tri_t> tris = std::get<1>(srf);
    
    std::vector<entity_range_t> ranges = createRanges(vertices);
    std::vector<std::future<MappingResult>> futures{};
    std::size_t ntasks = ranges.size() - 1;
    std::clog << "Number of concurrent tasks: " << ntasks << std::endl;
    for (std::size_t k = 0; k != ntasks; ++k) {
      const entity_range_t& range = ranges[k];
      futures.push_back(std::async(std::launch::async, 
                                   mapper_,
                                   std::ref(vertices),
                                   std::ref(range),
				   std::ref(points)
                                   ));
    }
    // Wait for these tasks to complete in other threads.
    std::vector<MappingResult> results = wait_for_all<MappingResult>(futures);

    // Current thread.
    const entity_range_t& range = ranges[ranges.size() - 1];
    MappingResult result = mapper_(vertices, range, points);
    results.push_back(result);

    // Replace vertices.
    for (auto& result : results) {
      const std::vector<vertex_ptr_t>& vptrs = result.vertices_;
      const entity_range_t& range = result.range_;
      for (std::size_t k = range.start(); k != range.end(); k++) {
	vertex_ptr_t ptr = vptrs[k];
	std::size_t index = ptr->id() - 1;
	vertices[index] = ptr;
      }
    }

    // Done.
    std::clog << "Done." << std::endl;
    
    return std::make_pair(vertices, tris);
  }
  
  /**
   * Creates final triangulated surface.
   */
  static TriangulatedSurface makeFrom_(const tri_srf_t& srf)
  {
    // Triangulated surface.
    const std::vector<vertex_ptr_t> vertices = std::get<0>(srf);
    const std::vector<tri_t>& tris = std::get<1>(srf);

    // Edges created.
    std::size_t nve = vertices.size();
    boost::numeric::ublas::matrix<bool> created(nve, nve); // At most. The actual number is lower.
    for (std::size_t i = 0; i != nve; ++i) {
      for (std::size_t j = 0; j != nve; ++j) {
	created(i,j) = false;
      }
    }    

    std::vector<Triangle> triangles{};
    std::vector<Edge> edges{};
    for (const auto& tri : tris) {
      std::size_t i0 = std::get<0>(tri);
      std::size_t i1 = std::get<1>(tri);
      std::size_t i2 = std::get<2>(tri);
      Triangle triangle{vertices[i0], vertices[i1], vertices[i2]};
      triangles.push_back(triangle);
      
      if ( !created(i0,i1) ) {
	edges.push_back(Edge{vertices[i0], vertices[i1]});
	created(i0,i1) = true;
	created(i1,i0) = true;
      }
      
      if ( !created(i1,i2) ) {
	edges.push_back(Edge{vertices[i1], vertices[i2]});
	created(i1,i2) = true;
	created(i2,i1) = true;
      }
      
      if ( !created(i2,i0) ) {
	edges.push_back(Edge{vertices[i2], vertices[i0]});
	created(i2,i0) = true;
	created(i0,i2) = true;
      }
      
    }

    std::clog << "Final triangulated surface: " << std::endl;
    std::clog << "Number of vertices: " << vertices.size() << std::endl;
    std::clog << "Number of triangles: " << triangles.size() << std::endl;
    std::clog << "Number of edges: " << edges.size() << std::endl;
    
    return TriangulatedSurface{vertices, triangles, edges};
  }
  

  /**
   * Returns a triangulated surface.
   * @param radius - Radius in nm.
   * @param ntriangles - Desired number of triangles. Default is 240.
   * @param spherical - If true, the returned surface will be spherical. Default is false.
   */
  static TriangulatedSurface generateTriangulatedSurface_(const std::vector<position_t>& points,
							  const radius_t& radius,
							  std::size_t ntriangles = 240,
							  bool spherical = false)
  {
    dodecahedron_t dodecahedron = makeDodecahedron_(radius);
    tri_srf_t srf = make60(radius, dodecahedron);
    std::size_t ntr = std::get<1>(srf).size();
    while (ntr < ntriangles) {
      srf = refine_(radius, srf);
      ntr = std::get<1>(srf).size();
    }
    if ( !spherical) {
      srf =  mapToDottedSurface_(srf, points);
    }
    return makeFrom_(srf);
  }

  SphereTriangulator::SphereTriangulator(std::size_t ntriangles) :
    ntriangles_{ntriangles}
  {
    if ( ntriangles_ < 60 ) {
      throw std::domain_error("Can only generate 60 or more triangles.");
    }
  }

  TriangulatedSurface SphereTriangulator::generate(const std::vector<position_t>& points,
						   bool spherical) const
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
    return generateTriangulatedSurface_(points, radius, ntriangles_, spherical);
  }

  triangulator_ptr_t SphereTriangulator::make(std::size_t ntriangles)
  {
    return std::make_shared<SphereTriangulator>(ntriangles);
  }
  
}
