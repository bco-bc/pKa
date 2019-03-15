#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "types.hpp"

namespace simploce {

  struct Factory {

    /**
     * Returns atom specifications catalog.
     */
    static atom_catalog_ptr_t atomCatalog();

    /**
     * Returns the atom group specification catalog.
     */
    static atom_group_catalog_ptr_t atomGroupCatalog();

    /**
     * Returns a triangulator.
     * @param ntriangles - Requested number of triangles.
     * @return Triangulator.
     */
    static triangulator_ptr_t triangulator(std::size_t ntriangles);

  };
}

#endif
