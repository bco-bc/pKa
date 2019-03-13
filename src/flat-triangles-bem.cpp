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

/*
 * Any Eq(...) always refers to Juffer et al, J. Comput. Phys., v. 97, p. 144, 1991.
 */

namespace simploce {
  
  //using namespace boost::numeric::ublas;

  enum matrix_id_t {
    ML1, ML2, ML3, ML4
  };

  /**
   * For concurrent submatrices calculations.
   */
  struct Result {
    
    /**
     * Constructor.
     * @param matrix Calculated submatrix.
     * @param id - Identifies the submatrix, value is one of "L1", "L2", "L3", or "L4".
     */
    Result(const matrix_t& matrix, const matrix_id_t id) : matrix_(matrix), id_(id) {}

    bool is(matrix_id_t id) { return id_ == id; }
    
    matrix_t matrix_;
    matrix_id_t id_;
  };

  const real_t FOUR_PI = 4.0 * MathConstants<real_t>::PI;
  const real_t E = MathConstants<real_t>::E;
  const real_t E0 = MUUnits<real_t>::E0;

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
   * @return P(r,s), F(r,s)
   */
  static std::tuple<real_t, real_t> P_(const position_t r, const position_t s, const real_t& ka)
  {
    auto pair = F_(r,s);
    real_t F = std::get<0>(pair);
    real_t R = std::get<1>(pair);
    real_t kaR = ka * R;
    real_t exp = std::pow(E, -kaR);
    real_t P = exp * F;
    return std::make_tuple(P, F);
  }

  /**
   * dF(r; s)/dn, where F(r;s) = 1/(4*pi*|r-s|), where n is the unit normal vector at r.
   * @return dFdn and R=|r-s|.
   */
  static std::tuple<real_t, length_t> dFdn_(const position_t& r,
					    const normal_t& n,
					    const position_t& s)
  {
    dist_vect_t disv = r - s;
    real_t imp = inner<real_t>(n, disv);
    length_t R = norm<real_t>(disv);
    real_t dFdn = -imp / (FOUR_PI * R * R * R);
    return std::make_tuple(dFdn, R);
  }

  /**
   * dF(r; s)/dns, where F(r;s) = 1/(4*pi*|r-s|), where ns is the unit vector at s.
   * @return dFdns and |r-s|.
   */
  static std::tuple<real_t, length_t> dFdns_(const position_t& r,
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
   * @return dPdn, dFdF
   */
  static std::tuple<real_t, real_t> dPdn_(const position_t& r,
					  const position_t& n,
					  const position_t& s,
					  const real_t& ka)
  {
    auto pair = dFdn_(r, n, s);
    real_t dFdn = std::get<0>(pair);
    length_t R = std::get<1>(pair);
    real_t kaR = ka * R;
    real_t exp = std::pow(E, -kaR);
    real_t dPdn = (1.0 + kaR) * exp * dFdn;
    return std::make_tuple(dPdn, dFdn);
  }

  /**
   * dP(r; s)/dns, where P(r;s) = exp(-ka * |r-s|)) * F(r;s), r is located on the triangle, and 
   * ns is the unit normal vector at s.
   * @return dP/dns, dF/dns
   */
  static std::tuple<real_t, real_t> dPdns_(const position_t& r,
					   const position_t& s,
					   const normal_t& ns,
					   const real_t& ka)
  {
    auto pair = dFdns_(r, s, ns);
    real_t dFdns = std::get<0>(pair);
    length_t R = std::get<1>(pair);
    real_t kaR = ka * R;
    real_t exp = std::pow(E, -kaR);
    real_t dPdns = (1.0 + kaR) * exp * dFdns;
    return std::make_tuple(dPdns, dFdns);
  }

  /**
   * dP^2/dnsdn, where P(r;s) = exp(-ka * |r-s|) * F(r;s), n is the unit normal vector at r, and 
   * ns is the unit normal vector at s
   * @return d^2Pdnsdn, d^2Fdnsdn.
   */
  static std::tuple<real_t, real_t> d2Pdnsdn_(const position_t& r,
					      const normal_t& n,
					      const position_t& s,
					      const normal_t& ns,
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
    real_t dP2dnsdn = (1.0 + kaR) * exp * dF2dnsdn - ka2 * exp * imp_n_R * imp_ns_R /(FOUR_PI * R);
    return std::make_tuple(dP2dnsdn, dF2dnsdn);
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
    const real_t eps = epsO/epsI;
    const real_t factor = 2.0 * (eps - 1.0) / (1.0 + eps);

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
      
      // r0
      const Triangle& ti = triangles[i];      
      position_t r0 = ti.midpoint();

      // r, n
      for (std::size_t j = 0; j < i; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	normal_t n = tj.normal();
	area_t area = tj.area();
	auto pair = dFdn_(r, n, r0);
	real_t dFdn = std::get<0>(pair);
	real_t lij = factor * dFdn * area;
	L(i,j) -= lij;
      }

      // Diagonal element.
      L(i,i) = 1.0;

      // r, n
      for (std::size_t j = i+1; j < ntr; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	normal_t n = tj.normal();
	area_t area = tj.area();
	auto pair = dFdn_(r, n, r0);
	real_t dFdn = std::get<0>(pair);
	real_t lij = factor * dFdn * area;
	L(i,j) -= lij;
      }
      
    }
  }

