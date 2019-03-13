#include "simploce/surface/dotted-surface-generator.hpp"
#include "simploce/surface/surface.hpp"
#include "simploce/surface/nsc.hpp"
#include "simploce/protein/atom.hpp"
#include <iostream>

namespace simploce {

  /**
   * Reles on NSC.
   * See nsc.hpp for references.
   */
  Surface generateDottedSurface(const std::vector<atom_ptr_t>& atoms, std::size_t nod)
  {
    std::size_t natoms = atoms.size();
    real_t *r = new double[3 * natoms], *radii = new double[natoms];

    // Get positions and radii.
    for (std::size_t i = 0; i != natoms; ++i) {
      const Atom& atom = *atoms[i];
      position_t ri = atom.position();
      std::size_t i3 = 3 * i;
      for (std::size_t k = 0; k != 3; ++k) {
	r[i3 + k] = ri[k];
      }
      radii[i] = atom.radius();
    }

    // Set up NSC.
    real_t *rsp;    // Surface points.
    int nsp{0};     // Number of surface points.
    real_t area{0}, *areaPerAtom = new double, volume{0};

    // Call NSC.
    int flag= ( FLAG_VOLUME | FLAG_ATOM_AREA | FLAG_DOTS );
    int error = NSC(r, radii, natoms,
		    nod, flag, &area, &areaPerAtom, &volume, &rsp, &nsp);
    if ( error != 0) {
      throw std::domain_error("An error occurred in NSC. Error code: " + error); 
    }

    // Log some results.
    std::clog << "NSC results:" << std::endl;
    std::clog << "Number of dots per fully accessible sphere: " << nod << std::endl;
    std::clog << "Total surface area: " << area << std::endl;
    std::clog << "Area per atom: " << *areaPerAtom << std::endl;
    std::clog << "Volume: " << volume << std::endl;
    std::clog << "Number of surface points: " << nsp << std::endl;

    // Create the surface points required by Surface.
    std::vector<position_t> points;
    for (int i = 0; i != nsp; ++i) {
      position_t ri;
      std::size_t i3 = 3 * i;
      for (std::size_t k = 0; k != 3; ++k) {
	ri[k] = rsp[i3 + k];
      }
      points.push_back(ri);
    }

    // Clear memory.
    delete r;
    delete radii;
    delete areaPerAtom;

    // Done
    return Surface{points, area, volume};
  }
}
