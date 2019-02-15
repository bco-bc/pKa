#ifndef PDB_READER_HPP
#define PDB_READER_HPP

#include "chem-reader.hpp"

namespace simploce {

    /**
     * Parses PDB (protein) content. Residues are reported to the content handler as atom groups.
     */
  class PDBReader : public ChemReader {

  public:

    PDBReader() : ChemReader() {}

    void contentHandler(const content_handler_ptr_t &handler) override;

    /**
     * Parses PDB content. The PDB structure should at least contain a TITLE and/or 
     * a HEADER record and also one or more ATOM records.
     * @param source - PDB structure.
     */
    void parse(const input_source_ptr_t &source) const override;

    /**
     * Returns a PDB chemical content reader.
     */
    static chem_reader_ptr_t make();
    
  private:
    
    content_handler_ptr_t handler_;
  };
  
}

#endif

