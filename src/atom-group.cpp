#include "simploce/protein/atom-group.hpp"
#include "simploce/protein/atom.hpp"
#include <stdexcept>

namespace simploce {

  AtomGroup::AtomGroup() : name_{}, atoms_{}
  {
  }

  AtomGroup::AtomGroup(const std::string& name) : name_{name}, atoms_{}
  {
  }

  std::string AtomGroup::name() const
  {
    return name_;
  }

  void AtomGroup::add(const atom_ptr_t& atom)
  {
    atoms_.push_back(atom);
  }
}
