#include "simploce/protein/atom-group.hpp"
#include "simploce/protein/atom.hpp"
#include <stdexcept>

namespace simploce {

  AtomGroup::AtomGroup() :
    id_{id_t::next()}, name_{}, atoms_{}
  {
  }

  AtomGroup::AtomGroup(const std::string& name) :
    id_{id_t::next()}, name_{name}, atoms_{}
  {
  }

  AtomGroup::AtomGroup(const id_t& id, const std::string& name) :
    id_{id}, name_{name}, atoms_{}
  {
  }

  AtomGroup::id_t AtomGroup::id() const
  {
    return id_;
  }

  std::string AtomGroup::name() const
  {
    return name_;
  }

  void AtomGroup::add(const atom_ptr_t& atom)
  {
    atoms_.push_back(atom);
  }

  void AtomGroup::add(const std::vector<atom_ptr_t>& atoms)
  {
    for (auto& atom : atoms) {
      this->add(atom);
    }
  }

  const std::vector<atom_ptr_t>  AtomGroup::atoms() const
  {
    return atoms_;
  }
}
