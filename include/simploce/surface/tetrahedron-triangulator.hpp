#ifndef TETRAHEDRON_TRIANGULATOR_HPP
#define TETRAHEDRON_TRIANGULATOR_HPP

#include "triangulator.hpp"

namespace simploce {

  /**
   * Creates a simple tetrahedron.
   */
  struct TetrahedronTriangulator : public Triangulator {    

    TriangulatedSurface generate(const std::vector<position_t>& points,
				 bool spherical = false,
				 const radius_t& radius = 1.0) const;

    /**
     * Returns triangulator.
     */
    static triangulator_ptr_t make();
    
  };
}

#endif
