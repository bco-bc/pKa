#include "simploce/protein/pdb-reader.hpp"
#include "simploce/protein/content-handler.hpp"
#include "simploce/protein/input-source.hpp"
#include "simploce/util/mu-units.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

using namespace simploce;

/**
 * Test whether given string represent a non-negative number.
 */
static bool isNonNegativeInteger(const std::string& s)
{
  try {
    int n = boost::lexical_cast<int>(s);
    return n >= 0 ? true : false;
  } catch(std::exception exception) {
    // Not a number.
    return false;
  }    
}

namespace simploce {
  
  /**
   * Helper class.
   */
  class PDBReaderHelper {
  public:
    
    /**
     * Set of standard protein amino acid names
     */
    static std::vector<std::string> AA_NAMES;
    
    /**
     * Set of record in PDB structure
     */
    static std::vector<std::string> RECORD_NAMES;
    
    /**
     * Permissible chain identifiers
     */
    static std::vector<char> CHAIN_IDS;
    
    /**
     * Maximum number of fields for the ATOM record
     */
    static int ATOM_FIELDS_MAX;
    
    /**
     * Constructor.
     */
    PDBReaderHelper(const content_handler_ptr_t& handler, const input_source_ptr_t& source);
    
    /**
     * Parse the source
     */
    void parse();
    
    /**
     * Identify a title from the PDB structure file.
     */
    std::string findTitle();
    
  private:
    
    /**
     * Checks whether a new molecule was encountered. A new molecule is identified only if a 
     * new MODEL or new Chain ID is encountered.
     * @param current Current value of the iterator over input source strings. Must refer to 
     * the RECORD name of the current line in the PDB input source.
     * @result Result.
    */
    bool newMolecule(std::vector<std::string>::const_iterator &current);

    /*
     * Handle a new molecule. This is called if newMolecule(...) returns true.
     * @param current Current value of the iterator over input source strings. 
     * Must refer to the RECORD name of the current line in the PDB input source.
     * @return New value of the iterator over input source strings pointing to current RECORD
     */
    std::vector<std::string>::const_iterator
    handleNewMolecule(std::vector<std::string>::const_iterator &current);

    /**
     * Check whether a new atom was encountered. A new atom is identified only is new ATOM and
     * an atom index is encountered.
     */
    bool newAtom(std::vector<std::string>::const_iterator &current);

    /**
     * Handles a new atom. Only called is newAtom() return true.
     */
    std::vector<std::string>::const_iterator
    handleAtom(std::vector<std::string>::const_iterator &iter);

    /**
     * Handles new residue. Only called if newAtom(...) return true.
     */
    void handleResidue(std::vector<std::string>::const_iterator &iter);

    int lineNumber_;
    content_handler_ptr_t handler_;
    std::string sourceId_;
    std::vector<std::string> items_;
    std::vector<std::string>::const_iterator last_;  // End of input source
    std::string chainId_;                            // Current chain ID
    
    static std::string record_names_[];
    static char chainIds_[];
    static std::string aa_names_[];
  };
  

  void PDBReader::contentHandler(const content_handler_ptr_t &handler)
  {
    handler_ = handler;
  }
  
  void PDBReader::parse(const input_source_ptr_t &source) const
  {
    std::string sourceId = source->sourceId();
    std::vector<std::string> items = source->asStrings();
    if ( items.empty() ) {
      throw std::domain_error(sourceId + ": Input source is empty");
    }
    if ( !handler_ ) {
      throw std::domain_error("No content handler available");
    }

    PDBReaderHelper helper(handler_, source);
    std::string title = helper.findTitle();
    //std::clog << "Title PDB content: " << title << std::endl;
    handler_->start(title);
    helper.parse();
    handler_->end();
  }

  chem_reader_ptr_t PDBReader::make()
  {
    return std::make_shared<PDBReader>();
  }


  PDBReaderHelper::PDBReaderHelper(const content_handler_ptr_t& handler,
				   const input_source_ptr_t& source) :
    lineNumber_{0}, handler_{handler}, sourceId_{source->sourceId()},
    items_{source->asStrings()}, last_{}
  {
    last_ = items_.end();
  }
  
