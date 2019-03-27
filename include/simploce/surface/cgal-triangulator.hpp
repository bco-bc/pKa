#ifndef CGAL_TRIANGULATOR_HPP
#define CGAL_TRIANGULATOR_HPP

#include "triangulator.hpp"

namespace simploce {

  /**
   * Create a triangulated surface on cloud points.
   * The triangulated surface is computed using Delaunay Triangulation
   * and Alpha Shape from CGAL.  
   */
  struct CgalTriangulator : public Triangulator {    

    TriangulatedSurface generate(const std::vector<position_t>& points,
				 bool spherical = false,
				 const radius_t& radius = 1.0) const;

    /**
     * Returns triangulator.
     * @param Requested number of triangles. Ignored.
     */
    static triangulator_ptr_t make(std::size_t ntriangles);
    
  };
}

#endif
