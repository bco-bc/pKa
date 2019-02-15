#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "../types.hpp"
#include <vector>
#include <iostream>

namespace simploce {

  /**
   * A surface represented by a collection of surface points (a dotted surface).
   */
  class Surface {
  public:

    /**
     * Constructor.
     * @param points - Surface points.
     */
    Surface(const std::vector<position_t>& points);

    /**
     * Constructor.
     * @param points - Surface points (dotted surface representation).
     * @param area - Area of surface.
     * @param volume - Volume of surface.
     */
    Surface(std::vector<position_t>& points, const area_t& area, const volume_t& volume);

    /**
     * Triangulate this surface.
     * @param triangulator - Triangulation algorithme.
     * @return Triangulated surface.
     */
    TriangulatedSurface triangulate(const triangulator_ptr_t& triangulator);

    /**
     * Returns surface area.
     * @return Total area, or 0 if not provided.
     */
    area_t area() const;

    /**
     * Returns volume enclosed by surface.
     * @return volume, or 0 if not provided.
     */
    volume_t volume() const;

    /**
     * Writes this surface to an output stream.
     * @param stream Output stream.
     * @return Output stream.
     */
    std::ostream& writeTo(std::ostream& stream) const;

  private:

    std::vector<position_t> points_;
    area_t area_;
    volume_t volume_;

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
