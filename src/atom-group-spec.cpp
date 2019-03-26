#include "simploce/protein/atom-group-spec.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/conf.hpp"
#include <stdexcept>
#include <utility>
#include <memory>

namespace simploce {

  using iter_t = std::map<std::string, atom_spec_ptr_t>::const_iterator;
    

  AtomGroupSpec::AtomGroupSpec(const std::string& name,
			       std::size_t occupancy,
			       const std::vector<atom_spec_ptr_t>& specs) :
    name_{name}, occupancy_{occupancy}, specs_{}
  {
    for (auto spec : specs) {
      this->add(spec);
    }
  }

  atom_spec_ptr_t AtomGroupSpec::lookup(const std::string& name) const
  {
    iter_t iter = specs_.find(name);
    if ( iter == specs_.end() ) {
      return atom_spec_ptr_t{};
    }
    return (*iter).second;
  }

  std::string AtomGroupSpec::name() const
  {
    return name_;
  }

  std::size_t AtomGroupSpec::protonationState() const
  {
    return occupancy_;
  }

  std::vector<atom_spec_ptr_t> AtomGroupSpec::atomSpecifications() const
  {
    std::vector<atom_spec_ptr_t> all;
    for (iter_t iter = specs_.begin(); iter != specs_.end(); ++iter) {
      all.push_back(iter->second);
    }
    return all;
  }

  void AtomGroupSpec::add(const atom_spec_ptr_t& spec)
  {
    if ( this->lookup(spec->name()) ) {
      std::string msg =
	"Atom specification '" + spec->name() + "' "  + 
	"does already exist in atom group spec '" + name_ + "'.";
      throw std::domain_error(msg);
    }
    std::pair<std::string, atom_spec_ptr_t> value = std::make_pair(spec->name(), spec);
    specs_.insert(value);
  }

  atom_group_spec_ptr_t AtomGroupSpec::make(const std::string& name,
					    std::size_t occupancy,
					    const std::vector<atom_spec_ptr_t>& specs)
  {
    return std::make_shared<AtomGroupSpec>(name, occupancy, specs);
  }

  std::ostream& operator << (std::ostream& stream, const AtomGroupSpec& spec)
  {
    using iterator_t =  std::vector<atom_spec_ptr_t>::iterator;
    
    stream << std::setw(WIDTH) << spec.name() << SPACE << spec.protonationState() << std::endl;
    std::vector<atom_spec_ptr_t> all = spec.atomSpecifications();
    for (iterator_t iter = all.begin(); iter != all.end(); ++iter) {
      atom_spec_ptr_t spec = *iter;
      stream << *spec;
      if ( iter < all.end() - 1) {
	stream << std::endl;
      }
    }
    return stream;
  }
      
}
