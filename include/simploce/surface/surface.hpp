#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "../types.hpp"
#include <vector>
#include <iostream>

namespace simploce {

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

    /**
     * Triangulate this surface.
     * @param triangulator - Triangulation algorithm.
     */
    void triangulate(const Triangulator& triangulator);

    bool isTriangulated();

    /**
     * Validate the surface, if possible.
     */
    void validate();

    /**
     * Returns total surface area.
     */
    area_t area() const;

    /**
     * Returns triangles.
     */
    const std::vector<Triangle>& triangles() const;

    /**
     * Returns edges.
     */
    const std::vector<Edge>& edges() const;

    /**
     * Writes this surface to an output stream.
     * @param stream Output stream.
     * @return Output stream.
     */
    std::ostream& writeTo(std::ostream& stream) const;

  private:

    std::vector<vertex_ptr_t> vertices_;
    std::vector<Triangle> triangles_;
    std::vector<Edge> edges_;
    bool triangulated_;
  };

  /**
   * Writes surface to an output stream.
   * @param stream - Output stream.
   * @param surface - Surface.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const Surface& surface);
}

#endif
