#ifndef TYPES_HPP
#define TYPES_HPP

#include "util/cvector.hpp"
#include <utility>
#include <memory>

namespace simploce {

  using real_t = double;

  /**
   * Position type.
   */
  using position_t = cvector<real_t, 1>;

  /**
   * Unit vector type. Vector with norm 1.
   */
  using unitv_t = position_t;

  /**
   * Normal unit vector.
   */
  using normal_t = unitv_t;

  /**
   * Vertex type. Holds a position and a normal vector.
   */
  using vertex_t = std::pair<position_t, unitv_t>;

  /**
   * Vertex pointer type.
   */
  using vertex_ptr_t = std::shared_ptr<vertex_t>;

}

#endif
