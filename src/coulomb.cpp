#include "simploce/bem/coulomb.hpp"
#include "simploce/util/mu-units.hpp"
#include "simploce/protein/atom.hpp"
#include <stdexcept>

namespace simploce {

  static const real_t FOUR_PI_E0 = MUUnits<real_t>::FOUR_PI_E0;

  Coulomb::Coulomb(const permittivity_t& epsI) :
    epsI_{epsI}
  {
    if ( epsI_ <= 0.0 ) {
      throw std::domain_error("Coulomb: Permittivities must be non-negative numbers.");
    }    
  }

  /**
   * No special techniques, such as Ewald summation, are required.
   */
  void Coulomb::potentials(const std::vector<Atom> &atoms,
			   const std::vector<position_t> positions,
			   std::vector<elec_pot_t>& potentials)
  {
    const real_t factor = 1.0 / (epsI_ * FOUR_PI_E0);

    potentials.clear();
    potentials.resize(positions.size(), 0.0);
    
    for (std::size_t i = 0; i != positions.size(); ++i) {
      position_t r0 = positions[i];
      elec_pot_t potential{0};
      for (const auto& atom : atoms) {
	charge_t q = atom.charge();
	position_t r = atom.position();
	length_t R = norm<length_t>(r - r0);
	// Avoid possible self terms. Positions may refer to atom positions.
	if ( R > 0.0 ) {
	  potential += q / R;
	}
      }
      potentials[i] = factor * potential;
    }
  }
}
