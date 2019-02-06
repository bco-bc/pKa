#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "../types.hpp"

namespace simploce {

  /**
   * A triangle is defined by three vertices.
   */
  class Triangle {
  public:

    /**
     * Constructor.
     * @param v1 First of three vertices.
     * @param v2 Second of three vertices.
     * @param v3 Third of three vertices.
     */
    Triangle(const vertex_ptr_t& v1, const vertex_ptr_t& v2, const vertex_ptr_t& v3);

    /**
     * Returns a unit vector perpendicular to the flat surface by the three vertices. The 
     * orientation is defined by the vertices' unit vectors.
     */
    unitv_t unitNormal();

  private:

    vertex_ptr_t v1_;
    vertex_ptr_t v2_;
    vertex_ptr_t v3_;

  };
}

#endif
