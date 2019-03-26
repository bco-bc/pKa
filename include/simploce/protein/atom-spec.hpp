#ifndef ATOM_SPEC_HPP
#define ATOM_SPEC_HPP

#include "../types.hpp"
#include <string>
#include <iostream>

namespace simploce {

  class AtomSpec {
  public:

    /**
     * Constructor
     * @param name - Specification/type name.
     * @param radius - Radius.
     * @param charge - Charge.
     * @param mass - Mass.
     */
    AtomSpec(const std::string& name,
	     const radius_t& radius,
	     const charge_t& charge,
	     const mass_t& mass);

    // Noncopyable.
    AtomSpec(const AtomSpec &) = delete;
    AtomSpec &operator = (const AtomSpec &) = delete;

    /**
     * Returns specification name.
     */
    std::string name() const;

    /**
     * Returns atom radius.
     */
    radius_t radius() const;

    /**
     * Returns atom charge.
     */
    charge_t charge() const;

    /**
     * Returns mass.
     */
    mass_t mass() const;

    /**
     * Creates atom specification.
     * @param name - Specification/type name.
     * @param radius - Radius.
     * @param charge - Charge.
     * @return Atom specification.
     */
    static atom_spec_ptr_t make(const std::string& name,
				const radius_t& radius,
				const charge_t& charge,
				const mass_t& mass);

  private:

    std::string name_;
    radius_t radius_;
    charge_t charge_;
    mass_t mass_;
  };

  /**
   * Writes atom specification to output stream.
   * @param stream - Output stream.
   * @param spec - Atom specification.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const AtomSpec& spec);
}

#endif
