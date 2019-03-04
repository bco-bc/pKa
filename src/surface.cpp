#include "simploce/surface/surface.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/conf.hpp"
#include <stdexcept>
#include <iostream>

namespace simploce {
 
  Surface::Surface(const std::vector<position_t>& points) :
    points_{points}, area_{0}, volume_{0}
  {
    if ( points_.empty() ) {
      throw std::domain_error("Surface: surface points must be provided.");
    }
  }

  Surface::Surface(std::vector<position_t>& points, const area_t& area, const volume_t& volume) :
     points_{points}, area_{area}, volume_{volume}
  {
    if ( points_.empty() ) {
      throw std::domain_error("Surface: surface points must be provided.");
    }    
  }

  TriangulatedSurface Surface::triangulate(const triangulator_ptr_t& triangulator,
					   bool spherical,
					   const radius_t& radius)
  {
    return triangulator->generate(points_, spherical, radius);
  }

  area_t Surface::area() const
  {
    return area_;
  }

  volume_t Surface::volume() const
  {
    return volume_;
  }

  std::ostream& Surface::writeTo(std::ostream& stream) const
  {
    stream.setf(std::ios::scientific);
    stream.precision(PRECISION);
    
    stream << points_.size() << std::endl;
    for (auto p : points_) {
      stream << p << std::endl;
    }
    stream << area_ << SPACE << volume_;
    
    return stream;
  }

  std::ostream& operator << (std::ostream& stream, const Surface& surface)
  {
    return surface.writeTo(stream);
  }
			 
}