  void PDBReaderHelper::parse()
  {
    bool atoms = false;
    std::string ATOM{"ATOM"};
    std::string MODEL{"MODEL"};
    std::string HETATM{"HETATM"};
    std::string names[3] = { ATOM, MODEL, HETATM };
    std::vector<std::string> targets(names, names + 2);
    
    std::vector<std::string>::const_iterator iter = items_.begin();
    do {
      // New record?
      if (std::find(RECORD_NAMES.begin(),RECORD_NAMES.end(), *iter)  != RECORD_NAMES.end()) {
	++lineNumber_;
      }
      if (std::find(targets.begin(), targets.end(), *iter) != targets.end()) {
	bool newMol = this->newMolecule(iter);
	if ( newMol )
	  iter = this->handleNewMolecule(iter);
	if (*iter == ATOM || *iter == HETATM) {
	  bool newAtom = this->newAtom(iter);
	  if ( newAtom ) {
	    atoms = true;
	    this->handleResidue(iter);
	    iter = this->handleAtom(iter);
	  }
	}
      }
      ++iter;
    } while (iter < items_.end());
    if (!atoms) {
      std::string lineNumber = boost::lexical_cast<std::string>(lineNumber_);
      std::string message =
	"SourceId: " + sourceId_ + ", line number: " + lineNumber + ": No " + ATOM +
	"records found.";
      throw std::domain_error(message);
    }
    handler_->endAtomGroup();
    handler_->endMolecule();
  }

  bool PDBReaderHelper :: newMolecule(std::vector<std::string>::const_iterator &current)
  {
    static char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    static std::vector<char> DIGITS(digits, digits + 10);
    static std::string previousChainId;
    static std::string MODEL = "MODEL";
    static bool firstTime = true;
    
    // If the end of the input source was reached, return immediately.
    if (current + 1 >= last_ || current + 5 >= last_) {
      firstTime = false;
      return false;
    }
    
    // Check if there is new MODEL.
    if (*current == MODEL) {
      firstTime = false;
      return true;
    }

    // Find a new chain identifier, if any. A chainId is any string of length 1 and -not-
    // represeting a integer number as specified in digits.
    bool foundChainId = false;
    std::string chainId = *(current + 5);
    if ( chainId.size() > 1) {
      firstTime = false;
      return false;
    }
   
    if ( !isNonNegativeInteger(chainId) ) {
      foundChainId = true;
    } else {
      chainId = *(current + 4);
      if (chainId.size() == 1) {  // Chain id must be one character long.
	char ch = chainId[0];
	if (std::find(DIGITS.begin(), DIGITS.end(), ch) == DIGITS.end()) {
	  // Chain Id does not refer to a digit.
	  if (std::find(PDBReaderHelper::CHAIN_IDS.begin(), PDBReaderHelper::CHAIN_IDS.end(), 
			chainId[0]) == PDBReaderHelper::CHAIN_IDS.end()) {
	    foundChainId = true;
	  }
	}
      }
    }
    
    if ( foundChainId ) {
      if ( !previousChainId.empty() ) {
	// Already assigned before.
	if (previousChainId != chainId) {
	  // Update chainIds
	  previousChainId = chainId;
	  chainId_ = chainId;
	  firstTime = false;
	  return true;
	}
      }
      previousChainId = chainId;
      chainId_ = chainId;
    }
    
    if ( !firstTime ) {
      firstTime = false;
      return false;
    } else {
      firstTime = false;
      return true;
    }
  }
  
  std::vector<std::string>::const_iterator
  PDBReaderHelper :: handleNewMolecule(std::vector<std::string>::const_iterator &current)
  {
    static std::string MODEL = "MODEL";
    static bool firstTime = true;
    std::vector<std::string>::const_iterator iter;
    
    if (current + 1 >= last_) {
      return last_;
    }
    
    if (*current == MODEL) {
      try {
	if ( !firstTime )
	  handler_->endMolecule();
	std::string moleculeName = *current + *(current + 1);
	int serial = boost::lexical_cast<int>(*(current + 1));  // May refer to MODEL index number.
	if (serial < 0) {
	  moleculeName = *current;
	  handler_->startMolecule(moleculeName);
	  iter = current;
	} else {
	  handler_->startMolecule(moleculeName);
	  iter = current + 1;
	}
      } catch (boost::bad_lexical_cast exception) {
	iter = current;
      }
    } else {
      std::string moleculeName = chainId_;
      if ( !firstTime )
	handler_->endMolecule();
      handler_->startMolecule(moleculeName);
      iter = current;
    }

    firstTime = false;
    return iter;
  }

