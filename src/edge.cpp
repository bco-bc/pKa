#include "simploce/surface/edge.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/conf.hpp"
#include <stdexcept>

namespace simploce {

  Edge::Edge(const vertex_ptr_t& v1, const vertex_ptr_t& v2) :
    v1_{v1}, v2_{v2}
  {
    if ( !v1_ || !v2_ ) {
      throw std::domain_error("Edge: Two vertices must be provided.");
    }
  }

  length_t Edge::length() const
  {
    return norm<length_t>(v1_->position() - v2_->position());
  }

  std::ostream& Edge::writeTo(std::ostream& stream) const
  {
    stream.setf(std::ios::scientific);
    stream.precision(PRECISION);
    stream << v1_->id() << SPACE << v2_->id();
    return stream;
  }

  std::ostream& operator << (std::ostream& stream, const Edge& edge)
  {
    return edge.writeTo(stream);
  }
}
