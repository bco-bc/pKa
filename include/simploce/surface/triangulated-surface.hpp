#ifndef TRIANGULATED_SURFACE_HPP
#define TRIANGULATED_SURFACE_HPP

#include "../types.hpp"
#include <vector>
#include <iostream>

namespace simploce {

  /**
   * Triangulated surface. The value of Euler charactersistic is 2.
   */
  class TriangulatedSurface {
  public:

    /**
     * Constructor. The arguments must satify the Euler charactersistic of 2.
     * @param vertices - Triangle vertices.
     * @param triangles - Triangles.
     * @param edges - Triangle edges.
     */
    TriangulatedSurface(const std::vector<vertex_ptr_t>& vertices,
			const std::vector<Triangle>& triangles,
			const std::vector<Edge>& edges);

    /**
     * Ensure that normals point outward.
     */
    void orientOutward();

    /**
     * Returns vertices.
     */
    const std::vector<vertex_ptr_t> vertices() const;

    /**
     * Returns triangles.
     */
    const std::vector<Triangle>& triangles() const;

    /**
     * Returns edges.
     */
    const std::vector<Edge>& edges() const;

    /**
     * Returns surface area.
     */
    area_t area() const;

    /**
     * Writes this triangulated surface to an output stream.
     * @param stream - Output stream.
     * @return Output stream.
     */
    std::ostream& writeTo(std::ostream& stream) const;

    /**
     * Takes (reads) a triangulated surface from input stream.
     */
    static tri_surface_ptr_t make(std::istream& stream);

  private:
    
    std::vector<vertex_ptr_t> vertices_;
    std::vector<Triangle> triangles_;
    std::vector<Edge> edges_;
  };

  /**
   * Writes triangulated surface to an output stream.
   * @param stream - Output stream.
   * @param surface - Surface.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const TriangulatedSurface& surface);

}

#endif
