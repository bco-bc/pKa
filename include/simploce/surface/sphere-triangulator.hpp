#ifndef SPHERE_TRIANGULATOR_HPP
#define SPHERE_TRIANGULATOR_HPP

#include "triangulator.hpp"

namespace simploce {

  /**
   * Relies on spherical triangulated surfaces to triangular an arbitrary set of surface points.
   */
  class SphereTriangulator : public Triangulator {
  public:

    /**
     * Constructor.
     *  @param ntriangles - Requested number of triangles. Should be one of 60, 240, or 960.
     */
    SphereTriangulator(std::size_t ntriangles);
    
    TriangulatedSurface generate(const std::vector<position_t>& points) const override;

    /**
     * Returns triangulator.
     * @param ntriangles - Requested number of triangles. Should be one of 60, 240, or 960.
     */
    static triangulator_ptr_t make(std::size_t ntriangles);

  private:

    std::size_t ntriangles_;
    
  };
}


#endif
