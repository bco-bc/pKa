#ifndef INPUT_SOURCE_HPP
#define INPUT_SOURCE_HPP

#include <string>
#include <vector>

namespace simploce {

  /**
   * A single molecular input source for molecular content.
   */
  struct InputSource {
    
    virtual ~InputSource() {}
    
    /**
     * Returns input as a collection of string items. Each word in the original content 
     * must be placed in a separate item.
     * @return String items
     */
    virtual std::vector<std::string> asStrings() const = 0;
    
    /**
     * Get a source identifier, if available.
     */
    virtual std::string getSourceId() const = 0;
  };
  
}

#endif

