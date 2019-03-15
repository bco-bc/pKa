#include "simploce/protein/protein-structure.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/atom-group.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/surface/dotted-surface-generator.hpp"
#include "simploce/surface/surface.hpp"
#include "simploce/conf.hpp"

namespace simploce {

  /**
   * Moves origin inside structure.
   */
  static std::vector<atom_ptr_t> moveOrigin_(const std::vector<atom_ptr_t>& atoms)
  {
    // Determine origin.
    position_t origin;
    for (const auto& atom : atoms) {
      origin += atom->position();
    }
    origin /= atoms.size();

    // Move atoms.
    std::vector<atom_ptr_t> moved;
    for (const auto& atom : atoms) {
      position_t r = atom->position() - origin;
      moved.push_back(Atom::make(atom->id(), atom->name(), r, atom->spec()));
    }

    return std::vector<atom_ptr_t>{moved};
  }

  ProteinStructure::ProteinStructure() :
    title_{}, atoms_{}, atomGroups_{}
  {
  }

  ProteinStructure::ProteinStructure(const std::string& title,
				     const std::vector<atom_ptr_t>& atoms,
				     const std::vector<AtomGroup>& atomGroups) :
    title_(title), atoms_{moveOrigin_(atoms)}, atomGroups_{atomGroups}
  {
  }

  std::string ProteinStructure::title() const
  {
    return title_;
  }

  std::size_t ProteinStructure::numberOfAtoms() const
  {
    return atoms_.size();
  }

  std::size_t ProteinStructure::numberOfAtomGroups() const
  {
    return atomGroups_.size();
  }

  Surface ProteinStructure::dottedSurface() const
  {
    return generateDottedSurface(atoms_);
  }

  const std::vector<atom_ptr_t>& ProteinStructure::atoms() const
  {
    return atoms_;
  }

  const std::vector<AtomGroup>& ProteinStructure::atomGroups() const
  {
    return atomGroups_;
  }

  std::ostream& ProteinStructure::writeTo(std::ostream& stream) const
  {
    stream << title_ << std::endl;
    stream << atoms_.size() << std::endl;
    for (auto atom : atoms_) {
      stream << *atom << std::endl;
    }
    stream << atomGroups_.size() << std::endl;
    for (std::size_t i = 0; i != atomGroups_.size(); ++i) {
      const AtomGroup& atomGroup = atomGroups_[i];
      stream << atomGroup;
      if ( i != atomGroups_.size() - 1 ) {
	stream << std::endl;
      }
    }
    
    return stream;
  }

  prot_struct_ptr_t ProteinStructure::make(std::istream& stream, const atom_catalog_ptr_t& catalog)
  {
    // Buffer to read EOL.
    std::string buffer;

    std::vector<atom_ptr_t> atoms{};

    std::string title;
    stream >> title;
    std::getline(stream, buffer);        // Reads EOL.
    
    std::size_t natoms;
    stream >> natoms;
    std::getline(stream, buffer);        // Reads EOL.

    // Atoms
    std::size_t id;
    real_t x, y, z;
    std::string name;
    for (std::size_t i = 0; i != natoms; ++i) {
      stream >> id;
      char ch = stream.get();
      while (ch == ' ') {
	ch = stream.get();
      }
      while ( ch != ' ') {
	name.push_back(ch);
	ch = stream.get();
      }
      stream >> x >> y >> z;
      
      atom_spec_ptr_t spec = catalog->lookup(name);
      atom_ptr_t atom = Atom::make(Atom::id_t{id}, name, position_t{x, y, z}, spec);
      atoms.push_back(atom);
      std::getline(stream, buffer);
      name.clear();
    }

    // Atom groups.
    std::vector<AtomGroup> atomGroups;
    std::size_t nAtomGroups, atomId;
    stream >> nAtomGroups;
    std::getline(stream, buffer);
    std::vector<atom_ptr_t> atms{};
    bool protonatable{false};
    std::size_t occupancy{0};
    for (std::size_t i = 0; i != nAtomGroups; ++i) {
      name.clear();
      atms.clear();
      stream >> id;
      char ch = stream.get();
      while (ch == ' ') {
	ch = stream.get();
      }
      while ( ch != ' ') {
	name.push_back(ch);
	ch = stream.get();
      }
      stream >> protonatable >> occupancy;
      stream >> natoms;
      for (std::size_t j = 0; j != natoms; ++j) {
	stream >> atomId;
	atms.push_back(atoms[atomId - 1]);  // atomId starts at 1.
      }
      AtomGroup atomGroup(AtomGroup::id_t{id}, name);
      atomGroup.add(atms);
      atomGroup.protonatable(protonatable);
      atomGroup.protonationState_(occupancy);
      atomGroups.push_back(atomGroup);
      protonatable = false;
      occupancy = 0;
    }
    return std::make_shared<ProteinStructure>(title, atoms, atomGroups);
  }

  prot_struct_ptr_t ProteinStructure::make(const std::string& title,
					   const std::vector<atom_ptr_t>& atoms,
					   const std::vector<AtomGroup>& atomGroups)
  {
    return std::make_shared<ProteinStructure>(title, atoms, atomGroups);
  }
  
  std::ostream& operator << (std::ostream& stream, const ProteinStructure& structure)
  {
    return structure.writeTo(stream);
  }

}
