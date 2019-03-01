#ifndef ATOM_GROUP_HPP
#define ATOM_GROUP_HPP

#include "../types.hpp"
#include <string>
#include <vector>

namespace simploce {

  /**
   * Group of atoms forming a logical unit. E.g. a protein residue.
   */
  class AtomGroup {
  public:

    /**
     * Constructor.
     * Creates an empty group with no name.
     */
    AtomGroup();

    /**
     * Creates an empty group.
     * @param name - Atom group name.
     */
    AtomGroup(const std::string& name);

    /**
     * Returns name.
     */
    std::string name() const;

    /**
     * Adds a single atom.
     */
    void add(const atom_ptr_t& atom);

    /**
     * Adds many atoms.
     */
    void add(const std::vector<atom_ptr_t>& atoms);

    /**
     * Returns number of atoms.
     */
    std::size_t size() const;

  private:

    std::string name_;
    std::vector<atom_ptr_t> atoms_;
    
  };

}

#endif
