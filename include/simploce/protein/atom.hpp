#ifndef ATOM_HPP
#define ATOM_HPP

#include "../types.hpp"
#include "simploce/util/id.hpp"
#include <string>
#include <iostream>

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

    /**
     * Returns specification/type.
     */
    atom_spec_ptr_t spec() const;

    /**
     * Returns new atom. An identifier is generated.
     * @param name - Atom name.
     * @param r - Atom position.
     * @param spec - Atom specification.
     */
    static atom_ptr_t make(const std::string& name,
			   const position_t& r,
			   const atom_spec_ptr_t& spec);

    /**
     * Returns new atom.
     * @param id - Unique identifier.
     * @param name - Atom name.
     * @param r - Atom position.
     * @param spec - Atom specification.
     */
    static atom_ptr_t make(const id_t& id,
			   const std::string& name,
			   const position_t& r,
			   const atom_spec_ptr_t& spec);

  private:

    friend class AtomGroup;

    /**
     * Changes specification, e.g. due to a protonation event.
     */
    void changeSpec_(atom_spec_ptr_t& spec);
    
    id_t id_;
    std::string name_;
    position_t r_;
    atom_spec_ptr_t spec_;
    
  };

  /**
   * Write atom to output stream.
   * @param stream - Output stream.
   * @param atom - Atom.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const Atom& atom);
}

#endif
