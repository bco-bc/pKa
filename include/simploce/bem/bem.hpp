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
     * Computes kernels.
     * @param surface - Triangulated surface.
     * @param A - Upon return, holds kernel values.
     */
    virtual void kernels(const TriangulatedSurface& surface, matrix_t& A) = 0;

    /**
     * Returns right-hand-size b of the matrix equation Ax=b.
     * @param atoms - Atoms, some of which should be charged.
     * @param surface -  Triangulated surface.
     * @param rhs - Upon return, holds the right-hand-size values.
     */
    virtual void rhs(const std::vector<Atom>& atoms,
		     const TriangulatedSurface& surface,
		     vector_t& b) = 0;

    /**
     * Returns solution x of Ax=b.
     * @param A - Kernel values.
     * @param b - Right hand side of Ax=b.
     * @param x - Upon return, holds solution.
     */
    virtual void solve(const matrix_t& A, const vector_t& b, vector_t& x) = 0;

    /**
     * Given the solution x of Ax=b, this methods returns the reaction electrostatic 
     * potentials at the given positions.
     * @param surface - Triangulated surface.
     * @param x - Solution of Ax=b
     * @param positions - Coordinates of positions at which the potential is to be calculated.
     * @param potentials - Upon return, holds electrostatic potentials at positions.
     */
    virtual void integrate(const TriangulatedSurface& surface,
			   const vector_t& x,
			   const std::vector<position_t> positions,
			   std::vector<elec_pot_t>& potentials) = 0;

  };
  
}

#endif
