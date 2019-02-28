#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "../types.hpp"
#include <iostream>

namespace simploce {

  /**
   * A triangle is defined by three vertices.
   */
  class Triangle {
  public:

    /**
     * Constructor. Arguments point to vertices in a larger set.
     * @param v1 First of three vertices.
     * @param v2 Second of three vertices.
     * @param v3 Third of three vertices.
     */
    Triangle(const vertex_ptr_t& v1, const vertex_ptr_t& v2, const vertex_ptr_t& v3);

    /**
     * Returns mid point of triangle. This assumes the triangle to be flat.
     */
    position_t midpoint() const;

    /**
     * Returns a unit vector perpendicular to the flat surface defined by the three vertices. The 
     * orientation is defined by the vertices' unit vectors.
     */
    normal_t normal() const;

    /**
     * Returns this triangle's area. This considers the triangle to be flat.
     * @return Area.
     */
    area_t area() const;

    /**
     * Writes this triangle to output stream. The identifiers of the three vertices are put into
     * the output stream.
     * @param stream - Output stream. 
     * @return Output stream.
     */
    std::ostream& writeTo(std::ostream& stream) const;

  private:

    vertex_ptr_t v1_;
    vertex_ptr_t v2_;
    vertex_ptr_t v3_;

  };

  /**
   * Writes triangle to output stream. The identifiers of the three vertices are put into
   * the output stream.
   * @param stream - Output stream.
   * @param triangle - Triangle.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const Triangle& triangle);
}

#endif
