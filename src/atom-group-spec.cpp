#include "simploce/protein/atom-group-spec.hpp"
#include <stdexcept>

namespace simploce {

  AtomGroupSpec::AtomGroupSpec() :
    specs_{}
  {
  }

  atom_spec_ptr_t AtomGroupSpec::lookup(const std::string& name) const
  {
    using iter_t = std::map<std::string, atom_spec_ptr_t>::const_iterator;
    
    iter_t iter = specs_.find(name);
    if ( iter == specs_.end() ) {
      throw std::domain_error(name + ": No such atom specification in atom group specification.");
    }
    return (*iter).second;
  }
}
