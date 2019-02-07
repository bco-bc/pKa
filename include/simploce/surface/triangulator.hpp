#ifndef TRIANGULATOR_HPP
#define TRIANGULATOR_HPP

#include "../types.hpp"
#include <utility>

namespace simploce {

  /**
   * Triangulates a collection of points on a surface enclosing some protein system.
   */
  struct Triangulator {

    virtual ~Triangulator() {}

    /**
     * Return type of triangulator.
     */
    using result_t = std::pair<std::vector<Triangle>, std::vector<Edge>>;

    /**
     * Creates a triangulated surface from vertices.
     * @param vertices - Vertices. Should be enclosing some object.
     * @return Triangles and edges.
     */
    virtual result_t generate(std::vector<vertex_ptr_t>& vertices) const = 0;
  };
}

#endif
