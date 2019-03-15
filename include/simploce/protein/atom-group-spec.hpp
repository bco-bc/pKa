#ifndef ATOM_GROUP_SPEC_HPP
#define ATOM_GROUP_SPEC_HPP

#include "../types.hpp"
#include <map>
#include <string>

namespace simploce {

  class AtomGroupSpec {
  public:
    
    AtomGroupSpec();

    atom_spec_ptr_t lookup(const std::string& name) const;
    
  private:

    std::map<std::string, atom_spec_ptr_t> specs_;

  };
}

#endif
