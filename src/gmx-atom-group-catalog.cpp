#include "simploce/protein/gmx-atom-group-catalog.hpp"
#include "simploce/util/util.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <memory>

namespace simploce {

  static const std::string ATOMS = "atoms";

  void findAtoms_(std::istream& stream)
  {
    // Buffer to read line plus EOL.
    std::string buffer;

    // Reads line.
    std::getline(stream, buffer);
    bool atoms = buffer.find(ATOMS) != std::string::npos;
    while ( !atoms && stream.good() ) {
      buffer.clear();
      std::getline(stream, buffer);
      atoms = buffer.find(ATOMS) != std::string::npos;
    }
    if ( !stream.good() ) {
      throw std::domain_error("No '[ atoms ]' in topology.");
    }
  }

  static std::vector<atom_group_spec_ptr_t> findAtomGroups_(std::istream& stream)
  {
    std::vector<atom_group_spec_ptr_t> specs;

    return specs;
  }

  GMXAtomGroupCatalog::GMXAtomGroupCatalog() :
    AtomGroupCatalog()
  {
  }

  atom_group_catalog_ptr_t GMXAtomGroupCatalog::valueOf(std::istream& stream)
  {
    // Buffer to read line plus EOL.
    std::string buffer;
    
    // Located atoms in the topology.
    findAtoms_(stream);
    std::getline(stream, buffer);
    
    // We have reach the first atom group (usually a residue in the topolog).
    // Now find the atom groups.
    std::vector<atom_group_spec_ptr_t> specs = findAtomGroups_(stream);

    // Create the catalog
    atom_group_catalog_ptr_t catalog = std::make_shared<GMXAtomGroupCatalog>();
    for (auto& spec : specs) {
      catalog->add(spec);
    }

    return catalog;
  }
}
