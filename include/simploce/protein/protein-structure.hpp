#ifndef PROTEIN_STRUCTURE_HPP
#define PROTEIN_STRUCTURE_HPP

#include "../types.hpp"
#include <string>
#include <iostream>
#include <fstream>

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

    /**
     * Writes this protein structure to output stream.
     * @param stream - Output stream.
     * @return Output stream.
     */    
    std::ostream& writeTo(std::ostream& stream) const;

    /**
     * Takes (reads) a protein strcuture from input stream.
     * @param stream - Input stream.
     * @param catalog - Atom specifications catalog.
     * @return protein structure.
     */
    static prot_struct_ptr_t make(std::istream& stream, const atom_catalog_ptr_t& catalog);

  private:
    
    std::string title_;
    std::vector<Atom> atoms_;
  };

  /**
   * Writes protein structure to output stream.
   * @param stream - Output stream.
   * @param structure - Protein structure.
   * @return Output stream.
   */
  std::ostream& operator << (std::ostream& stream, const ProteinStructure& structure);  
  
}

#endif
