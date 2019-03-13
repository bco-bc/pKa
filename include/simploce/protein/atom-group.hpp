#ifndef ATOM_GROUP_HPP
#define ATOM_GROUP_HPP

#include "../types.hpp"
#include "simploce/util/id.hpp"
#include <string>
#include <vector>

namespace simploce {

  /**
   * Group of atoms forming a logical unit. E.g. a protein residue.
   */
  class AtomGroup {
  public:

    /**
     * Atom group identifier.
     */
    using id_t = Id<AtomGroup, std::size_t>;

    /**
     * Constructor.
     * Creates an empty group with no name.
     */
    AtomGroup();

    /**
     * Constructor. An identifier is generated.
     * @param name - Atom group name.
     */
    AtomGroup(const std::string& name);

    /**
     * Creates an empty group.
     * @param id - Identifier.
     * @param name - Atom group name.
     */
    AtomGroup(const id_t& id, const std::string& name);

    /**
     * Returns identifier.
     */
    id_t id() const;

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

    /**
     * Returns atoms.
     */
    const std::vector<atom_ptr_t> atoms() const;

  private:

    id_t id_;
    std::string name_;
    std::vector<atom_ptr_t> atoms_;
    
  };

}

#endif
