#ifndef EDGE_HPP
#define EDGE_HPP

#include "../types.hpp"
#include <iostream>

namespace simploce {

  class Edge {
  public:

    /**
     * Constructor. 
     * @param v1 First of two vertices.
     * @param v2 Second of two vertices.
     */    
    Edge(const vertex_ptr_t& v1, const vertex_ptr_t& v2);

    /**
     * Returns edle length.
     * @return Length.
     */
    length_t length() const;

    /**
     * Writes this edge to an output stream.  The identifiers of the two vertices are put into
     * the output stream.
     * @param stream - Output stream.
     * @return Output stream.
     */
    std::ostream& writeTo(std::ostream& stream) const;
    
  private:

    static id_t ID;

    vertex_ptr_t v1_;
    vertex_ptr_t v2_;
  };

  /**
   * Writes edge to output stream. The identifiers of the two vertices are put into
   * the output stream.
   * @param stream - Output stream.
   * @param edge - Edge.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const Edge& edge);
}


#endif
