#include "simploce/surface/edge.hpp"
#include <stdexcept>

namespace simploce {

  Edge::Edge(const vertex_ptr_t& v1, const vertex_ptr_t& v2) :
    v1_{v1}, v2_{v2}
  {
    if ( !v1_ || !v2_ ) {
      throw std::domain_error("Edge: Two vertices must be provided.");
    }
  }
  
}
