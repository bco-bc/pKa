#ifndef ATOM_GROUP_SPEC_HPP
#define ATOM_GROUP_SPEC_HPP

#include "../types.hpp"
#include <map>
#include <string>

namespace simploce {

  class AtomGroupSpec {
  public:
    
    AtomGroupSpec(const std::string& name);

    /**
     * Returns atom specification with given name.
     */
    atom_spec_ptr_t lookup(const std::string& name) const;

    /**
     * Returns specification name.
     */
    std::string name() const;
    
  private:

    std::string name_;
    std::map<std::string, atom_spec_ptr_t> specs_;

  };
}

#endif
