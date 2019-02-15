#include "simploce/surface/vertex.hpp"
#include "simploce/util/util.hpp"
#include "simploce/conf.hpp"
#include <cmath>
#include <stdexcept>

namespace simploce {

  Vertex::Vertex(const position_t& r) :
    id_{id_t::next()}, r_{r}, n_{}
  {
    real_t R = norm<real_t>(r_);
    if ( R < SMALL ) {
      throw std::domain_error("Vertex: position vector must have a nonzero norm.");
    }
    n_ = r_ / R;
  }

  Vertex::Vertex(const position_t& r, const normal_t& n) :
    id_{id_t::next()}, r_{r}, n_{n}
  {
    real_t length = norm<real_t>(n_);
    if ( std::fabs(length - 1.0) > SMALL ) {
      std::clog << "Norm normal: " << length << std::endl;
      throw std::domain_error(
        "Vertex: length of normal unit vector must be 1."
      );
    }
  }

  const position_t& Vertex::position() const
  {
    return r_;
  }

  const normal_t& Vertex::normal() const
  {
    return n_;
  }

  std::size_t Vertex::id() const
  {
    return id_();
  }

  vertex_ptr_t Vertex::make(const position_t& r)
  {
    return std::make_shared<Vertex>(r);
  }

  std::ostream& operator << (std::ostream& stream, const Vertex& v)
  {
    stream.setf(std::ios::scientific);
    stream.precision(PRECISION);
    stream << std::setw(WIDTH) << v.id() << SPACE << v.position() << SPACE << v.normal();
    return stream;
  }
  
}