  static Result L1_(const TriangulatedSurface& surface,
		    const permittivity_t& epsI,
		    const permittivity_t& epsO,
		    const real_t& ka)
  {
    real_t eps = epsO/epsI;
    real_t factor = 2.0 / ( 1.0 + eps );
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    
    matrix_t L1(ntr, ntr);
    for (std::size_t i = 0; i != ntr; ++i) {

      // r0
      const Triangle& ti = triangles[i];      
      position_t r0 = ti.midpoint();

      // r,n
      for (std::size_t j = 0; j != i; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	normal_t n = tj.normal();
	area_t area = tj.area();
	auto pair = dPdn_(r, n, r0, ka);
	real_t dPdn = std::get<0>(pair);
	real_t dFdn = std::get<1>(pair);
	real_t lij = factor * (eps * dPdn - dFdn) * area;
	L1(i,j) = -lij;
      }

      // Diagonal element.
      L1(i,i) = 1.0;

      // r,n
      for (std::size_t j = i + 1; j < ntr; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	normal_t n = tj.normal();
	area_t area = tj.area();
	auto pair = dPdn_(r,n, r0, ka);
	real_t dPdn = std::get<0>(pair);
	real_t dFdn = std::get<1>(pair);
	real_t lij = factor * (eps * dPdn - dFdn) * area;
	L1(i,j) = -lij;
      }
      
    }
    
    return Result{L1, ML1};
  }

  static Result L2_(const TriangulatedSurface& surface,
		    const permittivity_t& epsI,
		    const permittivity_t& epsO,
		    const real_t& ka)
  {
    const real_t eps = epsO/epsI;
    const real_t factor = 2.0 / ( 1.0 + eps );
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    
    matrix_t L2(ntr, ntr);
    for (std::size_t i = 0; i != ntr; ++i) {

      // r0
      const Triangle& ti = triangles[i];      
      position_t r0 = ti.midpoint();

      // r
      for (std::size_t j = 0; j != i; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	area_t area = tj.area();
	auto pair = P_(r, r0, ka);
	real_t P = std::get<0>(pair);
	real_t F = std::get<1>(pair);
	real_t lij = factor * (F - P) * area;
	L2(i,j) = -lij;
      }

      // Diagonal element.
      L2(i,i) = 0.0;

      // r
      for (std::size_t j = i+1; j < ntr; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	area_t area = tj.area();
	auto pair = P_(r, r0, ka);
	real_t P = std::get<0>(pair);
	real_t F = std::get<1>(pair);
	real_t lij = factor * (F - P) * area;
	L2(i,j) = -lij;
      }
      
    }
    
    return Result{L2, ML2};
  }

