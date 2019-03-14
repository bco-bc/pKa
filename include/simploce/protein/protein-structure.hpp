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
     * Constructor. Empty protein.
     */
    ProteinStructure();

    /**
     * Constructor. This class will take full ownership of atoms.
     * @param title - Descriptive title
     * @param atoms - Atoms
     * @param atomGroups - Atom groups (e.g. residues).
     */
    ProteinStructure(const std::string& title,
		     const std::vector<atom_ptr_t>& atoms,
		     const std::vector<AtomGroup>& atomGroups);

    /**
     * Returns title.
     */
    std::string title() const;

    /**
     * Returns number of atoms.
     * @return Number of atoms.
     */
    std::size_t numberOfAtoms() const;

    /**
     * Returns number of atom groups.
     * @return Number of atom groups.
     */
    std::size_t numberOfAtomGroups() const;

    /**
     * Generates a dotted surface.
     */
    Surface dottedSurface() const;

    /**
     * Return all atoms.
     */
    const std::vector<atom_ptr_t >& atoms() const;

    /**
     * Returns all atom groups.
     */
    const std::vector<AtomGroup>& atomGroups() const;

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

    /**
     * Creates a protein structure.
     * @param title - Descriptive title
     * @param atoms - Atoms
     * @param atomGroups - Atom groups (e.g. residues).
     */
    static prot_struct_ptr_t make(const std::string& title,
				  const std::vector<atom_ptr_t>& atoms,
				  const std::vector<AtomGroup>& atomGroups);

  private:
    
    std::string title_;
    std::vector<atom_ptr_t> atoms_;
    std::vector<AtomGroup> atomGroups_;
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
