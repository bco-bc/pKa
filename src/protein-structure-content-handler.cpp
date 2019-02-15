#include "simploce/protein/protein-structure-content-handler.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/protein-structure.hpp"
#include <stdexcept>

namespace simploce {

  static std::string title_{};
  static std::vector<Atom> atoms_{};  
  static std::string atomName_{};
  static position_t r_{};

  ProteinStructureContentHandler::
  ProteinStructureContentHandler(const atom_catalog_ptr_t& atomCatalog) :
    LoggerContentHandler{}, atomCatalog_{atomCatalog} 
  {
    if ( !atomCatalog_ ) {
      throw std::domain_error("ProteinStructureContentHandler: atom catalog must be provided.");
    }
  }

  void ProteinStructureContentHandler::start(const std::string &title)
  {
    atoms_.clear();
    simploce::title_ = title;
  }

  void ProteinStructureContentHandler::startAtom(const std::string &atomName)
  {
    atomName_ = atomName;    
  }

  void ProteinStructureContentHandler::atomPosition(const position_t& r)
  {
    r_ = r;
  }

  void ProteinStructureContentHandler::endAtom()
  {
    atom_spec_ptr_t spec = atomCatalog_->lookup(atomName_);
    Atom atom{atomName_, r_, spec};
    atoms_.push_back(atom);
  }

  ProteinStructure ProteinStructureContentHandler::proteinStructure() const
  {
    return ProteinStructure{simploce::title_, atoms_};
  }

  std::shared_ptr<ProteinStructureContentHandler>
  ProteinStructureContentHandler::make(const atom_catalog_ptr_t& atomCatalog)
  {
    return std::make_shared<ProteinStructureContentHandler>(atomCatalog);
  }
}

