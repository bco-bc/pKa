#include "simploce/surface/triangle.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/util/cvector.hpp"
#include "simploce/conf.hpp"
#include <stdexcept>
#include <algorithm>

namespace simploce {

  Triangle::Triangle(const vertex_ptr_t& v1, const vertex_ptr_t& v2, const vertex_ptr_t& v3) :
    v1_{v1}, v2_{v2}, v3_{v3}
  {
    if ( !v1_|| !v2_ || !v3_ ) {
      throw std::domain_error("Triangle: Three vertices must be provided.");
    }
  }

  position_t Triangle::midpoint() const
  {
    return position_t{rmp_};
  }

  normal_t Triangle::normal() const
  {
    return normal_t{normal_};
  }

  /**
   * See <a href="https://en.wikipedia.org/wiki/Heron%27s_formula">Heron's formula</a>
   */
  area_t Triangle::area() const
  {
    return area_;
  }

  std::tuple<vertex_ptr_t, vertex_ptr_t, vertex_ptr_t> Triangle::vertices()
  {
    return std::make_tuple(v1_, v2_, v3_);
  }

  void Triangle::swapTwoVertices()
  {
    std::swap(v2_, v3_);
  }

  void Triangle::properties()
  {
    // Area.
    real_t R12 = norm<real_t>(v1_->position() - v2_->position());
    real_t R23 = norm<real_t>(v2_->position() - v3_->position());
    real_t R31 = norm<real_t>(v3_->position() - v1_->position());
    real_t s = (R12 + R23 + R31) / 2.0;
    area_ = std::sqrt(s * (s - R12) * (s - R23) * (s - R31));
    
    // Midpoint.
    rmp_ = (v1_->position() + v2_->position() + v3_->position() ) / 3.0;

    // Unit outward normal vector.
    dist_vect_t r12 = v2_->position() - v1_->position();  // From 1 to 2.
    dist_vect_t r13 = v3_->position() - v1_->position();  // From 1 to 3.
    normal_ = cross(r12, r13);
    normal_ /= norm<real_t>(normal_);    
  }

  std::ostream& Triangle::writeTo(std::ostream& stream) const
  {
    stream.setf(std::ios::scientific);
    stream.precision(PRECISION);
    stream << v1_->id() << SPACE << v2_->id() << SPACE << v3_->id();
    return stream;
  }

  std::ostream& operator << (std::ostream& stream, const Triangle& triangle)
  {
    return triangle.writeTo(stream);
  }
    
}
