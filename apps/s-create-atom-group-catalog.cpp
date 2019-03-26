#include "simploce/protein/gmx-atom-group-catalog.hpp"
#include "simploce/types.hpp"
#include "simploce/factory.hpp"
#include "simploce/util/util.hpp"
#include <boost/program_options.hpp>
#include <stdexcept>

using namespace simploce;
namespace po = boost::program_options;

enum Format { gmx, splc };

int main(int argc, char *argv[])
{
  std::string fnInput{"protein.top"};
  std::string fnOutput{"protein.agrp"};
  Format format{gmx};

  po::options_description usage("Usage");
  usage.add_options()
    ("fn-input", po::value<std::string>(&fnInput),
     "Input file name. Default is 'protein.top'.")

    ("gmx", "Assume GROMACS format for protein topology (extension .top). " 
     "This is the default.")
    ("splc", "Assume SIMPLOCE format for atom groups.")
    
    ("fn-output", po::value<std::string>(&fnOutput),
     "Output file name. Default is 'protein.agrp'.")
    ;
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, usage), vm);
  po::notify(vm);

  if ( vm.count("help") || argc == 1) {
    std::cout << std::endl;
    std::cout << "Parses atom group information and converts this to an internal format."
	      << std::endl;
    std::cout << usage << "\n";
    return 0;
  }
  if ( vm.count("gmx") ) {
    format = gmx;
  }
  if ( vm.count("splc") ) {
    format = splc;
  }
  if ( vm.count("fn-input") ) {
    fnInput = vm["fn-input"].as<std::string>();
  }  
  if ( vm.count("fn-output") ) {
    fnOutput = vm["fn-output"].as<std::string>();
  }

  /**
   * Read the input file.
   */
  atom_group_catalog_ptr_t catalog;
  std::ifstream istream;
  util::openInputFile(istream, fnInput);
  if ( format == gmx) {
    std::clog << "Reading GROMACS topology from '" << fnInput << "'." << std::endl;
    catalog = GMXAtomGroupCatalog::valueOf(istream, factory::atomCatalog());
  } else if ( format == splc) {
    std::clog << "Reading SIMPLOCE atom group specifications from '"
	      << fnInput << "'." << std::endl;
    istream >> *catalog;
  } else {
    throw std::domain_error(format + ": Unknown format.");
  }
  istream.close();

  /**
   * Write the catalog to an output file.
   */
  std::clog << "Writing atom group specifications to '" << fnOutput << "'." << std::endl;
  std::ofstream ostream;
  util::openOutputFile(ostream, fnOutput);
  ostream << *catalog << std::endl;
  ostream.close();
  
  return 0;
}
