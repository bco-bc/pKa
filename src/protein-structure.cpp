#include "simploce/protein/protein-structure.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/surface/dotted-surface-generator.hpp"
#include "simploce/surface/surface.hpp"

namespace simploce {

  ProteinStructure::ProteinStructure(const std::string& title) :
    title_(title), atoms_{}
  {
  }

  ProteinStructure::ProteinStructure(const std::string& title, const std::vector<Atom>& atoms) :
    title_(title), atoms_{atoms}
  {
  }

  void  ProteinStructure::atoms(const std::vector<Atom>& atoms)
  {
    atoms_ = std::vector<Atom>{atoms};
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
