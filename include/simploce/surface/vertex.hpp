#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "../types.hpp"
#include "../util/util.hpp"
#include "../util/id.hpp"

namespace simploce {

  class Vertex {
  public:

    /**
     * Constructor. Normal vector is taken to be parallel to position vector.
     * @param r - Position. Its norm must be larger than 0.
     */
    Vertex(const position_t& r);

    /**
     * Contructor.
     * @param r - Position.
     * @oaram n - Unit normal vector.
     */
    Vertex(const position_t& r, const normal_t& n);

    /**
     * Returns unique identifier.
     * @return Value > 0.
     */
    std::size_t id() const;
    
  private:

    using id_t = Id<Vertex,std::size_t>;

    id_t id_;
    position_t r_;
    normal_t n_;
  };
}

#endif
