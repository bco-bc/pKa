#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "./types.hpp"

namespace simploce {

  struct Factory {

    /**
     * Returns atom specifications catalog.
     */
    static atom_catalog_ptr_t atomCatalog();

    /**
     * Returns a triangulator.
     */
    static triangulator_ptr_t triangulator();

  };
}

#endif
