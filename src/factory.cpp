#include "simploce/factory.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/gmx-atom-group-catalog.hpp"
#include "simploce/surface/sphere-triangulator.hpp"
#include "simploce/util/util.hpp"
#include <fstream>

namespace simploce {
  namespace factory {

    static atom_catalog_ptr_t atomCatalog_{};
    static triangulator_ptr_t triangulator_{};
    static atom_group_catalog_ptr_t atomGroupCatalog_{};
    
    atom_catalog_ptr_t atomCatalog()
    {
      if ( !atomCatalog_) {
	atomCatalog_ = AtomCatalog::make();
      }
      return atomCatalog_;
    }
    
    triangulator_ptr_t triangulator(std::size_t ntriangles)
    {
      if ( !triangulator_ ) {
	triangulator_ = SphereTriangulator::make(ntriangles);
      }
      return triangulator_;
    }
    
    atom_group_catalog_ptr_t atomGroupCatalog(const std::string& fileName)
    {
      if ( !atomGroupCatalog_) {
	std::ifstream stream;
	util::openInputFile(stream, fileName);
	atomGroupCatalog_ = GMXAtomGroupCatalog::valueOf(stream);
	stream.close();
      }
      return atomGroupCatalog_;
    }

  }
}
