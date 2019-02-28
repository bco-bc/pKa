#ifndef KERNEL_HPP
#define KERNEL_HPP

#include "../types.hpp"
#include <vector>

namespace simploce {

  class TriangulatedSurface;
  
  class Kernels {
  public:

    /**
     * Constructor.
     * @param epsI - Relative permittivity of the material enclosed by surface.
     * @param epsO - Relative permittivity of the material outside the surface.
     * @param I - Ionic strength of the material outside the surface.
     * @see <a href="https://en.wikipedia.org/wiki/Permittivity">Permittivity</a>
     */
    Kernels(const permittivity_t& epsI,  const permittivity_t& epsO, const ionic_strength_t& I);

    /**
     * Computes kernel L for zero ionic strength.
     * @param surface - Triangulated surface.
     * @param L - Upon return, holds kernel values of L. Its size corresponds to the number 
     * of collocation points.
     */
    void L0(const TriangulatedSurface& surface, std::vector<real_t>& L);

    /*
    void Lij0(double e_ratio,
	      MATH_VECTOR<double> &x, MATH_VECTOR<double> &n,
	      MATH_VECTOR<double> &x0,
	      double &lij);
    
    
    
    
    void Lij(double ka, double e_ratio,
	     MATH_VECTOR<double> &x, MATH_VECTOR<double> &n,
	     MATH_VECTOR<double> &x0,
	     double &lij);
    
    
    
    
    void KLij(double ka, double e_ratio,
	      MATH_VECTOR<double> &x, MATH_VECTOR<double> &n,
	      MATH_VECTOR<double> &x0,
	      double &kij, double &lij);

    void KLMNij(double ka, double e_ratio,
		MATH_VECTOR<double> &x, MATH_VECTOR<double> &n,
		MATH_VECTOR<double> &x0, MATH_VECTOR<double> &n0,
		double &kij, double &lij, double &mij, double &nij);
    
    */
    
  private:
    
    permittivity_t epsI_;
    permittivity_t epsO_;
    ionic_strength_t I_;
    
  };

}


#endif
