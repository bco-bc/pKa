#ifndef TYPES_HPP
#define TYPES_HPP

#include "simploce/util/cvector.hpp"
//#include <boost/numeric/ublas/matrix.hpp>
//##include <boost/numeric/ublas/vector.hpp>
//##include <boost/numeric/ublas/io.hpp>
#include <Eigen/Dense>
#include <utility>
#include <memory>
#include <tuple>

namespace simploce {

  /**
   * Real number type.
   */
  using real_t = double;

  /**
   * Matrix type for linear algebra.
   */
  //using matrix_t = boost::numeric::ublas::matrix<real_t>;
  typedef Eigen::Matrix<real_t, Eigen::Dynamic, Eigen::Dynamic> matrix_t;

  /**
   * Vector type for linear algebra.
   */
  //using vector_t = boost::numeric::ublas::vector<real_t>;
  typedef Eigen::Matrix<real_t, Eigen::Dynamic, 1> rvector_t;
  typedef Eigen::Matrix<int,  Eigen::Dynamic, 1> ivector_t;
  
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
   * Type for (relative) permittivity.
   */
  using permittivity_t = real_t;

  /**
   * Ionic strength type.
   */
  using ionic_strength_t = real_t;

  /**
   * Electrostatic potential type.
   */
  using elec_pot_t = real_t;

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

  /**
   * Triangulator pointer type.
   */
  using triangulator_ptr_t = std::shared_ptr<Triangulator>;

  /**
   * Triangulated surface.
   */
  class TriangulatedSurface;

  /**
   * Triangulated surface pointer type.
   */
  using tri_surface_ptr_t = std::shared_ptr<TriangulatedSurface>;

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

  /**
   * Atom.
   */
  class Atom;

  /**
   * Atom pointer type.
   */
  using atom_ptr_t = std::shared_ptr<Atom>;

  /**
   * Atom group, such as a protein residue.
   */
  class AtomGroup;

  /**
   * Atom specification type.
   */
  class AtomSpec;

  /**
   * Atom specification pointer type.
   */
  using atom_spec_ptr_t = std::shared_ptr<AtomSpec>;

  /**
   * Atom group specification type.
   */
  class AtomGroupSpec;

  /**
   * Atom group specification pointer type.
   */  
  using atom_group_spec_ptr_t = std::shared_ptr<AtomGroupSpec>;

  /**
   * Atom specifications catalog.
   */
  class AtomCatalog;

  /**
   * Atom catalog pointer type.
   */
  using atom_catalog_ptr_t = std::shared_ptr<AtomCatalog>;

  /**
   * Atom group specification catalog.
   */
  class AtomGroupCatalog;

  /**
   * Atom group specification catalog pointer type.
   */
  using atom_group_catalog_ptr_t = std::shared_ptr<AtomGroupCatalog>;

  /**
   * Protein structure.
   */
  class ProteinStructure;

  /**
   * Protein structure pointer type.
   */
  using prot_struct_ptr_t = std::shared_ptr<ProteinStructure>;
}

#endif
