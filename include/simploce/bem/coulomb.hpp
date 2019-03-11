#ifndef COULOMB_HPP
#define COULOMB_HPP

#include "../types.hpp"

namespace simploce {

  /**
   * Coulomb interactions, potentials, and fields.
   */
  class Coulomb {
  public:

    /**
     * Constructor.
     * @param epsI - Relative permittivity of the material encloded by surface.
     */
    Coulomb(const permittivity_t& epsI);

    /**
     * Computes the (direct) electrostatic potential due to charged atoms at specified locations.
     * @param atoms - Atoms.
     * @param positions - Coordinates of positions at which the electrostic potential should  
     * be calculated.
     * @param potentials - Upon return, holds electrostatic potentials at positions.
     */
    void potentials(const std::vector<Atom> &atoms,
		    const std::vector<position_t> positions,
		    std::vector<elec_pot_t>& potentials);

  private:

    permittivity_t epsI_;
    
  };
}

#endif
