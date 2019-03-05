#ifndef KERNEL_HPP
#define KERNEL_HPP

#include "bem.hpp"

namespace simploce {

  /**
   * Defines a matrix equation of the type Ax=b, where b is the source term and
   * A represents the kernels of the integral equations. Collocation points are the 
   * midpoints of flat triangles. It is assumed the the kernel values are constant 
   * accross triangles.
   */
  class FlatTrianglesBEM : public BEM {
  public:

    /**
     * Constructor.
     * @param epsI - Relative permittivity of the material enclosed by surface.
     * @param epsO - Relative permittivity of the material outside the surface.
     * @param I - Ionic strength of the material outside the surface.
     * @see <a href="https://en.wikipedia.org/wiki/Permittivity">Permittivity</a>
     */
    FlatTrianglesBEM(const permittivity_t& epsI,
		     const permittivity_t& epsO,
		     const ionic_strength_t& I);

    void kernels(const TriangulatedSurface& surface);

    void rhs(const std::vector<Atom>& atoms,
	     const TriangulatedSurface& surface,
	     rvector_t& b) override;

    void solve(rvector_t& b) override;

    void integrate(const TriangulatedSurface& surface,
		   const rvector_t& x,
		   const std::vector<position_t> positions,
		   std::vector<elec_pot_t>& potentials) override;
    
  private:
    
    permittivity_t epsI_;
    permittivity_t epsO_;
    ionic_strength_t I_;

    // Pivot information.
    ivector_t indx_;
    Eigen::ColPivHouseholderQR<matrix_t> luQR_;
  };

}


#endif