  static Result L3_(const TriangulatedSurface& surface,
		    const permittivity_t& epsI,
		    const permittivity_t& epsO,
		    const real_t& ka)
  {
    const real_t eps = epsO/epsI;
    const real_t factor = 2.0 / ( 1.0 + 1.0/eps );
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    
    matrix_t L3(ntr, ntr);
    for (std::size_t i = 0; i != ntr; ++i) {

      // r0, n0
      const Triangle& ti = triangles[i];      
      position_t r0 = ti.midpoint();
      normal_t n0 = ti.normal();

      // r, n
      for (std::size_t j = 0; j != i; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	normal_t n = tj.normal();
	area_t area = tj.area();
	auto pair = d2Pdnsdn_(r, n, r0, n0, ka);
	real_t dP2dn0dn = std::get<0>(pair);
	real_t dF2dn0dn = std::get<1>(pair);
	real_t lij = factor * (dP2dn0dn - dF2dn0dn) * area;
	L3(i,j) = -lij;
      }

      // Diagonal element.
      L3(i,i) = 0.0;

      // r, n
      for (std::size_t j = i + 1; j < ntr; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	normal_t n = tj.normal();
	area_t area = tj.area();
	auto pair = d2Pdnsdn_(r, n, r0, n0, ka);
	real_t dP2dn0dn = std::get<0>(pair);
	real_t dF2dn0dn = std::get<1>(pair);
	real_t lij = factor * (dP2dn0dn - dF2dn0dn) * area;
	L3(i,j) = -lij;
      }      

    }
    
    return Result{L3, ML3};
  }

  static Result L4_(const TriangulatedSurface& surface,
		    const permittivity_t& epsI,
		    const permittivity_t& epsO,
		    const real_t& ka)
  {
    const real_t eps = epsO/epsI;
    const real_t factor = 2.0 / ( 1.0 + 1.0/eps );
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    
    matrix_t L4(ntr, ntr);
    for(std::size_t i = 0; i != ntr; ++i) {
      
      // r0, n0
      const Triangle& ti = triangles[i];      
      position_t r0 = ti.midpoint();
      normal_t n0 = ti.normal();

      // r
      for (std::size_t j = 0; j != i; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	area_t area = tj.area();
	auto pair = dPdns_(r, r0, n0, ka);
	real_t dPdn0 = std::get<0>(pair);
	real_t dFdn0 = std::get<1>(pair);
	real_t lij = factor * (dFdn0 - dPdn0) * area;
	L4(i,j) = -lij;
      }

      // Diagonal element
      L4(i,i) = 1.0;

      // r
      for (std::size_t j = i + 1; j < ntr; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	area_t area = tj.area();
	auto pair = dPdns_(r, r0, n0, ka);
	real_t dPdn0 = std::get<0>(pair);
	real_t dFdn0 = std::get<1>(pair);
	real_t lij = factor * (dFdn0 - dPdn0) * area;
	L4(i,j) = -lij;
      }
    }
      
    return Result{L4, ML4};
  }

