#include "simploce/protein/protein-structure.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/surface/dotted-surface-generator.hpp"
#include "simploce/surface/surface.hpp"
#include "simploce/conf.hpp"

namespace simploce {

  /**
   * Moves origin inside structure.
   */
  static std::vector<Atom> moveOrigin_(const std::vector<Atom>& atoms)
  {
    // Determine origin.
    position_t origin;
    for (const Atom& atom : atoms) {
      origin += atom.position();
    }
    origin /= atoms.size();

    // Translated.
    std::vector<Atom> moved;
    for (const Atom& atom : atoms) {
      position_t r = atom.position() - origin;
      moved.push_back(Atom{atom.id(), atom.name(), r, atom.spec()});
    }

    return std::vector<Atom>{moved};
  }

  ProteinStructure::ProteinStructure(const std::string& title, const std::vector<Atom>& atoms) :
    title_(title), atoms_{moveOrigin_(atoms)}
  {
  }

  std::string ProteinStructure::title() const
  {
    return title_;
  }

  std::size_t ProteinStructure::size() const
  {
    return atoms_.size();
  }

  Surface ProteinStructure::dottedSurface() const
  {
    return generateDottedSurface(atoms_);
  }

  std::ostream& ProteinStructure::writeTo(std::ostream& stream) const
  {
    stream << title_ << std::endl;
    stream << atoms_.size() << std::endl;
    for (auto iter = atoms_.begin(); iter != atoms_.end() - 1; ++iter) {
      const Atom& atom = *iter;
      stream << atom << std::endl;
    }
    const Atom& atom = *(atoms_.end() - 1);
    stream << atom;
    return stream;
  }

  prot_struct_ptr_t ProteinStructure::make(std::istream& stream, const atom_catalog_ptr_t& catalog)
  {
    // Buffer to read EOL.
    std::string buffer;

    std::vector<Atom> atoms{};

    std::string title;
    stream >> title;
    std::getline(stream, buffer);        // Reads EOL.
    
    std::size_t natoms;
    stream >> natoms;
    std::getline(stream, buffer);        // Reads EOL.
 
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
      Atom atom{Atom::id_t{id}, name, position_t{x, y, z}, spec};
      atoms.push_back(atom);
      std::getline(stream, buffer);
      name.clear();
    }

    return std::make_shared<ProteinStructure>(title, atoms);
  }

  std::ostream& operator << (std::ostream& stream, const ProteinStructure& structure)
  {
    return structure.writeTo(stream);
  }

}
