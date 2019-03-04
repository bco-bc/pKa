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

    /**
     * Computes kernels. Returned is an inverse matrix S, such that Sb = x and A = S^-1.
     * @param surface - Triangulated surface.
     * @param S - Upon return, holds values for S.
     */
    void kernels(const TriangulatedSurface& surface, matrix_t& S);

    void rhs(const std::vector<Atom>& atoms,
	     const TriangulatedSurface& surface,
	     vector_t& b) override;

    /**
     * Returns solution x of Ax=b or Sb = x with A = S^-1.
     * @param S - Kernel values.
     * @param b - Right hand side of Ax=b.
     * @param x - Upon return, holds solution.
     */
    void solve(const matrix_t& S, const vector_t& b, vector_t& x) override;

    void integrate(const TriangulatedSurface& surface,
		   const vector_t& x,
		   const std::vector<position_t> positions,
		   std::vector<elec_pot_t>& potentials) override;
    
  private:
    
    permittivity_t epsI_;
    permittivity_t epsO_;
    ionic_strength_t I_;

    // Pivot information.
    boost::numeric::ublas::vector<std::size_t> indx_;
  };

}


#endif
