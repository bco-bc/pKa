#include "simploce/protein/atom-group-catalog.hpp"
#include "simploce/protein/atom-group-spec.hpp"
#include <boost/lexical_cast.hpp>
#include <memory>
#include <map>
#include <string>
#include <stdexcept>

namespace simploce {

  /**
   * A key is defined as the number of bound protons (protonation state) and the name of an 
   * atom group. Thus, the pair (0, "ARG") specifies the deprotonated state of ARG.
   */
  using key_t = std::pair<std::size_t, std::string>;

  /**
   * Container type for holding specifications.
   */
  using container_t = std::map<std::pair<std::size_t, std::string>, atom_group_spec_ptr_t>;

  /**
   * Corresponding iterator type.
   */
  using iter_t = container_t::const_iterator;

  /**
   * Actual Container holding specifications.
   */
  static container_t container_{};

  static key_t makeKey(std::size_t occupancy, const std::string& name)
  {
    return std::make_pair(occupancy, name);
  }

  AtomGroupCatalog::~AtomGroupCatalog()
  {
  }

  std::size_t AtomGroupCatalog::maximumProtonOccupancy(const std::string& name) const
  {
    std::size_t maximum{0};
    key_t key = std::make_pair(maximum, name);
    iter_t iter = container_.find(key);
    while ( iter != container_.end() ) {
      key = std::make_pair(++maximum, name);
      iter = container_.find(key);
    }
    return maximum;
  }
  
  atom_group_spec_ptr_t AtomGroupCatalog::lookup(std::size_t protonationState,
						 const std::string& name) const
  {
    std::pair<std::size_t, std::string> key = std::make_pair(protonationState, name);
    iter_t iter = container_.find(key);
    if ( iter == container_.end() ) {
      std::string msg =
	boost::lexical_cast<std::string, std::size_t>(protonationState) +
	" : No such protonation state for atom group '" + name + "'.";
      throw std::domain_error(msg);
    }
    return iter->second;
  }

  void AtomGroupCatalog::add(const atom_group_spec_ptr_t& spec, std::size_t occupancy)
  {
    std::string name = spec->name();
    key_t key = std::make_pair(occupance, name);
    std::pair value = std::make_pair(key, spec);
    container_.insert(pair);
  }

}
