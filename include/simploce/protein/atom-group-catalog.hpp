#ifndef ATOM_GROUP_CATALOG_HPP
#define ATOM_GROUP_CATALOG_HPP

#include "../types.hpp"
#include <string>
#include <vector>

namespace simploce {

  class AtomGroupCatalog {
  public:

    /**
     * Returns the maximum number of bound protons.
     * @param name - Atom group specification name.
     * @return Maximum number.
     */
    std::size_t maximumProtonOccupancy(const std::string& name) const;

    /**
     * Looks up an atom group specification with a given protonation state.
     * @param protonationState - Number of bound protons: 0, 1, ..
     * @param name - Atom group specification name.
     */
    atom_group_spec_ptr_t lookup(std::size_t protonationState, const std::string& name) const;    

    /**
     * Returns atom group catalog.
     */
    static atom_group_catalog_ptr_t make();
  };
}

#endif
