#ifndef DOTTED_SURFACE_GENERATOR_HPP
#define DOTTED_SURFACE_GENERATOR_HPP

#include "../types.hpp"
#include <vector>

namespace simploce {

  /**
   * Returns a dotted surface representation for a surface enclosing a collection of particles.
   * @param nod - Number of dots per fully accessible sphere. Default value is 100.
   */
  Surface generateDottedSurface(const std::vector<particle_t>& particles, std::size_t nod = 100);
}

#endif
