#ifndef PROTEIN_STRUCTURE_HPP
#define PROTEIN_STRUCTURE_HPP

#include "../types.hpp"
#include <string>

namespace simploce {

  /**
   * Static conformation of some protein. Consists of atoms, etc.
   */
  class ProteinStructure {
  public:

    /**
     * Constructor.
     * @param title - Descriptive title
     * @param atoms - Atoms
     */
    ProteinStructure(const std::string& title, const std::vector<Atom>& atoms);

    /**
     * Returns title.
     */
    std::string title() const;

    /**
     * Returns size.
     * @return Number of atoms.
     */
    std::size_t size() const;

    /**
     * Generates a dotted surface.
     */
    Surface dottedSurface() const;

  private:
    
    std::string title_;
    std::vector<Atom> atoms_;
  };

  
}

#endif
