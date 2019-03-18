#ifndef GMX_ATOM_GROUP_CATALOG_HPP
#define GMX_ATOM_GROUP_CATALOG_HPP

#include "atom-group-catalog.hpp"
#include "../types.hpp"
#include <istream>

namespace simploce {

  /**
   * Reads a Gromacs topology file to create a catalog for atom group specifications.
   */
  class GMXAtomGroupCatalog : public AtomGroupCatalog {
  public:

    GMXAtomGroupCatalog();

    /**
     * Reads a topology from an input stream.
     * @param stream - Input stream.
     * @return Catalog.
     */
    static atom_group_catalog_ptr_t valueOf(std::istream& stream);
    
  };
}

#endif
