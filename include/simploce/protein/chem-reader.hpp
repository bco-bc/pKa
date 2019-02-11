#ifndef CHEM_READER_HPP
#define CHEM_READER_HPP

#include "../types.hpp"
#include <memory>

namespace simploce {

  /**
   * Parses chemical content in an input source. An implementation assumes a particular 
   * format of the chemical content.
   */
  struct ChemReader {
    
    virtual ~ChemReader() {}
    
    /**
     * Sets the content handler. The content handler receives notification of particular 
     * content in the input source.
     * @param handler - Content handler
     */
    virtual void contentHandler(const content_handler_ptr_t& handler) = 0;
    
    /**
     * Parse the input source. Parsing can only begin after a content handler has been provided.
     * @param source - Input source.
     */
    virtual void parse(const input_source_ptr_t& source) const = 0;
    
  };
  
}

#endif
