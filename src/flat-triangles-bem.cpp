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
#include <utility>
#include <tuple>

namespace simploce {
  
  //using namespace boost::numeric::ublas;  

  const real_t FOUR_PI = 4.0 * MathConstants<real_t>::PI;
  const real_t E = MathConstants<real_t>::E;
  const real_t E0 = MUUnits<real_t>::E0;

  struct KernelResult {
    KernelResult(matrix_t& mtrx) : matrix{mtrx} {}
    matrix_t& matrix;
  };

  using result_t = KernelResult;
  using entity_range_t = EntityRange<Triangle>;

  /**
   *  Returns F(r;s) = 1/(4*pi*|r-s|)
   */
  static std::tuple<real_t, length_t> F_(const position_t r, const position_t s)
  {
    length_t R = norm<length_t>(r - s);
    real_t F = 1.0 / (FOUR_PI * R);
    return std::make_tuple(F, R);
  }

  /**
   * Returns P(r;s) = exp(-ka * |r-s|)) * F(r;s)
   */
  static real_t P_(const position_t r, const position_t s, const real_t& ka)
  {
    auto pair = F_(r,s);
    real_t F = std::get<0>(pair);
    real_t R = std::get<1>(pair);
    real_t kaR = ka * R;
    real_t exp = std::pow(E, -kaR);
    return exp * F;
  }

  /**
   * dF(r; s)/dn, where F(r;s) = 1/(4*pi*|r-s|), where n is the unit normal vector at r.
   * @return dFdn and R=|r-s|.
   */
  static std::tuple<real_t, length_t> dFdn_(const position& r,
					    const normal_t& n,
					    const position_t& s)
  {
    dist_vect_t disv = r - s;
    real_t imp = inner<real_t>(ns, disv);
    length_t R = norm<real_t>(rs);
    real_t dFdn = -imp / (FOUR_PI * R * R * R);
    return std::make_tuple(dFdn, R);
  }

  /**
   * dF(r; s)/dns, where F(r;s) = 1/(4*pi*|r-s|), where ns is the unit vector at s.
   * @return dFdns and |r-s|.
   */
  static std::tuple<real_t, length_t> dFdns_(const position& r,
					     const position_t& s,
					     const normal_t& ns)
  {
    dist_vect_t disv = r - s;
    real_t imp = inner<real_t>(ns, disv);
    length_t R = norm<real_t>(disv);
    real_t dFdns = imp / (FOUR_PI * R * R * R);
    return std::make_tuple(dFdns, R);
  }

  /**
   * dF^2/dnsdn, where F(r;s) = 1/(4*pi*|r-s|), n is the unit normal vector at r, and 
   * ns is the unit normal vector at s.
   * @return dF2dnsdn, |r-s|, n*(r-s)/|r-s|, ns*(r-s)/|r-s|.
   */
  static std::tuple<real_t, length_t, real_t, real_t> dF2dnsdn_(const position_t& r,
								const normal_t& n,
								const position_t& s,
								const normal_t& ns)
  {
    dist_vect_t disv = r - s;
    length_t R = norm<real_t>(disv);
    real_t imp_nns = inner<real_t>(n, ns);
    real_t imp_n_R = inner<real_t>(n, disv) / R;
    real_t imp_ns_R = inner<real_t>(ns, disv) / R;
    real_t dF2dndns = (imp_nns - 3.0 * imp_n_R * imp_ns_R) / (FOUR_PI * R * R * R);
    return std::make_tuple(dF2dndns, R, imp_n_R, imp_ns_R);
  }

  /**
   * dP(r; s)/dn, where P(r;s) = exp(-ka * |r-s|)) * F(r;s), where n is the unit vector at r.
   */
  static real_t dPdn_(const position_t& r,
		      const position_t& n,
		      const position_t& s,
		      const real_t& ka)
  {
    auto pair = dFdn_(r, n, s);
    real_t dFdn = std::get<0>(pair);
    length_t R = std::get<1>(pair);
    real_t kaR = ka * R;
    real_t exp = std::pow(E, -kaR);
    return (1.0 + kaR) * exp * dFdn;
  }

  /**
   * dP(r; s)/dns, where P(r;s) = exp(-ka * |r-s|)) * F(r;s), r is located on the triangle, and 
   * ns is the unit normal vector at s.
   */
  static real_t dPdns_(const position& r,
		       const position_t& s,
		       const normal_t& ns,
		       const real_t& ka)
  {
    auto pair = dFdns_(r, s, ns);
    real_t dFdns = std::get<0>(pair);
    length_t R = std::get<1>(pair);
    real_t kaR = ka * R;
    real_t exp = std::pow(E, -kaR);
    return (1.0 + kaR) * exp * dFdns;
  }

