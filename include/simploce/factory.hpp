#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "types.hpp"
#include <string>

namespace simploce {
  namespace factory {

    /**
     * Returns atom specifications catalog.
     */
    atom_catalog_ptr_t atomCatalog();

    /**
     * Reads an atom group specification catalog from file.
     * @param fileName - File name
     */
    atom_group_catalog_ptr_t atomGroupCatalog(const std::string& fileName);

    /**
     * Returns a triangulator.
     * @param ntriangles - Requested number of triangles.
     * @return Triangulator.
     */
    triangulator_ptr_t triangulator(std::size_t ntriangles);

  }
}

#endif
