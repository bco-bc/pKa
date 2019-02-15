#include "simploce/protein/atom-spec.hpp"

namespace simploce {

  AtomSpec::AtomSpec(const std::string& name, const radius_t& radius, const charge_t& charge) :
    name_{name}, radius_{radius}, charge_{charge}
  {
  }

  radius_t AtomSpec::radius() const
  {
    return radius_;
  }

  charge_t AtomSpec::charge() const
  {
    return charge_;
  }

  atom_spec_ptr_t AtomSpec::make(const std::string& name,
				 const radius_t& radius,
				 const charge_t& charge)
  {
    return std::make_shared<AtomSpec>(name, radius, charge);
  }
}
