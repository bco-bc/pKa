#include "simploce/types.hpp"
#include "simploce/factory.hpp"
#include "simploce/protein/file-input-source.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/pdb-reader.hpp"
#include "simploce/protein/gmx-reader.hpp"
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

enum Format { pdb, gmx, splc };

int main(int argc, char *argv[])
{
  std::string fnInputProtein{"1abc.pdb"};
  std::string fnOutputProtein{"protein.splc"};
  Format format{pdb};
  std::string fnOutputDottedSurface{"dotted.srf"};
  std::string fnOutputTriangulatedSurface{"triangulated.srf"};
  std::size_t ntriangles{240};
  bool spherical{false};
  radius_t radius{2.0};
  
  po::options_description usage("Usage");
  usage.add_options()
    ("fn-input-protein", po::value<std::string>(&fnInputProtein),
     "Input file name of protein structure. Default is 'protein.pdb'.")

    ("fn-output-protein", po::value<std::string>(&fnOutputProtein),
     "Output file name of protein structure. Default is 'protein.splc'. "
     "Note that if the input format is that of simploce (splc), then no output is produced.")
    ("fn-output-dotted-surface", po::value<std::string>(&fnOutputDottedSurface),
     "Output file name of dotted surface. Default is 'dotted.srf'.")
    ("fn-output-triangulated-surface", po::value<std::string>(&fnOutputTriangulatedSurface),
     "Output file name of triangulated surface. Default is 'triangulated.srf'.")

    ("pdb", "Assume PDB format for protein structure. This is the default.")
    ("gmx", "Assume GROMACS format for protein structure. ")
    ("splc", "Assume SIMPLOCE format for protein structure.")
    
    ("spherical", "Create a spherical triangulated surface.")
    ("radius", po::value<radius_t>(&radius), "Radius of spherical surface. Default is 2.0 nm.")

    ("number-of-triangles", po::value<std::size_t>(&ntriangles),
     "Requested number of triangles. Default is 240.")
    
    ("help", "Help message")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, usage), vm);
  po::notify(vm);

  if ( vm.count("help") || argc == 1) {
    std::cout << std::endl;
    std::cout << "Creates a triangulated surface around a protein." << std::endl;
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
  if ( vm.count("splc") ) {
    format = splc;
  }
  if ( vm.count("spherical") ) {
    spherical = true;
  }
  if ( vm.count("radius") ) {
    radius = vm["radius"].as<radius_t>();
  }
  if ( vm.count("number-of-triangles") ) {
    ntriangles = vm["number-of-triangles"].as<std::size_t>();
  }

  chem_reader_ptr_t chemReader;
  switch (format) {
    case pdb: {
      std::clog << fnInputProtein
		<< ": Assuming PDB format for this input protein structure." << std::endl;
      chemReader = PDBReader::make();
      break;
    }
    case gmx: {
      std::clog << fnInputProtein
		<< ": Assuming GROMACS format for this input protein structure." << std::endl;
      chemReader = GMXReader::make();
      break;
    }
    case splc: {
      // Do nothing.
      std::clog << fnInputProtein
		<< ": Assuming SIMPLOCE format for this input protein structure." << std::endl;
      break;
    }
    default: {
      std::clog << fnInputProtein
		<< ": Assuming PDB format for this input protein structure." << std::endl;
      chemReader = PDBReader::make();
      break;
    }
  };

  prot_struct_ptr_t protein;
  std::ofstream ostream;

  // Read chemical content.
  atom_catalog_ptr_t atomCatalog = factory::atomCatalog();
  if ( format != splc) {
    input_source_ptr_t inputSource = FileInputSource::make(fnInputProtein);
    std::shared_ptr<ProteinStructureContentHandler> contentHandler =
      ProteinStructureContentHandler::make(atomCatalog);
    chemReader->contentHandler(contentHandler);
    chemReader->parse(inputSource);
    protein = contentHandler->proteinStructure();
  } else {
    std::ifstream stream;
    util::openInputFile(stream, fnInputProtein);
    protein = ProteinStructure::make(stream, atomCatalog);
    stream.close();
  }
  std::clog << "Title: " << protein->title() << std::endl;
  std::clog << "Number of atoms: " << protein->numberOfAtoms() << std::endl;
  std::clog << "Number of atom groups: " << protein->numberOfAtomGroups() << std::endl;

  // Write protein structure to output file.
  if ( format != splc ) {
    util::openOutputFile(ostream, fnOutputProtein);
    ostream << *protein << std::endl;
    ostream.close();
    std::clog << "Protein strcuture written to output file '"
	      << fnOutputProtein << "'." << std::endl;
  }

  // Generate dotted surface.
  Surface surface = protein->dottedSurface();
  std::clog << "Surface area (nm^2): " << surface.area() << std::endl;
  std::clog << "Volume (nm^3): " << surface.volume() << std::endl;

  // Write surface to output file.
  util::openOutputFile(ostream, fnOutputDottedSurface);
  ostream << surface << std::endl;
  ostream.close();
  std::clog << "Dotted surface written to output file '"
	    << fnOutputDottedSurface << "'." << std::endl;

  // Triangulate.
  triangulator_ptr_t triangulator = factory::triangulator(ntriangles);
  TriangulatedSurface triangulatedSurface = surface.triangulate(triangulator, spherical, radius);
  
  // Write surface to output file.
  util::openOutputFile(ostream, fnOutputTriangulatedSurface);
  ostream << triangulatedSurface << std::endl;
  ostream.close();
  std::clog << "Triangulated surface written to output file '"
	    << fnOutputTriangulatedSurface << "'." << std::endl;
  
  return 0;
}
