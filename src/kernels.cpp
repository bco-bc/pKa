#include "simploce/bem/kernels.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/util/math-constants.hpp"
#include "simploce/util/entity-range.hpp"
#include "simploce/util/util.hpp"
#include <future>
#include <thread>

namespace simploce {

  struct KernelResult {
    KernelResult(std::vector<real_t>& mtrx) : matrix{mtrx} {}
    std::vector<real_t> matrix;
  };

  using result_t = KernelResult;

  using entity_range_t = EntityRange<Triangle>;

  const real_t FOUR_PI = 4.0 * MathConstants<real_t>::PI;
  
  void result_t L0_(const std::vector<Triangle>& triangles,
		    const real_t& epsRatio, std::vector<real_t>& L)
  {
    std::size_t ntr = triangles.size();
    L.clear();
    L.resize(ntr * ntr, 0.0);
    
    for (std::size_t i = 0; i != ntr; ++i) {
      const Triangle& ti = triangles[i];
      position_t r0 = ti.midpoint();
      for (std::size_t j = ; j != ntr; ++j) {
	const Triangle& tj = triangles[j];
	position_t r = tj.midpoint();
	normal_t n = tj.normal();
	dist_vect_t disv = r - r0;
	real_t imp = inner<real_t>(disv, n);
	length_t R = norm<real_t>(disv);
	
	real_t t3 = 2.0 / (1.0 + epsRatio);
	real_t t4 = FOUR_PI * R;
	real_t t5 = t4 * R * R;
	real_t lij = t3 * ( 1.0 - epsRatio) * imp / t5;
	L[i * ntr + j] += lij;
      }
    }
    return result_t{L};
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

  void Kernels::L0(const TriangulatedSurface& surface, std::vector<real_t>& L)
  {
    const real_t epsRatio = epsO_ / epsI_; 
    const std::vector<Triangle>& triangles = surface.triangles();
    std::size_t ntr = triangles.size();
    std::vector<entity_range_t> ranges = createRanges(triangles);
    
    std::vector<std::future<result_t> > futures{};
    std::size_t ntasks = ranges.size() - 1;
    for (std::size_t k = 0; k != ntasks; ++k) {
      const entity_range_t& range = ranges[k];
      futures.push_back(
        std::async(std::launch::async,
		   L0_,
		   std::ref(triangles),
		   std::ref(range),
		   std::ref(epsRatio))
      );
    }

    // Wait for these tasks to complete in other threads.
    std::vector<result_t> results = wait_for_all<result_t>(futures);

    // One remaining task in current thread.
    const entity_range_t& range = ranges[ranges.size() -1];
    result_t result = L0_(triangles, range, epsRatio);
    results.push_back(result);

    L.clear();
    L.resize(ntr * ntr, 0.0);
    for (
  }
}
