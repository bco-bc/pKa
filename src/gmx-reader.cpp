#include "simploce/protein/gmx-reader.hpp"
#include "simploce/protein/content-handler.hpp"
#include "simploce/protein/input-source.hpp"
#include "simploce/util/mu-units.hpp"
#include "simploce/util/util.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <algorithm>

namespace simploce {

  static std::pair<std::size_t, std::string> extractResidueIndexAndName(const std::string& s)
  {
    static std::vector<char> numbers = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    
    std::size_t counter = 0;
    char ch = s[counter];
    std::string number{};
    
    bool isNumber = std::find(numbers.begin(), numbers.end(), ch) != numbers.end();
    while (isNumber) {
      number.push_back(ch);
      counter += 1;
      ch = s[counter];
      isNumber = std::find(numbers.begin(), numbers.end(), ch) != numbers.end();
    }
    std::size_t index = boost::lexical_cast<std::size_t, std::string>(number);
    std::string name = s.substr(counter);
    return std::make_pair(index, name);
  }

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
    handler_->startMolecule("Protein");
    
    std::size_t natoms = boost::lexical_cast<std::size_t, std::string>(*itemsIter_++);
    std::clog << "GMXReader: Number of atoms: " << natoms << std::endl;    
    lineNumber_ = 2;
    std::size_t residueIndex{0};
    for (std::size_t i = 0; i != natoms; ++i) {

      // Each line holds 6 words.
      std::string residue = *itemsIter_;  // Both index and name.
      auto pair = extractResidueIndexAndName(residue);
      std::string residueName = pair.second;
      if ( residueIndex == 0) {
	// First residue encountered.
	residueIndex = pair.first;
      } else {
	// Same or new residue?
	if ( residueIndex != pair.first) {
	  // New residue encountered.
	  residueIndex = pair.first;
	  handler_->startAtomGroup(residueName);
	  handler_->index(residueIndex);
	}
      }
      std::string atomName = *(itemsIter_ + 1);
      std::size_t atomIndex = boost::lexical_cast<std::size_t, std::string>(*(itemsIter_ + 2));
      real_t x = boost::lexical_cast<real_t,  std::string>(*(itemsIter_ + 3));
      real_t y = boost::lexical_cast<real_t,  std::string>(*(itemsIter_ + 4));
      real_t z = boost::lexical_cast<real_t,  std::string>(*(itemsIter_ + 5));
      handler_->startAtom(atomName);
      handler_->index(atomIndex);
      handler_->atomPosition(position_t{x, y, z});
      handler_->endAtom();

      itemsIter_ += 6;
    }
    // The last line contains box sizes. Not needed.
    
    handler_->endMolecule();
    handler_->end();
  }

  std::string GMXReaderHelper::findTitle_()
  {
    bool firstTime{true};
    std::string title{};
    std::string s = *itemsIter_;
    while ( !util::isNonNegativeInteger(s) ) {
      if ( !firstTime) {
	// Add space first.
	title += " ";
      }
      title += s;
      s = *(++itemsIter_);
      if ( firstTime ) {
	firstTime = false;
      }
    }

    // itemsIter_ should now be at line 2.
    return title;
  }

}
