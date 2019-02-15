#ifndef DOTTED_SURFACE_GENERATOR_HPP
#define DOTTED_SURFACE_GENERATOR_HPP

#include "../types.hpp"
#include <vector>

namespace simploce {

  /**
   * Returns a dotted surface representation for a surface enclosing a collection of atoms.
   * @param nod - Number of dots per fully accessible sphere. Default value is 500.
   */
  Surface generateDottedSurface(const std::vector<Atom>& atoms, std::size_t nod = 500);
  
}

#endif
