#ifndef EDGE_HPP
#define EDGE_HPP

#include "../types.hpp"

namespace simploce {

  class Edge {
  public:

    /**
     * Constructor. 
     * @param v1 First of two vertices.
     * @param v2 Second of two vertices.
     */    
    Edge(const vertex_ptr_t& v1, const vertex_ptr_t& v2);
    
  private:

    vertex_ptr_t v1_;
    vertex_ptr_t v2_;
  };
}


#endif