  void PDBReaderHelper::handleResidue(std::vector<std::string>::const_iterator &current)
  {
    static std::string prevResidueName{};
    static int prevResidueIndex{-1};

    int residueIndex{0};
    std::string residueName = *(current + 4);
    std::vector<std::string>::const_iterator iter = current + 4;
      
    // Test whether residueName is a positive integer.
    //std::clog << "Verifying '" << residueName << "' is truly a residueName." << std::endl;
    if ( isNonNegativeInteger(residueName) ) {
      //std::clog << "It is a positive integer number. Go three forward." << std::endl;
      iter = current + 3;
    } else {
      if ( (*iter).size() < 3) {
	//std::clog << "It looks like a residue name, but it is too short." << std::endl;
	iter -= 1;
      }
    }
    residueName = *iter;

    std::string s = *(iter + 1);
    if ( isNonNegativeInteger(s) ) {
      residueIndex = boost::lexical_cast<int>(s);
    } else {
      std::string s = *(iter + 2);
      if ( isNonNegativeInteger(s) ) {
	residueIndex = boost::lexical_cast<int>(s);
      }
    }

    if (prevResidueName.length() > 0) {
      if (residueName != prevResidueName || residueIndex != prevResidueIndex) {
	prevResidueName = residueName;
	prevResidueIndex = residueIndex;
	handler_->endAtomGroup();
	handler_->startAtomGroup(residueName);
	handler_->index(residueIndex);
      }
    } else {
      prevResidueName = residueName;
      prevResidueIndex = residueIndex;
      handler_->startAtomGroup(residueName);
      handler_->index(residueIndex);
    }
  }

  
  bool PDBReaderHelper::newAtom(std::vector<std::string>::const_iterator &current)
  {
    static std::string ATOM{"ATOM"};
    static std::string HETATM{"HETATM"};

    if ( *current != ATOM && *current != HETATM ) {
      return false;
    }

    std::string s = *(current + 1);
    if ( !isNonNegativeInteger(s) ) {
      return false;
    }

    return true;
  }


  std::vector<std::string>::const_iterator
  PDBReaderHelper::handleAtom(std::vector<std::string>::const_iterator &current)
  {
    std::string ATOM = "ATOM";
    std::string MODEL = "MODEL";
    std::string HETATM = "HETATM";
    std::string names[3] = { ATOM, MODEL, HETATM };
    std::vector<std::string> targets(names, names + 2);
    
    if (current + 10 >= last_) {
      return last_;
    }

    int atomIndex{0};
    std::string s = *(current + 1);
    if ( isNonNegativeInteger(s) ) {
      atomIndex = boost::lexical_cast<int>(*(current + 1));
    }
    std::string atomName = *(current + 2);
    handler_->startAtom(atomName);
    handler_->index(atomIndex);
    
    // Get the coordinates
    int index = 4; // Assume chain identifier, code for insertion of residues and alternate
                   // location indicator are not there.
    real_t x{0}, y{0}, z{0}, fraction{0}, eps = 1.0e-07;
    bool success = false;
    do {
      try {
	std::string s = *(current + index);	  
	int length = s.length();
	x = boost::lexical_cast<real_t>(s);
	fraction = std::modf(x, &fraction);
	if (std::fabs(fraction) > eps) {
	    success = true;
	} else {
	  if (length >= 5) {
	    std::string t = s.substr(length - 3, 4);
	    if (t == "000")
	      success = true;
	  }
	}
      } catch (boost::bad_lexical_cast) {
	// Deliberately ignore!
      }
      ++index;
    } while (!success && index < 8);
    if (!success) {
      std::string cindex = *(current + index);
      std::string lineNumber = boost::lexical_cast<std::string>(lineNumber_);
      std::string message =
	"SourceId: " + sourceId_ + ", line number: " + lineNumber + ": " +
	cindex + "': Cannot find atom coordinates in ATOM record";
      throw std::domain_error(message);
    }
    --index;
    try {
      y = boost::lexical_cast<real_t>(*(current + index + 1));
      z = boost::lexical_cast<real_t>(*(current + index + 2));
    } catch (boost::bad_lexical_cast &exception) {
      std::string cindex = *(current + index);
      std::string lineNumber = boost::lexical_cast<std::string>(lineNumber_);
      std::string message =
	"SourceId: " + sourceId_ + ", line number: " + lineNumber + ": " +
	cindex + "': Cannot find atom coordinates in ATOM record";
      throw std::domain_error(message);
      
    }
    x *= MUUnits<real_t>::Angstrom_to_nm;
    y *= MUUnits<real_t>::Angstrom_to_nm;
    z *= MUUnits<real_t>::Angstrom_to_nm;
    position_t r{x, y, z};
    handler_->atomPosition(r);
    charge_t charge(0.0);
    handler_->atomCharge(charge);    // PDB has no information about charges.
    
    handler_->endAtom();

    // To next record.
    std::vector<std::string>::const_iterator iter = current;
    std::vector<std::string>::const_iterator it = std::find(targets.begin(), targets.end(), *iter);
    while ( it == targets.end() && iter != last_) {
      ++iter;
      it = std::find(targets.begin(), targets.end(), *iter);
    }

    return iter;
  }


