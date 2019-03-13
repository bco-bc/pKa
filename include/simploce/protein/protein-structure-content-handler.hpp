#ifndef PROTEIN_STRUCTURE_CONTENT_HANDLER_HPP
#define PROTEIN_STRUCTURE_CONTENT_HANDLER_HPP

#include "../types.hpp"
#include "base-content-handler.hpp"
#include "atom.hpp"
#include "atom-group.hpp"

namespace simploce {

  /**
   * Generates a protein structures from chemical content.
   */
  class ProteinStructureContentHandler : public BaseContentHandler {
  public:

    ProteinStructureContentHandler(const atom_catalog_ptr_t& atomCatalog);

    void start(const std::string &title) override;

    void startAtomGroup(const std::string &atomGroupName) override;

    void endAtomGroup() override;

    void startAtom(const std::string &atomName) override;
    
    void atomPosition(const position_t& r) override;

    void index(int index) override;
    
    void endAtom() override;

    /**
     * Returns protein structure.
     */
    ProteinStructure proteinStructure() const;

    /**
     * Returns content handler.
     * @param atomCatalog - Atom specifications catalog.
     */
    static std::shared_ptr<ProteinStructureContentHandler>
    make(const atom_catalog_ptr_t& atomCatalog);
    
  private:

    atom_catalog_ptr_t atomCatalog_;

    std::string title_;
    std::vector<AtomGroup> atomGroups_;
    std::string atomGroupName_;
    int atomGroupId_;
    AtomGroup atomGroup_;
    std::vector<atom_ptr_t> atoms_;  
    std::string atomName_;
    position_t r_;
    
  };
}

#endif
