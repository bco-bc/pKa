#ifndef ATOM_GROUP_HPP
#define ATOM_GROUP_HPP

#include "../types.hpp"
#include "simploce/util/id.hpp"
#include <string>
#include <vector>
#include <map>

namespace simploce {

  /**
   * Group of atoms forming a logical unit. E.g. a protein residue. The group name is typically 
   * those of residues, such as GLU or HIS.
   */
  class AtomGroup {
  public:

    /**
     * Atom group identifier.
     */
    using id_t = Id<AtomGroup, std::size_t>;

    /**
     * Constructor.
     * Creates an empty group with no name. An identifier is generated.
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

    /**
     * Specifies whether this group is protonatable.
     * @param value - Specification.
     */
    void protonatable(bool value);

    /**
     * Is this group protonatable?
     * @return Result.
     */
    bool protonatable() const;

    /**
     * Changes the current protonation state, depending on the current protonation state.
     * @param catalog - Atom group specification catalog.
     */
    void changeProtonationState(const atom_group_catalog_ptr_t& catalog);

    /**
     * Returns protonation state (proton occupancy).
     * @return Usually, 0 or 1, but 2 or larger is possible as well.
     */
    std::size_t protonationState() const;

    /**
     * Returns group's total charge.
     */
    charge_t charge() const;

    /**
     * Translate this group, RIGID body motion.
     */
    void translate();

    /**
     * Rotate this group, RIGID body motion.
     */
    void rotate();

  private:

    friend class ProteinStructure;

    void protonationState_(std::size_t occupancy);

    id_t id_;
    std::string name_;
    std::vector<atom_ptr_t> atoms_;
    bool protonatable_;
    std::size_t occupancy_;
    std::size_t maximumOccupancy_;
  };

  /**
   * Writes atom group to output stream.
   * @param stream - Output stream.
   * @param atomGroup - Atom group.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const AtomGroup& atomGroup);

}

#endif
