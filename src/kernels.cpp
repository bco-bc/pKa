#include "simploce/bem/kernels.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/util/math-constants.hpp"
#include "simploce/util/entity-range.hpp"
#include "simploce/util/util.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <future>
#include <thread>

namespace simploce {

  struct KernelResult {
    KernelResult(std::vector<real_t>& mtrx) : matrix{mtrx} {}
    std::vector<real_t>& matrix;
  };

  using result_t = KernelResult;
  using entity_range_t = EntityRange<Triangle>;

  const real_t FOUR_PI = 4.0 * MathConstants<real_t>::PI;

  static real_t L0_ij_(const position_t& ri, const Triangle& triangle)
  {
    position_t rj = triangle.midpoint();
    area_t area = triangle.area();
    normal_t nj = triangle.normal();
    dist_vect_t rji = rj - ri;
    real_t imp = inner<real_t>(rji, nj);
    length_t R = norm<real_t>(rji);    
    real_t t3 = FOUR_PI * R * R * R;
    real_t lij = area * imp / t3;
    return lij;
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

    // Initialize kernel.
    std::size_t ntr = triangles.size();
    L.resize(ntr, ntr, false);

    // Compute kernel elements.
    for (std::size_t i = 0; i != ntr; ++i) {
      const Triangle& ti = triangles[i];
      position_t ri = ti.midpoint();
      
      for (std::size_t j = 0; j != i; ++j) {
	const Triangle& tj = triangles[j];
	real_t lij = -factor * L0_ij_(ri, tj);
	L(i,j) = -lij;
      }

      L(i,i) = 1.0;

      // Keep 'j < ntr'!
      for (std::size_t j = i+1; j < ntr; ++j) {
	const Triangle& tj = triangles[j];
	real_t lij = -factor * L0_ij_(ri, tj);
	L(i,j) = -lij;
      }
      
    }
  }

  Kernels::Kernels(const permittivity_t& epsI,
		   const permittivity_t& epsO,
		   const ionic_strength_t& I) :
    epsI_(epsI), epsO_(epsO), I_(I)
  {
    if ( epsI_ <= 0.0 || epsO_ <= 0.0 ) {
      throw std::domain_error("Kernels: permittivities must be non-negative numbers.");
    }
    if ( I_ < 0.0 ) {
      throw std::domain_error("Kernels: ionic strength cannot be a negative number.");
    }
  }

  void Kernels::L0(const TriangulatedSurface& surface, matrix_t& L)
  {
    L0_(surface, epsI_, epsO_, L);
  }

}
