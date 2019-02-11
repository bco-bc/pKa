#ifndef TRIANGULATOR_HPP
#define TRIANGULATOR_HPP

#include "../types.hpp"

namespace simploce {

  /**
   * Triangulates a collection of points on a surface enclosing some protein system.
   */
  struct Triangulator {

    virtual ~Triangulator() {}

    /**
     * Creates a triangulated surface from surface points.
     * @param points - Surface points.
     * @return Triangulated surface.
     */
    virtual TriangulatedSurface generate(const std::vector<position_t>& points) const = 0;
  };
}

#endif
