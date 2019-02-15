#ifndef ATOM_SPEC_HPP
#define ATOM_SPEC_HPP

#include "../types.hpp"
#include <string>

namespace simploce {

  class AtomSpec {
  public:

    /**
     * Constructor
     * @param name - Specification/type name.
     * @param radius - Radius.
     * @param charge - Charge.
     */
    AtomSpec(const std::string& name, const radius_t& radius, const charge_t& charge);

    // Noncopyable.
    AtomSpec(const AtomSpec &) = delete;
    AtomSpec &operator = (const AtomSpec &) = delete;

    radius_t radius() const;

    charge_t charge() const;

    /**
     * Creates atom specification.
     * @param name - Specification/type name.
     * @param radius - Radius.
     * @param charge - Charge.
     * @return Atom specification.
     */
    static atom_spec_ptr_t make(const std::string& name,
				const radius_t& radius,
				const charge_t& charge);

  private:

    std::string name_;
    radius_t radius_;
    charge_t charge_;
  };
}

#endif
