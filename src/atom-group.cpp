#include "simploce/protein/atom-group.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/atom-group-catalog.hpp"
#include "simploce/protein/atom-group-spec.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/conf.hpp"
#include "simploce/util/util.hpp"
#include <stdexcept>
#include <cassert>

namespace simploce {

  /**
   * Find new occupancy value from current occupancy.
   */
  static std::size_t newOccupance_(std::size_t occupancy, std::size_t maximumOccupancy)
  {
    if ( occupancy == 0) {
      return 1;
    }
    if ( occupancy == maximumOccupancy) {
      return maximumOccupancy - 1;
    }
    real_t r = util::random<real_t>();
    return r >= 0.5 ? occupancy - 1 : occupancy + 1;
  }

  AtomGroup::AtomGroup() :
    id_{id_t::next()}, name_{}, atoms_{},
    protonatable_{false}, occupancy_{0}, maximumOccupancy_{0}
  {
  }

  AtomGroup::AtomGroup(const std::string& name) :
    id_{id_t::next()}, name_{name}, atoms_{},
    protonatable_{false}, occupancy_{0}, maximumOccupancy_{0}
  {
  }

  AtomGroup::AtomGroup(const id_t& id, const std::string& name) :
    id_{id}, name_{name}, atoms_{},
    protonatable_{false}, occupancy_{0}, maximumOccupancy_{0}
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
  
  void AtomGroup::protonatable(bool value)
  {
    protonatable_ = value;
  }

  bool AtomGroup::protonatable() const
  {
    return protonatable_;
  }
  
  void AtomGroup::changeProtonationState(const atom_group_catalog_ptr_t& catalog)
  {
    assert(protonatable_ == true);

    // Establish first the maximum proton occupancy, if not yet established.
    if ( protonatable_ && maximumOccupancy_ == 0 ) {
      maximumOccupancy_ = catalog->maximumProtonOccupancy(name_);
      if ( maximumOccupancy_ == 0) {
	throw std::domain_error(name_ + ": Protonatable but -maximum- proton occupancy is 0.");
      }
    }

    occupancy_ = newOccupance_(occupancy_, maximumOccupancy_);
    atom_group_spec_ptr_t gspec = catalog->lookup(occupancy_, name_);
    for (auto atom : atoms_) {
      std::string name = atom->name();
      atom_spec_ptr_t aspec = gspec->lookup(name);
      
      assert(aspec != nullptr);
      
      atom->changeSpec_(aspec);
    }
  }

  std::size_t AtomGroup::protonationState() const
  {
    return occupancy_;
  }

  charge_t AtomGroup::charge() const
  {
    charge_t charge{0};
    for (auto atom : atoms_) {
      charge += atom->charge();
    }
    return charge;
  }

  void AtomGroup::protonationState_(std::size_t occupancy)
  {
    occupancy_ = occupancy;
  }

  std::ostream& operator << (std::ostream& stream, const AtomGroup& atomGroup)
  {
    stream << atomGroup.id()() << SPACE << atomGroup.name()
	   << SPACE << atomGroup.protonatable() << SPACE << atomGroup.protonationState();
    std::vector<atom_ptr_t> atoms = atomGroup.atoms();
    stream << SPACE << atoms.size();
    for (auto atom : atoms) {
      stream << SPACE << atom->id()();
    }
    return stream;
  }

}
