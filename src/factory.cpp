#include "simploce/factory.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/surface/sphere-triangulator.hpp"

namespace simploce {

  static atom_catalog_ptr_t atomCatalog_{};
  static triangulator_ptr_t triangulator_{};

  atom_catalog_ptr_t Factory::atomCatalog()
  {
    if ( !atomCatalog_) {
      atomCatalog_ = AtomCatalog::make();
    }
    return atomCatalog_;
  }

  triangulator_ptr_t Factory::triangulator()
  {
    if ( !triangulator_ ) {
      triangulator_ = SphereTriangulator::make(60);
    }
    return triangulator_;
  }
  
}
