#include "simploce/protein/gmx-reader.hpp"
#include "simploce/protein/content-handler.hpp"
#include "simploce/protein/input-source.hpp"
#include "simploce/util/mu-units.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <string>

namespace simploce {

  class GMXReaderHelper {
  public:

    /**
     * Constructor
     */
    GMXReaderHelper(const content_handler_ptr_t& handler, const input_source_ptr_t& source);

    /**
     * Set topologies
     */
    void setTopologies(const std::vector<input_source_ptr_t>& topologies);

    /**
     * Parse the input source.
     */
    void parse();

  private:

    std:: string findTitle_();

    content_handler_ptr_t handler_;
    input_source_ptr_t source_;
    
    // Items from coordinatres input source.
    std::vector<std::string> items_;

    // Items iterator
    std::vector<std::string>::iterator itemsIter_;

    // Line number in input source.
    std::size_t lineNumber_;

  };    

  GMXReader::GMXReader() : ChemReader()
  {
  }

  void GMXReader::contentHandler(const content_handler_ptr_t &handler)
  {
    handler_ = handler;
  }

  void GMXReader::parse(const input_source_ptr_t &source) const
  {
    std::string sourceId = source->sourceId();
    std::vector<std::string> items = source->asStrings();
    if ( items.empty() ) {
      throw std::domain_error(sourceId + ": Input source is empty");
    }
    if ( !handler_ ) {
      throw std::domain_error("No content handler available");
    }

    GMXReaderHelper helper(handler_, source);
    helper.parse();
  }

  chem_reader_ptr_t GMXReader::make()
  {
    return std::make_shared<GMXReader>();
  }

  GMXReaderHelper::GMXReaderHelper(const content_handler_ptr_t& handler,
				   const input_source_ptr_t& source) :
    handler_{handler}, source_{source}, items_{source->asStrings()}, itemsIter_{items_.begin()},
    lineNumber_{0}
  {
  }

  void GMXReaderHelper::parse()
  {
    std::string title = this->findTitle_();
    handler_->start(title);
    lineNumber_ = 2;
    std::size_t natoms = boost::lexical_cast<std::size_t, std::string>(*itemsIter_++);
    std::clog << "Number of atoms: " << natoms << std::endl;    
  }

  std::string GMXReaderHelper::findTitle_()
  {
    return "";
  }

}
