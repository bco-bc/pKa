#include "simploce/bem/rhs.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/util/math-constants.hpp"
#include "simploce/util/mu-units.hpp"

namespace simploce {

  static const real_t FOUR_PI_E0 = MUUnits<real_t>::FOUR_PI_E0;  

  /**
   * Zero ionic strength.
   */
  static void RHS0_(const std::vector<Atom>& atoms,
		    const TriangulatedSurface& surface,
		    const permittivity_t& epsI,
		    const permittivity_t& epsO,
		    vector_t& rhs)
  {
    const real_t factor = 1.0 / (epsI * FOUR_PI_E0);
      
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    
    rhs.clear();
    rhs.resize(ntr, false);
    for (std::size_t i = 0; i != ntr; ++i) {
      rhs(i) = 0.0;
    }
    
    for (std::size_t i = 0; i != ntr; ++i) {
      const Triangle& triangle = triangles[i];
      position_t r0 = triangle.midpoint();
      for (const Atom& atom : atoms) {
	position_t r = atom.position();
	charge_t q = atom.charge();
	dist_vect_t rij = r - r0;
	length_t R = norm<length_t>(rij);
	rhs(i) += factor * q / R;	
      }
    }
  }

  static void RHS_(const std::vector<Atom>& atoms,
		   const TriangulatedSurface& surface,
		   const permittivity_t& epsI,
		   const permittivity_t& epsO,
		   const ionic_strength_t& I,
		   vector_t& rhs)
  {
  }

  RHS::RHS(const permittivity_t& epsI, const permittivity_t& epsO, const ionic_strength_t& I) :
    epsI_{epsI}, epsO_{epsO}, I_{I}
  {
    if ( epsI_ <= 0.0 || epsO_ <= 0.0 ) {
      throw std::domain_error("RHS: Permittivities must be non-negative numbers.");
    }
    if ( I_ < 0.0 ) {
      throw std::domain_error("RHS: Ionic strength cannot be a negative number.");
    }
  }
  

  void RHS::compute(const std::vector<Atom>& atoms,
		    const TriangulatedSurface& surface,
		    vector_t& rhs)
  {
    if ( I_ > 0.0 ) {
      RHS_(atoms, surface, epsI_, epsO_, I_, rhs);
    } else {
      RHS0_(atoms, surface, epsI_, epsO_, rhs);
    }
  }
}

