#ifndef TETRAHEDRON_TRIANGULATOR_HPP
#define TETRAHEDRON_TRIANGULATOR_HPP

#include "triangulator.hpp"

namespace simploce {

  /**
   * Creates a simple tetrahedron.
   */
  struct TetrahedronTriangulator : public Triangulator {    

    /**
     * @param points - Surface points. Its size must be exactly 4.
     */
    TriangulatedSurface generate(const std::vector<position_t>& points) const;
    
  };
}

#endif
