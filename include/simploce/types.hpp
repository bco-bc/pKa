#ifndef TYPES_HPP
#define TYPES_HPP

#include "util/cvector.hpp"
#include <utility>
#include <memory>

namespace simploce {

  /**
   * Real number type.
   */
  using real_t = double;

  /**
   * Leangth type.
   */
  using length_t = real_t;

  /**
   * Area type.
   */
  using area_t = real_t;

  /**
   * Position type.
   */
  using position_t = cvector<real_t, 1>;

  /**
   * Distance (difference) type
   */
  using dist_vect_t = position_t;

  /**
   * Unit vector type. Vector with norm 1.
   */
  using unitv_t = position_t;

  /**
   * Normal unit vector.
   */
  using normal_t = unitv_t;

  /**
   * Triangle
   */
  class Triangle;

  /**
   * Edge
   */
  class Edge;

  /**
   * Vertex type. Holds a position and a normal vector.
   */
  class Vertex;
  using vertex_t = Vertex;

  /**
   * Vertex pointer type.
   */
  using vertex_ptr_t = std::shared_ptr<vertex_t>;

  /**
   * Triangulator type.
   */
  class Triangulator;
  using triangulator_t = Triangulator;

  /**
   * Triangulator pointer type.
   */
  using triangular_ptr_t = std::shared_ptr<triangulator_t>;
  

}

#endif
