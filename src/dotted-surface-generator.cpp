#include "simploce/surface/dotted-surface-generator.hpp"
#include "simploce/surface/surface.hpp"
#include "simploce/surface/nsc.hpp"
#include <iostream>

namespace simploce {

  /**
   * Reles on NSC.
   * See nsc.hpp for references.
   */
  Surface generateDottedSurface(const std::vector<particle_t>& particles, std::size_t nod)
  {
    std::size_t nparticles = particles.size();
    real_t *r = new double[3 * nparticles], *radii = new double[nparticles];

    // Get positions and radii.
    for (std::size_t i = 0; i != nparticles; ++i) {
      const particle_t& particle = particles[i];
      position_t ri = std::get<0>(particle);
      std::size_t i3 = 3 * i;
      for (std::size_t k = 0; k != 3; ++k) {
	r[i3 + k] = ri[k];
      }
      radii[i] = std::get<1>(particle);
    }

    // Set up NSC.
    real_t *rsp;    // Surface points.
    int nsp{0};     // Number of surface points.
    real_t area{0}, *areaPerParticle = new double, volume{0};

    // Call NSC.
    int flag= ( FLAG_VOLUME | FLAG_ATOM_AREA | FLAG_DOTS );
    int error = NSC(r, radii, nparticles,
		    nod, flag, &area, &areaPerParticle, &volume, &rsp, &nsp);
    if ( error != 0) {
      throw std::domain_error("An error occurred in NSC. Error code: " + error); 
    }

    // Log some results.
    std::clog << "NSC results:" << std::endl;
    std::clog << "Number of dots per fully accessible sphere: " << nod << std::endl;
    std::clog << "Total surface area: " << area << std::endl;
    std::clog << "Area per particle: " << *areaPerParticle << std::endl;
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
    delete areaPerParticle;

    // Done
    return Surface{points};
  }
}
