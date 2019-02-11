#include "simploce/protein/file-input-source.hpp"
#include "simploce/util/util.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace simploce {
  
  static void populateSource(std::vector<std::string> &source, std::istream &stream)
  {
    std::istream_iterator<std::string> begin(stream), end;
    std::copy(begin, end, std::back_inserter(source));
  }
  

  FileInputSource::FileInputSource(const std::string &fileName) :
    sourceId_(fileName), source_{}
  {
    std::ifstream stream;
    openInputFile(stream, fileName);
    populateSource(source_, stream);
    stream.close();
  }

  std::vector<std::string> FileInputSource::asStrings() const
  {
    return source_;
  }

  std::string  FileInputSource::getSourceId() const
  {
    return sourceId_;
  }
  
}
