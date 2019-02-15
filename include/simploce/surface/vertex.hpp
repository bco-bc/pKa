#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "../types.hpp"
#include "simploce/util/id.hpp"
#include <iostream>

namespace simploce {

  /**
   * An point on a surface. A unit normal vector may be associated with.
   */
  class Vertex {
  public:

    /**
     * Constructor.
     * @param r - Position. Its norm must be larger than 0.
     */
    Vertex(const position_t& r);

    /**
     * Constructor.
     * @param r - Position.
     * @oaram n - Unit normal vector at r.
     */
    Vertex(const position_t& r, const normal_t& n);

    /**
     * Returns unique identifier value.
     * @return Value > 0.
     */
    std::size_t id() const;
    
    /**
     * Returns position.
     */
    const position_t& position() const;

    /**
     * Returns unit normal vector.
     */
    const normal_t& normal() const;

    /**
     * Resets position and unit normal vector at r.
     */
    void reset(const position_t& r, const normal_t& n);

    /**
     * Returns vertex.
     */
    static vertex_ptr_t make(const position_t& r);

  private:

    /**
     * Identifier type.
     */
    using id_t = Id<Vertex, std::size_t>;

    id_t id_;
    position_t r_;
    normal_t n_;
  };

  /**
   * Writes vertex to output stream.
   * @param stream - Output stream.
   * @param v - Vertex
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const Vertex& v);
}

#endif