  std::string PDBReaderHelper::findTitle()
  {
    // Find the TITLE record.
    std::string TITLE = "TITLE";
    std::vector<std::string>::const_iterator iter = std::find(items_.begin(), items_.end(), TITLE);
    if (iter == items_.end()) {
      // No TITLE.
      // Find the HEADER record
      std::string HEADER = "HEADER";
      iter = std::find(items_.begin(), items_.end(), HEADER);
      if (iter == items_.end()) {
	// Did not find the HEADER entry. Try COMPND record.
	std::string COMPND = "COMPND";
	iter = std::find(items_.begin(), items_.end(), COMPND);
	if (iter == items_.end()) {
	  std::string message =
	    "SourceId: " + sourceId_ + ": Cannot identify a title from the PDB structure";
	  throw std::domain_error(message);
	}
      }
    }
    ++iter;
    if (*iter == "ATOM" || *iter == "HETATM")
      return "No Title";
    
    // Construct title. Use the full record.
    std::string space = " ";
    std::string title;
    bool firstTime = true;
    do {
      if (firstTime == true)
	firstTime = false;
      else
	title += space;
      title += *iter;
      ++iter;
    } while (std::find(PDBReaderHelper::RECORD_NAMES.begin(),
		       PDBReaderHelper::RECORD_NAMES.end(), *iter) ==
	                 PDBReaderHelper::RECORD_NAMES.end() && iter < items_.end());
    
    return title;
  }


  std::string PDBReaderHelper::record_names_[48] = { 
    "HEADER", "TITLE", "OBSLTE", "CAVEAT", "COMPND", "SOURCE", "KEYWDS", "EXPDTA", "AUTHOR",
    "REVDAT", "SPRSDE", "JRNL", "REMARK",
    "DBREF", "SEQADV", "SEQRES", "MODRES",
    "HET", "HETNAM", "HETSYN", "FORMUL", 
    "HELIX", "SHEET", "TURN",
    "SSBOND", "LINK", "HYDBND", "SLTBRG", "CISPEP",
    "SITE",
    "CRYST1", "ORIGXn", "ORIGX1", "ORIGX2", "ORIGX3", "SCALEn", "MTRIXn", "TVECT",
    "MODEL", "ATOM", "SIGATM", "ANISOU", "SIGUIJ", "TER", "HETATM",
    "CONECT",
    "MASTER", "END"
  };
  
  std::vector<std::string>
  PDBReaderHelper::RECORD_NAMES(PDBReaderHelper::record_names_,
				PDBReaderHelper::record_names_ + 47);

  char PDBReaderHelper::chainIds_[6] = {
    'A', 'B', 'C', 'D', 'E', 'F'
  };

  std::vector<char>
  PDBReaderHelper::CHAIN_IDS(PDBReaderHelper::chainIds_, PDBReaderHelper::chainIds_ + 5);

  std::string PDBReaderHelper::aa_names_[23] = {
    "ALA", "ARG", "ASN", "ASP", "ASX", "CYS", "GLN", "GLU", "GLX", "GLY", "HIS", "ILE",
    "LEU", "LYS", "MET", "PHE", "PRO", "SER", "THR", "TRP", "TYR", "UNK", "VAL"
  };

  std::vector<std::string> PDBReaderHelper::AA_NAMES(PDBReaderHelper::aa_names_,
						     PDBReaderHelper::aa_names_ + 22);

  int PDBReaderHelper :: ATOM_FIELDS_MAX = 16;
}

