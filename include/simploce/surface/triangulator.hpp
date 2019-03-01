#ifndef TRIANGULATOR_HPP
#define TRIANGULATOR_HPP

#include "../types.hpp"

namespace simploce {

  /**
   * Triangulates a set of points representing a dotted surface enclosing some object.
   */
  struct Triangulator {

    virtual ~Triangulator() {}

    /**
     * Creates a triangulated surface from surface points.
     * @param points - Surface points. The origin must inside the surface.
     * @param spherical - If true, the triangulated surface will be spherical.
     * @return Triangulated surface.
     */
    virtual TriangulatedSurface generate(const std::vector<position_t>& points,
					 bool spherical = false) const = 0;
  };
}

#endif
