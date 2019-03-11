#ifndef BEM_HPP
#define BEM_HPP

#include "../types.hpp"
#include <vector>

namespace simploce {

  /**
   * Boundary element method for solving integral equations, and calculating electrostatic
   * potentials. It handles an matrix-vector equation of the type Ax=b, where A represents 
   * the kernels of the integral equations and b is the source term.
   * @see 
   * <a href="https://doi.org/10.1016/0021-9991(91)90043-K">J. Comput. Phys., 97, 144, 1991.</a>
   */
  struct BEM {

    ~BEM() {}

    /**
     * Computes kernels. For the same surface, one calls this method only once.
     * @param surface - Triangulated surface.
     */
    virtual void kernels(const TriangulatedSurface& surface) = 0;

    /**
     * Returns right-hand-size b of the matrix equation Ax=b.
     * @param atoms - Atoms, some of which should be charged.
     * @param surface -  Triangulated surface.
     * @param b - Upon return, holds the right-hand-size values.
     */
    virtual void rhs(const std::vector<Atom>& atoms,
		     const TriangulatedSurface& surface,
		     rvector_t& b) = 0;

    /**
     * Returns solution x of Ax=b.
     * @param b - Right hand side of Ax=b. Upon return, holds solution x.
     */
    virtual void solve(rvector_t& b) = 0;

    /**
     * Given the solution x of Ax=b, this methods returns the -reaction- electrostatic 
     * potentials at the given positions.
     * @param surface - Triangulated surface.
     * @param x - Solution of Ax=b
     * @param positions - Coordinates of positions at which the reaction potential should  
     * be calculated.
     * @param potentials - Upon return, holds electrostatic reaction potentials at positions.
     */
    virtual void integrate(const TriangulatedSurface& surface,
			   const rvector_t& x,
			   const std::vector<position_t> positions,
			   std::vector<elec_pot_t>& potentials) = 0;

    /**
     * Returns the inverse Debye Screening Length.
     */
    virtual real_t inverseDebyeScreeningLength() = 0;

  };
  
}

#endif
