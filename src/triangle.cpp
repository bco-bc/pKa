#include "simploce/surface/triangle.hpp"
#include <stdexcept>

namespace simploce {

  Triangle::Triangle(const vertex_ptr_t& v1, const vertex_ptr_t& v2, const vertex_ptr_t& v3) :
    v1_{v1}, v2_{v2}, v3_{v3}
  {
    if ( !v1_|| !v2_ || !v3_ ) {
      throw std::domain_error("Triangle: Three vertices must be provided.");
    }
  }
    
}
