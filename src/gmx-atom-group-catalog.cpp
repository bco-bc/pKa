#include "simploce/protein/gmx-atom-group-catalog.hpp"
#include "simploce/protein/atom-group-spec.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/util/util.hpp"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <memory>

namespace simploce {

  static const std::string ATOMS = "[ atoms ]";
  static const std::string RESIDUE = "; residue";

  // Titrating sites in proteins.
  static const std::string ASP = "ASP";
  static const std::string GLU = "GLU";
  static const std::string LYS = "LYS";
  static const std::string ARG = "ARG";
  static const std::string CYS = "CYS";
  static const std::string HIS = "HIS";
  static const std::string TYR = "TYR";

  // Other
  // ...

  static const std::vector<std::string> ALL{ASP, GLU, LYS, ARG, CYS, HIS, TYR};

  static std::size_t protonationState_(const std::string &name, const charge_t& q)
  {
    std::size_t occupancy{0};
    if ( name.find(GLU) != std::string::npos || name.find(ASP) != std::string::npos ) {
      occupancy = q < 0 ? 0 : 1;
    } else if ( name.find(ARG) != std::string::npos || name.find(LYS) != std::string::npos ) {
      occupancy = q > 0 ? 1 : 0;
    } else if ( name.find(CYS) != std::string::npos ) {
      occupancy = q < 0 ? 0 : 1; 
    } else if ( name.find(HIS) != std::string::npos ) {
      if ( q > 0 ) {
	occupancy = 2;
      } else {
	occupancy = 1;
      }
    } else if ( name.find(TYR) != std::string::npos ) {
      occupancy = q < 0 ? 0 : 1;
    } else {
      std::clog << "NOTE: '" << name << "': Not a titrating site. Occupancy always 0." << std::endl;
    }
    return occupancy;
  }

  static void findAtoms_(std::istream& stream)
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
      std::string msg = "'" + ATOMS + "': Not found in topology.";
      throw std::domain_error(msg);
    }
  }

  static std::vector<atom_group_spec_ptr_t> findAtomGroups_(std::istream& stream,
							    const atom_catalog_ptr_t& atomCatalog)
  {
    const char SPACE = ' ';
    
    // Buffer to read line plus EOL.
    std::string buffer;
    
    std::vector<atom_group_spec_ptr_t> gspecs;
    std::vector<atom_spec_ptr_t> aspecs;

    std::getline(stream, buffer);
    bool residue = buffer.find(RESIDUE) != std::string::npos;
    while ( residue && stream.good() && !buffer.empty() ) {
      aspecs.clear();

      // Get name
      std::string name{};
      std::size_t index{22};
      char ch = buffer[index];
      while ( ch != SPACE ) {
	name.push_back(ch);
	ch = buffer[++index];
      }

      // Get total charge
      std::string qs;
      index = 28;
      ch = buffer[index];
      while ( ch == SPACE ) {
	ch = buffer[++index];
      }
      qs = buffer.substr(index);
      charge_t charge = boost::lexical_cast<charge_t, std::string>(qs);

      // Get protonation state.
      std::size_t occupancy = protonationState_(name, charge);

      // Get all atom specifications for this group.
      std::getline(stream, buffer);
      residue = buffer.find(RESIDUE) != std::string::npos;
      while ( !residue && stream.good() && !buffer.empty() ) {
	// Line holds a single atom specification.
	
	// Get the specification name.
	std::string specName{};
	index = 31;
	char ch = buffer[index];
	while ( ch == SPACE ) {
	  ch = buffer[++index];
	}
	while ( ch != SPACE ) {
	  specName.push_back(ch);
	  ch = buffer[++index];
	}
	
	// Get the charge value.
	index = 45;
	ch = buffer[index];
	while (ch == SPACE) {
	  ch = buffer[++index];
	}
	qs = "";
	while ( ch != SPACE) {
	  qs.push_back(ch);
	  ch = buffer[++index];
	}
	charge = boost::lexical_cast<charge_t, std::string>(qs);
	
	// Get the mass value.
	std::string ms{};
	while ( ch == SPACE ) {
	  ch = buffer[++index];
	}
	while ( ch != SPACE ) {
	  ms.push_back(ch);
	  ch = buffer[++index];
	}
	mass_t mass = boost::lexical_cast<mass_t, std::string>(ms);
	
	// Get radius value
	radius_t radius = atomCatalog->lookup(specName.substr(0, 1))->radius();
	
	// Creata an atom specification.
	atom_spec_ptr_t aspec = AtomSpec::make(specName, radius, charge, mass);
	aspecs.push_back(aspec);
	
	// Next line
	std::getline(stream, buffer);
	residue = buffer.find(RESIDUE) != std::string::npos;
      }
      atom_group_spec_ptr_t gspec = AtomGroupSpec::make(name, occupancy, aspecs);
      gspecs.push_back(gspec);
      
      // Next residue?
      residue = buffer.find(RESIDUE) != std::string::npos;
    }    
    if ( gspecs.empty() ) {
      throw std::domain_error("No atom group specifications found.");
    }
    
    return gspecs;
  }

  /**
   * Ensure that the number of atoms or atom specifications per atom group is the same for
   * each protonation state. 
   */
  static void harmonize_(std::vector<atom_group_spec_ptr_t>& specs)
  {
    // All protonation states for a given group;
    std::vector<atom_group_spec_ptr_t> states{};
    
    for (auto residue : ALL) {
      states.clear();

      // Find all states.
      for (auto spec : specs) {
	std::string name = spec->name();
	if ( name.find(residue) != std::string::npos) {
	  states.push_back(spec);
	}	
      }

      // Find state with -highest number- of atom specification. This would be the fully
      // protonated state.
      atom_group_spec_ptr_t highest;
      for (auto state : states) {
	if ( !highest ) {
	  highest = state;
	} else if ( state->atomSpecifications().size() > highest->atomSpecifications().size() ) {
	  highest = state;
	}
      }

      // Add "missing" atom specification.
      for (auto state : states) {
	if ( state != highest) {
	  for ( auto aspec : highest->atomSpecifications() ) {
	    if ( !state->lookup(aspec->name()) ) {
	      // Add a zero charged specification.
	      std::clog << "Adding missing atom specification '" << aspec->name()
			<< "' to atom group specification '" << state->name() << "'." << std::endl;
	      atom_spec_ptr_t missing = AtomSpec::make(aspec->name(),
						       aspec->radius(),
						       0.0,
						       aspec->mass());
	      state->add(missing);
	    }
	  }
	}
      }
    }
  }

  GMXAtomGroupCatalog::GMXAtomGroupCatalog() :
    AtomGroupCatalog()
  {
  }

  atom_group_catalog_ptr_t GMXAtomGroupCatalog::valueOf(std::istream& stream,
							const atom_catalog_ptr_t& atomCatalog)
  {
    // Buffer to read line plus EOL.
    std::string buffer;
    
    // Located atoms in the topology.
    findAtoms_(stream);
    std::getline(stream, buffer);  // Line starting with ';   nr       type  resnr...'.
    
    // We have reach the first atom group (usually a residue) in the topolog.
    // Now find and create atom group specifications.
    std::vector<atom_group_spec_ptr_t> specs = findAtomGroups_(stream, atomCatalog);

    // Ensure all states per group have the same number of atom specifications.
    harmonize_(specs);

    // Create the catalog
    atom_group_catalog_ptr_t catalog = std::make_shared<GMXAtomGroupCatalog>();
    for (auto& spec : specs) {
      catalog->add(spec, spec->protonationState());
    }

    // Done.
    return catalog;
  }
}