  /**
   * Computes kernels L1, L2, L3, and L4 for given ionic strength.
   */
  static void L1234_(const TriangulatedSurface& surface,
		     const permittivity_t& epsI,
		     const permittivity_t& epsO,
		     const real_t& ka,
		     matrix_t& S)
  {
    std::clog << "Computing S...";
    
    // There are 4 submatrices, each of which will be computed in a separate thread.
    std::vector<Result> results{};
    std::vector<std::future<Result>> futures{};

    // Three threads...
    futures.push_back(std::async(std::launch::async,
				 L1_,
				 std::ref(surface),
				 std::ref(epsI),
				 std::ref(epsO),
				 std::ref(ka)));
    futures.push_back(std::async(std::launch::async,
				 L2_,
				 std::ref(surface),
				 std::ref(epsI),
				 std::ref(epsO),
				 std::ref(ka)));
    futures.push_back(std::async(std::launch::async,
				 L3_,
				 std::ref(surface),
				 std::ref(epsI),
				 std::ref(epsO),
				 std::ref(ka)));
    results = util::wait_for_all<Result>(futures);

    // ...plus the current thread.
    Result result = L4_(surface, epsI, epsO, ka);
    results.push_back(result);

    // Merge 4 submatrices into a single one S of size 2*ntr x 2*ntr.
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    std::size_t size = 2 * ntr;
    S.resize(size, size);

    for ( auto& result : results) {
      matrix_id_t id = result.id_;
      matrix_t& matrix = result.matrix_;
      switch (id) {
        case ML1: {
	  for (std::size_t i = 0; i != ntr; ++i) {
	    for (std::size_t j = 0; j != ntr; ++j) {
	      S(i, j) = matrix(i,j);
	    }
	  }
	  break;
        }
        case ML2: {
	  for (std::size_t i = 0; i != ntr; ++i) {
	    for (std::size_t j = 0; j != ntr; ++j) {
	      S(i, j + ntr) = matrix(i,j);
	    }
	  }
	  break;
	}
        case ML3: {
	  for (std::size_t i = 0; i != ntr; ++i) {
	    for (std::size_t j = 0; j != ntr; ++j) {
	      S(i + ntr, j) = matrix(i,j);
	    }
	  }
	  break;
	}
        case ML4: {
	  for (std::size_t i = 0; i != ntr; ++i) {
	    for (std::size_t j = 0; j != ntr; ++j) {
	      S(i + ntr, j + ntr) = matrix(i,j);
	    }
	  }
	  break;
        }
        default: {
	  throw std::domain_error("Illegal submatrix identifier.");
	  break;
        }
      }
    }

    std::clog << "Done." << std::endl;

  }

  /**
   * Right hand side for zero ionic strength.
   * See Eq. (2.24), sum over charges (sources).
   */
  static void RHS0_(const std::vector<Atom>& atoms,
		    const TriangulatedSurface& surface,
		    const permittivity_t& epsI,
		    const permittivity_t& epsO,
		    rvector_t& b)
  {
    const real_t eps = epsO/epsI;
    const real_t factor = 2.0 / ((1.0 + eps) * epsI * E0);
      
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();

    // Initialize rhs with zeros.
    b = rvector_t(ntr);
    for (std::size_t i = 0; i != ntr; ++i) {
      b(i) = 0.0;
    }

    for (std::size_t i = 0; i != ntr; ++i) {

      // r0
      const Triangle& triangle = triangles[i];
      position_t r0 = triangle.midpoint();

      // ri
      for (const Atom& atom : atoms) {
	position_t ri = atom.position();
	charge_t qi = atom.charge();
	auto pair = F_(ri, r0);
	real_t F = std::get<0>(pair);
	b(i) += factor * qi * F;
      }
    }
  }

  /**
   * Right hand side, non-zero ionic strength.
   * Eqs (2.12) and (2.16) (or (2.17) and (2.18)).
   */
  static void RHS_(const std::vector<Atom>& atoms,
		   const TriangulatedSurface& surface,
		   const permittivity_t& epsI,
		   const permittivity_t& epsO,
		   const ionic_strength_t& I,
		   rvector_t& b)
  {
    const real_t eps = epsO/epsI;
    const real_t factor1 = 2.0 / ((1.0 + eps) * epsI * E0);
    const real_t factor2 = 2.0 / ((1.0 + 1.0 / eps) * epsI * E0);

    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();

    // // Initialize rhs with zeros.
    std::size_t ncol = 2 * ntr;
    b = rvector_t(ncol);
    for (std::size_t i = 0; i != ncol; ++i) {
      b(i) = 0.0;
    }

    for (std::size_t i = 0; i != ntr; ++i) {

      // r0, n0
      const Triangle& triangle = triangles[i];
      position_t r0 = triangle.midpoint();
      normal_t n0 = triangle.normal();

      // ri
      for (const Atom& atom : atoms) {
	position_t ri = atom.position();
	charge_t qi = atom.charge();

	// Eq. (2.12 or 2.17).
	auto pair1 = F_(ri, r0);
	real_t F = std::get<0>(pair1);
	b(i) += factor1 * qi * F;

	// Eq (2.16 or 2.18)
	auto pair2 = dFdns_(ri, r0, n0);
	real_t dFdn0 = std::get<0>(pair2);
	b(ntr + i) += factor2 * qi * dFdn0;
	
      }
    }
  }

