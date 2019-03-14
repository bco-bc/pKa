#include "simploce/types.hpp"
#include "simploce/factory.hpp"
#include "simploce/protein/file-input-source.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/pdb-reader.hpp"
#include "simploce/protein/gmx-reader.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/protein-structure.hpp"
#include "simploce/protein/protein-structure-content-handler.hpp"
#include "simploce/util/util.hpp"
#include "boost/program_options.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace simploce;
namespace po = boost::program_options;

enum Format { pdb, gmx };

int main(int argc, char *argv[])
{
  std::string fnInputProtein{"1abc.pdb"};
  std::string fnOutputProtein{"1abc.splc"};
  Format format{pdb};

  po::options_description usage("Usage");
  usage.add_options()
    ("fn-input-protein", po::value<std::string>(&fnInputProtein),
     "Input file name of protein structure. Default is '1abc.pdb'.")

    ("fn-output-protein", po::value<std::string>(&fnOutputProtein),
     "Output file name of protein structure. Default is '1abc.splc'.")

    ("pdb", "Assume PDB format for protein structure. This is the default.")
    ("gmx", "Assume GROMACS format for protein structure. Not yet implemented.")

    ("help", "Help message")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, usage), vm);
  po::notify(vm);

  if ( vm.count("help") || argc == 1) {
    std::cout << std::endl;
    std::cout << "Parses protein structure information and converts this to an internal format."
	      << std::endl;
    std::cout << usage << "\n";
    return 0;
  }

  if ( vm.count("fn-input-protein") ) {
    fnInputProtein = vm["fn-input-protein"].as<std::string>();
  }
  if ( vm.count("fn-output-protein") ) {
    fnOutputProtein = vm["fn-output-protein"].as<std::string>();
  }
  if ( vm.count("pdb") ) {
    format = pdb;  
  }
  if ( vm.count("gmx") ) {
    format = gmx;
  }
  input_source_ptr_t inputSource = FileInputSource::make(fnInputProtein);
  chem_reader_ptr_t chemReader;
  switch (format) {
    case pdb: {
      std::clog << inputSource->sourceId()
		<< ": Assuming PDB format for this input protein structure." << std::endl;
      chemReader = PDBReader::make();
      break;
    }
    case gmx: {
      std::clog << inputSource->sourceId()
		<< ": Assuming GROMACS format for this input protein structure." << std::endl;
      chemReader = GMXReader::make();
      break;
    }
    default: {
      chemReader = PDBReader::make();
      break;
    }
  };

  std::ofstream ostream;

  // Read chemical content.
  atom_catalog_ptr_t atomCatalog = Factory::atomCatalog();
  std::shared_ptr<ProteinStructureContentHandler> contentHandler =
    ProteinStructureContentHandler::make(atomCatalog);
  chemReader->contentHandler(contentHandler);
  chemReader->parse(inputSource);

  // Get protein structure
  prot_struct_ptr_t protein = contentHandler->proteinStructure();
  std::clog << "Title: " << protein->title() << std::endl;
  std::clog << "Number of atoms: " << protein->numberOfAtoms() << std::endl;
  std::clog << "Number of atom groups: " << protein->numberOfAtomGroups() << std::endl;

  // Write protein structure to output file.
  util::openOutputFile(ostream, fnOutputProtein);
  ostream << *protein << std::endl;
  ostream.close();
  std::clog << "Protein strcuture written to output file '"
	    << fnOutputProtein << "'." << std::endl;

  return 0;
}
