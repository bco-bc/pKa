#ifndef ATOM_GROUP_CATALOG_HPP
#define ATOM_GROUP_CATALOG_HPP

#include "../types.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace simploce {

  /**
   * Holds atom group specifications.
   */
  class AtomGroupCatalog {
  public:

    virtual ~AtomGroupCatalog();

    /**
     * Returns the maximum number of bound protons (or maximum proton occupancy).
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
     * Adds a specification.
     * @param spec - Specification.
     * @param occupancy - proton occupancy.
     */
    void add(const atom_group_spec_ptr_t& spec, std::size_t occupancy);

    /**
     * Returns an empty atom group catalog.
     */
    static atom_group_catalog_ptr_t make();

  };

  /**
   * Write catalog to output stream.
   * @param stream - Output stream.
   * @param catalog - Atom group specifications catalog.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const AtomGroupCatalog& catalog);

  /**
   * Reads catalog from input stream.
   * @param stream - Input stream.
   * @param catalog - Atom group specifications catalog.
   * @return Input stream.
   */
  std::istream& operator >> (std::istream& stream, AtomGroupCatalog& catalog);
}

#endif
