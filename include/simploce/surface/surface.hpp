#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "../types.hpp"
#include <vector>

namespace simploce {

  class Triangle;
  class Edge;

  /**
   * A surface consisting of vertices (surface points). It may also include edges and 
   * triangles, in which case the surface represents a triangulated surface.
   */
  class Surface {
  public:

    /**
     * Constructor.
     * @param vertices - Vertices. An internal copy is created.
     */
    Surface(const std::vector<vertex_t>& vertices);
    
    ~Surface();

    void triangulate();

    bool isTriangulated();

    /**
     * Validate the surface, if possible.
     */
    void validate();

  private:

    std::vector<vertex_ptr_t> vertices_;
    std::vector<Triangle> triangles_;
    std::vector<Edge> edges_;
    
  };
}

#endif
