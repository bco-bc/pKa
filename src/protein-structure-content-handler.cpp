#include "simploce/protein/protein-structure-content-handler.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/protein/atom-group.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/protein-structure.hpp"
#include <stdexcept>

namespace simploce {

  static std::string title_{};
  static std::vector<AtomGroup> atomGroups_{};
  static AtomGroup atomGroup_{};
  static std::vector<Atom> atoms_{};  
  static std::string atomName_{};
  static position_t r_{};

  ProteinStructureContentHandler::
  ProteinStructureContentHandler(const atom_catalog_ptr_t& atomCatalog) :
    BaseContentHandler{}, atomCatalog_{atomCatalog} 
  {
    if ( !atomCatalog_ ) {
      throw std::domain_error("ProteinStructureContentHandler: atom catalog must be provided.");
    }
  }

  void ProteinStructureContentHandler::start(const std::string &title)
  {
    atomGroups_.clear();
    atoms_.clear();
    simploce::title_ = title;
  }

  void ProteinStructureContentHandler::startAtomGroup(const std::string &atomGroupName)
  {
    atomGroup_ = AtomGroup{atomGroupName};   
  }

  void ProteinStructureContentHandler::endAtomGroup()
  {
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

