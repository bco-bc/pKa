#include "simploce/surface/vertex.hpp"
#include "simploce/util/util.hpp"

namespace simploce {

  Vertex::Vertex(const position_t& r) :
    id_{}, r_{r}, n_{}
  {
    id_t::use(0,1);
    id_ = id_t::next();
    real_t R = norm<real_t>(r_);
    n_ = r_ / R;
  }

  Vertex::Vertex(const position_t& r, const normal_t& n) :
    id_{}, r_{r}, n_{n}
  {
    id_t::use(0,1);
    id_ = id_t::next();
  }

  std::size_t Vertex::id() const
  {
    return id_();
  }
  
}
