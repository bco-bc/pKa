#include "simploce/protein/atom.hpp"
#include "simploce/protein/atom-spec.hpp"
#include <stdexcept>

namespace simploce {

  using id_t = Atom::id_t;

  Atom::Atom(const std::string& name, const position_t& r, const atom_spec_ptr_t& spec) :
    id_{id_t::next()}, name_{name}, r_{r}, spec_{spec}
  {
    if ( !spec_ ) {
      throw std::domain_error("Atom: Missing atom specification.");
    }
  }

  Atom::Atom(const id_t& id,
	     const std::string& name,
	     const position_t& r,
	     const atom_spec_ptr_t& spec) :
    id_{id}, name_{name}, r_{r}, spec_{spec}
  {
  }

  id_t Atom::id() const
  {
    return id_;
  }

  std::string Atom::name() const
  {
    return name_;
  }

  position_t Atom::position() const
  {
    return r_;
  }

  charge_t Atom::charge() const
  {
    return spec_->charge();
  }

  radius_t Atom::radius() const
  {
    return spec_->radius();
  }

  atom_spec_ptr_t Atom::spec() const
  {
    return spec_;
  }

}
