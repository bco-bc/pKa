#ifndef FILE_INPUT_SOURCE_HPP
#define FILE_INPUT_SOURCE_HPP

#include "input-source.hpp"

#include <string>
#include <vector>
#include <iostream>

namespace simploce {

  /**
   * Input source is a file.
   */
  class FileInputSource : public InputSource {
  public:

    /**
     * Constructor.
     * @param fileName - Name of an existing file on the local system. 
     */
    FileInputSource(const std::string &fileName);
    
    std::vector<std::string> asStrings() const override;
    
    /**
       @return Input file name.
    */
    std::string getSourceId() const override;
    
  private:
    
    std::string sourceId_;
    std::vector<std::string> source_;
  };
  
}

#endif

