#ifndef GMX_READER_HPP
#define GMX_READER_HPP

#include "chem-reader.hpp"

namespace simploce {

  /**
   * Parsers Gromacs coordinates and optionally also the associated topology, the latter for
   * extracting partial charges.
   */
  class GMXReader : public ChemReader {
  public:

    GMXReader();

    void contentHandler(const content_handler_ptr_t &handler) override;

    /**
     * Parses the Gromacs coordinate file.
     */
    void parse(const input_source_ptr_t &source) const override;

    /**
     * Returns content reader for parsing Gromacs coordinates.
     */
    static chem_reader_ptr_t make();

  private:
    
    content_handler_ptr_t handler_;
  };
  
}

#endif
