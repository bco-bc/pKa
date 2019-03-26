#include "simploce/protein/atom-spec.hpp"
#include "simploce/conf.hpp"

namespace simploce {

  AtomSpec::AtomSpec(const std::string& name,
		     const radius_t& radius,
		     const charge_t& charge,
		     const mass_t& mass) :
    name_{name}, radius_{radius}, charge_{charge}, mass_{mass}
  {
  }

  std::string AtomSpec::name() const
  {
    return name_;
  }

  radius_t AtomSpec::radius() const
  {
    return radius_;
  }

  charge_t AtomSpec::charge() const
  {
    return charge_;
  }

  mass_t AtomSpec::mass() const
  {
    return mass_;
  }

  atom_spec_ptr_t AtomSpec::make(const std::string& name,
				 const radius_t& radius,
				 const charge_t& charge,
				 const mass_t& mass)
  {
    return std::make_shared<AtomSpec>(name, radius, charge, mass);
  }

  std::ostream& operator << (std::ostream& stream, const AtomSpec& spec)
  {
    stream.setf(std::ios::scientific);
    stream.precision(PRECISION);
    stream << std::setw(WIDTH) << spec.name() << SPACE << std::setw(WIDTH) << spec.radius()
	   << SPACE << std::setw(WIDTH) << spec.charge()
	   << SPACE << std::setw(WIDTH) << spec.mass();
    return stream;
  }
}
