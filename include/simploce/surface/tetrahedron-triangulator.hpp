#ifndef TETRAHEDRON_TRIANGULATOR_HPP
#define TETRAHEDRON_TRIANGULATOR_HPP

#include "triangulator.hpp"

namespace simploce {

  class TetrahedronTriangulator : public Triangulator {    
  public:

    result_t generate(std::vector<vertex_ptr_t>& vertices) const;
    
  };
}

#endif
