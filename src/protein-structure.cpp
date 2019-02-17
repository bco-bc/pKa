#include "simploce/protein/protein-structure.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/surface/dotted-surface-generator.hpp"
#include "simploce/surface/surface.hpp"

namespace simploce {

  /**
   * Moves origin inside structure.
   */
  static std::vector<Atom> translateOrigin_(const std::vector<Atom>& atoms)
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
    title_(title), atoms_{translateOrigin_(atoms)}
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

}
