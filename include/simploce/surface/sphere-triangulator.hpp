#ifndef SPHERE_TRIANGULATOR_HPP
#define SPHERE_TRIANGULATOR_HPP

#include "triangulator.hpp"

namespace simploce {

  /**
   * Relies on spherical triangulated surfaces to triangular an arbitrary set of surface points. It
   * will try to match the requested number of triangles as closely as possible.
   */
  class SphereTriangulator : public Triangulator {
  public:

    /**
     * Constructor.
     *  @param ntriangles - Requested number of triangles. Typical values includes multiples of 60.
     */
    SphereTriangulator(std::size_t ntriangles);
    
    TriangulatedSurface generate(const std::vector<position_t>& points,
				 bool spherical = false,
				 const radius_t& radius = 1.0) const override;

    /**
     * Returns triangulator.
     * @param ntriangles - Requested number of triangles. Typical values includes multiples of 60.
     */
    static triangulator_ptr_t make(std::size_t ntriangles);

  private:

    std::size_t ntriangles_;
    
  };
}


#endif
