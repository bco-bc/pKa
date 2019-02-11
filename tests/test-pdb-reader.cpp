#include "simploce/protein/pdb-reader.hpp"
#include "simploce/protein/logger-content-handler.hpp"
#include "simploce/protein/file-input-source.hpp"
#include "simploce/types.hpp"
#include <string>
#include <memory>

using namespace simploce;

int main(int argc, char *argv[])
{
  if ( argc == 1 ) {
    throw std::domain_error("Missing input PDB file name.");
  }
  
  std::string fileName(argv[1]);
  input_source_ptr_t inputSource = std::make_shared<FileInputSource>(fileName);

  chem_reader_ptr_t pdbReader = std::make_shared<PDBReader>();
  std::shared_ptr<LoggerContentHandler> logger = std::make_shared<LoggerContentHandler>();
  pdbReader->contentHandler(logger);

  pdbReader->parse(inputSource);

  std::clog << "Number of protein molecules: " << logger->numberOfMolecules() << std::endl;
  std::clog << "Number of atom groups: " << logger->numberOfAtomGroups() << std::endl;
  std::clog << "Number of atoms: " << logger->numberOfAtoms() << std::endl;
  std::clog << "Total charge: " << logger->totalCharge() << std::endl;
  
}
