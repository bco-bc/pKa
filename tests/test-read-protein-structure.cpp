#include "simploce/protein/protein-structure.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/types.hpp"
#include "simploce/util/util.hpp"
#include "simploce/factory.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace simploce;

int main(int argc, char *argv[])
{
  if ( argc == 1 ) {
    throw std::domain_error("Missing input protein structure file name.");
  }
  std::string fileName(argv[1]);
  std::ifstream stream;
  util::openInputFile(stream, fileName);

  atom_catalog_ptr_t catalog = Factory::atomCatalog();

  prot_struct_ptr_t protein = ProteinStructure::make(stream, catalog);

  std::clog << *protein << std::endl;

}