  /**
   * dP^2/dnsdn, where P(r;s) = exp(-ka * |r-s|) * F(r;s), n is the unit normal vector at r, and 
   * ns is the unit normal vector at s
   */
  static real_t d2Pdnsdn_(const position_t& r,
			  const normal_t& n,
			  const position_t& s,
			  const normal_t& ns
			  const real_t& ka)
  {
    auto tuple =  dF2dnsdn_(r, n, s, ns);
    real_t dF2dnsdn = std::get<0>(tuple);
    real_t R = std::get<1>(tuple);
    real_t imp_n_R = std::get<2>(tuple);
    real_t imp_ns_R = std::get<3>(tuple);
    real_t kaR = ka * R;
    real_t exp = std::pow(E, -kaR);
    real_t ka2 = ka * ka;
    return (1.0 + kaR) * exp * dF2dnsdn - ka2 * exp * imp_n_R * imp_ns_R /(FOUR_PI * R);
  }

  /**
   * Calculates L1 for zero ionic strength. Collocation points are vertices. 
   * Integration over surface assumes the kernel and potential to be constant across a 
   * triangle.
   * 
   */
  static void L1_0_(const TriangulatedSurface& surface,
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
	position_t r = tj.position();
	normal_t n = tj.normal();
	area_t area = tj.area();
	real_t dFdn = std::get<0>(dFdn_(r, n, r0));
	real_t lij = factor * dFdn * area;
	L(i,j) -= lij;
      }

      // Diagonal element.
      L(i,i) = 1.0;

      for (std::size_t j = i+1; j < ntr; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.position();
	normal_t n = tj.normal();
	area_t area = tj.area();
	real_t dFdn = std::get<0>(dFdn_(r0, tj));
	real_t lij = factor * dFdn * area;
	L(i,j) -= lij;
      }
      
    }
  }

  /**
   * Computes kernels L1, L2, L3, and L4 for given ionic strength.
   */
  static void L1234_(const TriangulatedSurface& surface,
		     const permittivity_t& epsI,
		     const permittivity_t& epsO,
		     const real_t& ka,
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
		   rvector_t& b)
  {
    const real_t ratio = epsO/epsI;
    const real_t factor1 = 2.0 / ((1.0 + ratio) * epsI * E0 * FOUR_PI);
    const real_t factor2 = 2.0 / ((1.0 + 1.0 / ratio) * epsI * E0 * FOUR_PI);

    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();

    std::size_t ncol = ntr * ntr;
    b = rvector_t(ncol);
    for (std::size_t i = 0; i != ncol; ++i) {
      b(i) = 0.0;
    }
    
    for (std::size_t i = 0; i != ntr; ++i) {
      const Triangle& triangle = triangles[i];
      normal_t n0 = triangle.normal();
      for (const Atom& atom : atoms) {
	position_t r = atom.position();
	auto pair = dFdn_(r, triangle);
	real_t dFdn = pair.first;
	length_t R ) pair.second;
	charge_t q = atom.charge();
	b(i) += factor * q / R;
	
      }
    }    
  }

  FlatTrianglesBEM::FlatTrianglesBEM(const permittivity_t& epsI,
				     const permittivity_t& epsO,
				     const ionic_strength_t& I) :
    BEM(), epsI_(epsI), epsO_(epsO), I_(I), ka_{}, indx_{}, luQR_{}
  {
    if ( epsI_ <= 0.0 || epsO_ <= 0.0 ) {
      throw std::domain_error("BEM: Permittivities must be non-negative numbers.");
    }
    if ( I_ < 0.0 ) {
      throw std::domain_error("BEM: Ionic strength cannot be a negative number.");
    }

    real_t R = MUUnits<real_t>::R;
    real_t F = MUUnits<real_t>::F;
    real_t T = SIUnits<real_t>::roomT;
    ka_ = 2.0 * I_ * F * F / ( epsO * MUUnits<real_t>::E0 * R * T * T);
  }

  void FlatTrianglesBEM::kernels(const TriangulatedSurface& surface)
  {
    matrix_t S;
    if ( I_ > 0.0 ) {
      L1234_(surface, epsI_, epsO_, ka_, S);
    } else {
      L1_0_(surface, epsI_, epsO_, S);
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
	auto dFdn = dFdn_(ri, triangle);
	potentials[i] += factor * dFdn.second * x[j] * area;
      }
    }
  }

  real_t FlatTrianglesBEM::inverseDebyeScreeningLength()
  {
    return ka_;
  }

}