  /**
   * Zero ionic strength, Eq (2.25).
   */
  static void integrate0_(const TriangulatedSurface& surface,
			  const rvector_t& x,
			  const std::vector<position_t> positions,
			  const permittivity_t& epsI,
			  const permittivity_t& epsO,
			  std::vector<elec_pot_t>& potentials)
  {
    const real_t eps = epsO/epsI;
    const real_t factor = eps - 1.0;

    potentials.clear();
    potentials.resize(positions.size(), 0.0);

    const std::vector<Triangle>& triangles = surface.triangles();
    for (std::size_t i = 0; i != positions.size(); ++i) {

      // r0, inside surface.
      const position_t& r0 = positions[i];

      // r, n.
      for (std::size_t j = 0; j != triangles.size(); ++j) {
	const Triangle& triangle = triangles[j];
	const position_t r = triangle.midpoint();
	const normal_t n = triangle.normal();
	area_t area = triangle.area();
	auto pair = dFdn_(r, n, r0);
	real_t dFdn = std::get<0>(pair);
	potentials[i] += factor * dFdn * x[j] * area;
      }
    }
  }

  /**
   * Non-zero ionic strength.
   */
  static void integrate_(const TriangulatedSurface& surface,
			 const rvector_t& x,
			 const std::vector<position_t> positions,
			 const permittivity_t& epsI,
			 const permittivity_t& epsO,
			 const real_t& ka,
			 std::vector<elec_pot_t>& potentials)
  {
    potentials.clear();
    potentials.resize(positions.size(), 0.0);

    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    
    for (std::size_t i = 0; i != positions.size(); ++i) {

      // r0, inside surface.
      const position_t& r0 = positions[i];

      // r, n.
      for (std::size_t j = 0; j != ntr; ++j) {	
	const Triangle& triangle = triangles[j];
	const position_t r = triangle.midpoint();
	const normal_t n = triangle.normal();
	area_t area = triangle.area();

	auto pair1 = F_(r, r0);
	real_t F = std::get<0>(pair1);
	auto pair2 = dFdn_(r, n, r0);
	real_t dFdn = std::get<0>(pair2);
	
	potentials[i] += (F * x(ntr + j) - dFdn * x[j]) * area;
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

    real_t F = MUUnits<real_t>::F;
    real_t R = MUUnits<real_t>::R;
    real_t T = SIUnits<real_t>::roomT;
    real_t NA = SIUnits<real_t>::NA;
    real_t E0 = MUUnits<real_t>::E0;
    real_t l_to_nm3 = MUUnits<real_t>::l_to_nm3;

    // Debye length, in nm.
    real_t lD = std::sqrt( (l_to_nm3 * epsO * E0 * R * T * NA) / (2.0 * I * F * F) );

    // Inverse Debye length, in 1/nm.
    ka_ = 1.0 / lD;
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
    
    std::clog << "LU decomposition...";
    luQR_ = S.colPivHouseholderQr();
    std::clog << "Done." << std::endl;
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
    if ( I_ > 0.0 ) {
      integrate_(surface, x, positions, epsI_, epsO_, ka_, potentials);
    } else {
      integrate0_(surface, x, positions, epsI_, epsO_, potentials);
    }
  }

  real_t FlatTrianglesBEM::inverseDebyeScreeningLength()
  {
    return ka_;
  }

}
