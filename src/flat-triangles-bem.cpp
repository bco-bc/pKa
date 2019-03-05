#include "simploce/bem/flat-triangles-bem.hpp"
#include "simploce/bem/matrix-inversion.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/util/math-constants.hpp"
#include "simploce/util/entity-range.hpp"
#include "simploce/util/util.hpp"
#include "simploce/util/math-constants.hpp"
#include "simploce/util/mu-units.hpp"
#include "simploce/util/lu.hpp"
#include <future>
#include <thread>
#include <stdexcept>
#include <iostream>

namespace simploce {
  
  //using namespace boost::numeric::ublas;  

  const real_t FOUR_PI = 4.0 * MathConstants<real_t>::PI;
  const real_t E0 = MUUnits<real_t>::E0;

  struct KernelResult {
    KernelResult(matrix_t& mtrx) : matrix{mtrx} {}
    matrix_t& matrix;
  };

  using result_t = KernelResult;
  using entity_range_t = EntityRange<Triangle>;

  /**
   * This corresponds to dF(r; s)/dn * area, with r located on the triangle, 
   * F(r;s)=1/(4*pi*|r-s|), and area is the area of the triangle.
   * @see Equation (2.3) in reference.
   */
  static real_t dFdn_(const position_t& s, const Triangle& triangle)
  {
    position_t r = triangle.midpoint();
    normal_t n = triangle.normal();
    dist_vect_t disv = r - s;
    real_t imp = inner<real_t>(disv, n);
    length_t R = norm<real_t>(disv);
    return -imp / (FOUR_PI * R * R * R);
  }

  /**
   * Zero ionic strength. Collocation points are vertices. Integration over surface 
   * assumes the kernel to be constant across a triangle ("midpoint rule").
   */
  static void L0_(const TriangulatedSurface& surface,
		  const permittivity_t& epsI,
		  const permittivity_t& epsO,
		  matrix_t& L)
  {
    const std::vector<Triangle>& triangles = surface.triangles();
    const std::vector<vertex_ptr_t>& vertices = surface. vertices();

    // Ratio of permittivities.
    const real_t ratio = epsO/epsI;
    const real_t factor = 2.0 * (ratio - 1.0) / (1.0 + ratio);

    // Initialize kernel. Number of collocation points corresponds to the number of triangles.
    std::size_t ntr = triangles.size();
    L = matrix_t(ntr, ntr);
    for (std::size_t i = 0; i != ntr; ++i) {
      for ( std::size_t j = 0; j != ntr; ++j) {
	L(i,j) = 0.0;
      }
    }
    
    // Compute kernel elements.
    for (std::size_t i = 0; i != ntr; ++i) {
      const Triangle& ti = triangles[i];
      position_t r0 = ti.midpoint();
      
      for (std::size_t j = 0; j < i; ++j) {
	const Triangle& tj = triangles[j];
	area_t area = tj.area();
	real_t lij = factor * dFdn_(r0, tj) * area;
	L(i,j) -= lij;
      }

      // Diagonal element.
      L(i,i) = 1.0;

      for (std::size_t j = i+1; j < ntr; ++j) {
	const Triangle& tj = triangles[j];
	area_t area = tj.area();
	real_t lij = factor * dFdn_(r0, tj) * area;
	L(i,j) -= lij;
      }
      
    }
  }

  static void KLMN_(const TriangulatedSurface& surface,
		    const permittivity_t& epsI,
		    const permittivity_t& epsO,
		    matrix_t& L)
  {
  }

  /**
   * Right hand side for zero ionic strength.
   */
  static void RHS0_(const std::vector<Atom>& atoms,
		    const TriangulatedSurface& surface,
		    const permittivity_t& epsI,
		    const permittivity_t& epsO,
		    rvector_t& b)
  {
    const real_t ratio = epsO/epsI;
    const real_t factor = 2.0 / ((1.0 + ratio) * epsI * E0 * FOUR_PI);
      
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    
    b = rvector_t(ntr);
    for (std::size_t i = 0; i != ntr; ++i) {
      b(i) = 0.0;
    }
    
    for (std::size_t i = 0; i != ntr; ++i) {
      const Triangle& triangle = triangles[i];
      position_t r0 = triangle.midpoint();
      for (const Atom& atom : atoms) {
	position_t r = atom.position();
	charge_t q = atom.charge();
	length_t R = norm<length_t>(r - r0);
	b(i) += factor * q / R;	
      }
    }
  }

  /**
   * Right hand side for non-zero ionic strength.
   */
  static void RHS_(const std::vector<Atom>& atoms,
		   const TriangulatedSurface& surface,
		   const permittivity_t& epsI,
		   const permittivity_t& epsO,
		   const ionic_strength_t& I,
		   rvector_t& rhs)
  {
  }

  FlatTrianglesBEM::FlatTrianglesBEM(const permittivity_t& epsI,
				     const permittivity_t& epsO,
				     const ionic_strength_t& I) :
    BEM(), epsI_(epsI), epsO_(epsO), I_(I), indx_{}, luQR_{}
  {
    if ( epsI_ <= 0.0 || epsO_ <= 0.0 ) {
      throw std::domain_error("BEM: Permittivities must be non-negative numbers.");
    }
    if ( I_ < 0.0 ) {
      throw std::domain_error("BEM: Ionic strength cannot be a negative number.");
    }
  }

  void FlatTrianglesBEM::kernels(const TriangulatedSurface& surface)
  {
    matrix_t S;
    if ( I_ > 0.0 ) {
      KLMN_(surface, epsI_, epsO_, S);
    } else {
      L0_(surface, epsI_, epsO_, S);
    }

    /*
    std::size_t ndim = surface.triangles().size();
    //indx_.clear();
    indx_ = ivector_t(ndim);
    luDecomposition<real_t,
		    boost::numeric::ublas::matrix,
		    boost::numeric::ublas::vector>(S, ndim, indx_);
    */
    luQR_ = S.colPivHouseholderQr();    
  }

  void FlatTrianglesBEM::rhs(const std::vector<Atom>& atoms,
			     const TriangulatedSurface& surface,
			     rvector_t& b)
  {
    if ( I_ > 0.0 ) {
      RHS_(atoms, surface, epsI_, epsO_, I_, b);
    } else {
      RHS0_(atoms, surface, epsI_, epsO_, b);
    }
  }

  
  void FlatTrianglesBEM::solve(rvector_t& b)
  {
    /*
    std::size_t ndim = b.size();
    backSubstitution<real_t,
		     boost::numeric::ublas::matrix,
		     boost::numeric::ublas::vector>(S, ndim, indx_, b);
    */
    b = rvector_t{luQR_.solve(b)};
  }

  void FlatTrianglesBEM::integrate(const TriangulatedSurface& surface,
				   const rvector_t& x,
				   const std::vector<position_t> positions,
				   std::vector<elec_pot_t>& potentials)
  {
    const real_t ratio = epsO_/epsI_;
    const real_t factor = ratio - 1.0;

    potentials.clear();
    potentials.resize(positions.size(), 0.0);

    const std::vector<Triangle>& triangles = surface.triangles();
    for (std::size_t i = 0; i != positions.size(); ++i) {
      const position_t& ri = positions[i];
      for (std::size_t j = 0; j != triangles.size(); ++j) {
	const Triangle& triangle = triangles[j];
	area_t area = triangle.area();
	real_t dFdn = dFdn_(ri, triangle);
	potentials[i] += factor * dFdn * x[j] * area;
      }
    }
  }

}
