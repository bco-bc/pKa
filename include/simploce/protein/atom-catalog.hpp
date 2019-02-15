#ifndef ATOM_CATALOG_HPP
#define ATOM_CATALOG_HPP

#include "../types.hpp"
#include <map>
#include <string>

namespace simploce {

  /**
   * Holds atom specifications (types).
   */
  class AtomCatalog {
  public:

    AtomCatalog();

    /**
     * Looks up a atom specification.
     * @param name - Atom name or specification name.
     */
    atom_spec_ptr_t lookup(const std::string& name) const;

    /**
     * Returns ataom catalog.
     */
    static atom_catalog_ptr_t make();
    
  private:

    std::map<std::string, atom_spec_ptr_t> specs_;
  };
}

#endif
