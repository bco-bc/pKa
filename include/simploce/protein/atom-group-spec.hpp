#ifndef ATOM_GROUP_SPEC_HPP
#define ATOM_GROUP_SPEC_HPP

#include "../types.hpp"
#include <map>
#include <string>
#include <fstream>
#include <iostream>

namespace simploce {

  class AtomGroupSpec {
  public:

    /**
     * Constructor.
     * @param name - Specification name.
     * @param occupancy - Protonation state.
     * @param specs - Atom specifications of all atoms in this group.
     */
    AtomGroupSpec(const std::string& name,
		  std::size_t occupancy,
		  const std::vector<atom_spec_ptr_t>& specs);

    /**
     * Returns atom specification with given name.
     * @return Specification, or if nonexistent an empty atom_spec_ptr_t.
     */
    atom_spec_ptr_t lookup(const std::string& name) const;

    /**
     * Returns specification name.
     */
    std::string name() const;

    /**
     * Returns protonation state.
     * @returns Proton occupancy.
     */
    std::size_t protonationState() const;

    /**
     * Returns atom specifications for all atoms in this group.
     */
    std::vector<atom_spec_ptr_t> atomSpecifications() const;

    /**
     * Adds another atom specification.
     */ 
    void add(const atom_spec_ptr_t& spec);

    /**
     * Returns new atom group specification.
     * @param name - Specification name.
     * @param occupancy - Protonation state.
     * @param specs - Atom specifications of all atoms in this group.
     * @return Atom group specification.
     */
    static atom_group_spec_ptr_t make(const std::string& name,
				      std::size_t occupancy,
				      const std::vector<atom_spec_ptr_t>& specs);    
    
  private:

    std::string name_;
    std::size_t occupancy_;
    std::map<std::string, atom_spec_ptr_t> specs_;

  };

  /**
   * Writes atom group specification to an output stream.
   * @param stream - Output stream.
   * @param spec - Atom group specification.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const AtomGroupSpec& spec);
  
}

#endif
