#ifndef ATOM_HPP
#define ATOM_HPP

#include "../types.hpp"
#include "simploce/util/id.hpp"
#include <string>

namespace simploce {

  /**
   * An atom.
   */
  class Atom {
  public:

    /**
     * Atom identifier.
     */
    using id_t = Id<Atom, std::size_t>;

    /**
     * Constructor
     * @param name - Atom name.
     * @param r - Atom position.
     * @param spec - Atom specification.
     */
    Atom(const std::string& name, const position_t& r, const atom_spec_ptr_t& spec);
    
    /**
     * Constructor
     * @param id - Unique identifier.
     * @param name - Atom name.
     * @param r - Atom position.
     * @param spec - Atom specification.
     */
    Atom(const id_t& id,
	 const std::string& name,
	 const position_t& r,
	 const atom_spec_ptr_t& spec);

    /**
     * Returns identifier.
     */
    id_t id() const;

    /**
     * Returns name.
     */
    std::string name() const;

    /**
     * Returns position.
     */    
    position_t position() const;

    /**
     * Returns charge value.
     */
    charge_t charge() const;

    /**
     * Returns radius.
     */
    radius_t radius() const;

  private:

    static id_t ID;

    id_t id_;
    std::string name_;
    position_t r_;
    atom_spec_ptr_t spec_;
    
  };
}

#endif
