#ifndef PROTEIN_STRUCTURE_CONTENT_HANDLER_HPP
#define PROTEIN_STRUCTURE_CONTENT_HANDLER_HPP

#include "../types.hpp"
#include "logger-content-handler.hpp"

namespace simploce {

  /**
   * Generates a protein structures from chemical content.
   */
  class ProteinStructureContentHandler : public LoggerContentHandler {
  public:

    ProteinStructureContentHandler(const atom_catalog_ptr_t& atomCatalog);

    void start(const std::string &title) override;

    void startAtom(const std::string &atomName) override;
    
    void atomPosition(const position_t& r) override;
    
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
    
  };
}

#endif
