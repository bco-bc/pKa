#ifndef TYPES_HPP
#define TYPES_HPP

#include "simploce/util/cvector.hpp"
#include <utility>
#include <memory>
#include <tuple>

namespace simploce {

  /**
   * Real number type.
   */
  using real_t = double;

  /**
   * Length type.
   */
  using length_t = real_t;

  /**
   * Area type.
   */
  using area_t = real_t;

  /**
   * Volume type.
   */
  using volume_t = real_t;

  /**
   * Radius type.
   */
  using radius_t = real_t;

  /**
   * Charge type.
   */
  using charge_t = real_t;

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
   * Dotted surface.
   */
  class Surface;

  /**
   * Triangulated surface.
   */
  class TriangulatedSurface;

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

  /**
   * Particle type.
   */
  using particle_t = std::tuple<position_t, radius_t, charge_t>;

  /**
   * Chemical content reader.
   */
  class ChemReader;

  /**
   * Chemical content reader pointer type.
   */
  using chem_reader_ptr_t = std::shared_ptr<ChemReader>;
  
  /**
   * Molecular content handler.
   */
  class ContentHandler;

  /**
   * Molecular content handler pointer type.
   */
  using content_handler_ptr_t = std::shared_ptr<ContentHandler>;

  /**
   * Molecular input source.
   */
  class InputSource;

  /**
   * Molecular input source pointer type.
   */
  using input_source_ptr_t = std::shared_ptr<InputSource>;
}

#endif
