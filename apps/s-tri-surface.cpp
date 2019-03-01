#include "simploce/types.hpp"
#include "simploce/factory.hpp"
#include "simploce/protein/file-input-source.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/pdb-reader.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/protein-structure.hpp"
#include "simploce/protein/protein-structure-content-handler.hpp"
#include "simploce/surface/surface.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"
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
  std::string fnOutputProtein{"protein.structure"};
  Format format{pdb};
  std::string fnOutputDottedSurface{"dotted.srf"};
  std::string fnOutputTriangulatedSurface{"triangulated.srf"};
  std::size_t ntriangles{240};
  bool spherical{false};
  
  po::options_description usage("Usage");
  usage.add_options()
    ("fn-input-protein", po::value<std::string>(&fnInputProtein),
     "Input file name of protein structure. Default is '1abc.pdb'.")

    ("fn-output-protein", po::value<std::string>(&fnOutputProtein),
     "Output file name of protein structure. Default is 'protein.structure'.")
    ("fn-output-dotted-surface", po::value<std::string>(&fnOutputDottedSurface),
     "Output file name of dotted surface. Default is 'dotted.srf'.")
    ("fn-output-triangulated-surface", po::value<std::string>(&fnOutputTriangulatedSurface),
     "Output file name of triangulated surface. Default is 'triangulated.srf'.")

    ("pdb", "Assume PDB format for protein structure. This is the default.")
    ("gmx", "Assume GROMACS format for protein structure. Not yet implemented.")
    ("spherical", "Create a spherical triangulated surface.")

    ("number-of-triangles", po::value<std::size_t>(&ntriangles),
     "Requested number of triangles. Default is 240.")
    
    ("help", "Help message")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, usage), vm);
  po::notify(vm);

  if ( vm.count("help") || argc == 1) {
    std::cout << std::endl;
    std::cout << usage << "\n";
    return 0;
  }

  if ( vm.count("fn-input-protein") ) {
    fnInputProtein = vm["fn-input-protein"].as<std::string>();
  }
  if ( vm.count("fn-output-protein") ) {
    fnOutputProtein = vm["fn-output-protein"].as<std::string>();
  }
  if ( vm.count("fn-output-dotted-surface") ) {
    fnOutputDottedSurface = vm["fn-output-dotted-surface"].as<std::string>();
  }
  if ( vm.count("fn-output-triangulated-surface") ) {
    fnOutputTriangulatedSurface = vm["fn-output-triangulated-surface"].as<std::string>();
  }
  if ( vm.count("pdb") ) {
    format = pdb;  
  }
  if ( vm.count("gmx") ) {
    format = gmx;
  }
  if ( vm.count("spherical") ) {
    spherical = true;
  }
  if ( vm.count("number-of-triangles") ) {
    ntriangles = vm["number-of-triangles"].as<std::size_t>();
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
      std::clog << "NOT IMPLEMENTED YET." << std::endl;
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
  ProteinStructure structure = contentHandler->proteinStructure();
  std::clog << "Title: " << structure.title() << std::endl;
  std::clog << "Number of atoms: " << structure.size() << std::endl;

  // Write protein structure to output file.
  openOutputFile(ostream, fnOutputProtein);
  ostream << structure << std::endl;
  ostream.close();
  std::clog << "Protein strcuture written to output file '"
	    << fnOutputProtein << "'." << std::endl;

  // Generate dotted surface.
  Surface surface = structure.dottedSurface();
  std::clog << "Surface area (nm^2): " << surface.area() << std::endl;
  std::clog << "Volume (nm^3): " << surface.volume() << std::endl;

  // Write surface to output file.
  openOutputFile(ostream, fnOutputDottedSurface);
  ostream << surface << std::endl;
  ostream.close();
  std::clog << "Dotted surface written to output file '"
	    << fnOutputDottedSurface << "'." << std::endl;

  // Triangulate.
  triangulator_ptr_t triangulator = Factory::triangulator(ntriangles);
  TriangulatedSurface triangulatedSurface = surface.triangulate(triangulator, spherical);
  
  // Write surface to output file.
  openOutputFile(ostream, fnOutputTriangulatedSurface);
  ostream << triangulatedSurface << std::endl;
  ostream.close();
  std::clog << "Triangulated surface written to output file '"
	    << fnOutputTriangulatedSurface << "'." << std::endl;
  
  return 0;
}
