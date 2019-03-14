#include "simploce/protein/protein-structure-content-handler.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/protein/atom-group.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/protein-structure.hpp"
#include <stdexcept>

namespace simploce {

  ProteinStructureContentHandler::
  ProteinStructureContentHandler(const atom_catalog_ptr_t& atomCatalog) :
    BaseContentHandler{}, atomCatalog_{atomCatalog}, title_{}, atomGroups_{},
    atomGroupName_{""}, atomGroupId_{-1}, atomGroup_{}, atoms_{}, atomName_{}, r_{}
  {
    if ( !atomCatalog_ ) {
      throw std::domain_error("ProteinStructureContentHandler: atom catalog must be provided.");
    }
  }

  void ProteinStructureContentHandler::start(const std::string &title)
  {
    atomGroups_.clear();
    atoms_.clear();
    title_ = title;
  }

  void ProteinStructureContentHandler::startAtomGroup(const std::string &atomGroupName)
  {
    atomGroup_ = AtomGroup{};
    atomGroupName_ = atomGroupName;
  }

  void ProteinStructureContentHandler::endAtomGroup()
  {
    if ( atomGroupId_ != -1 ) {
      AtomGroup::id_t id = AtomGroup::id_t(atomGroupId_);
      AtomGroup atomGroup{id, atomGroupName_};
      atomGroup.add(atomGroup_.atoms());
      atomGroups_.push_back(atomGroup);
    } else {
      AtomGroup atomGroup(atomGroupName_);
      atomGroup.add(atomGroup_.atoms());
      atomGroups_.push_back(atomGroup);
    }
    atomGroupId_ = -1;
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
    atom_ptr_t atom = Atom::make(atomName_, r_, spec);
    atoms_.push_back(atom);
    atomGroup_.add(atom);
  }

  void ProteinStructureContentHandler::index(int index)
  {
    if (atomGroupId_ == -1 ) {
      atomGroupId_ = index;
    }
  }

  prot_struct_ptr_t ProteinStructureContentHandler::proteinStructure() const
  {
    return ProteinStructure::make(title_, atoms_, atomGroups_);
  }

  std::shared_ptr<ProteinStructureContentHandler>
  ProteinStructureContentHandler::make(const atom_catalog_ptr_t& atomCatalog)
  {
    return std::make_shared<ProteinStructureContentHandler>(atomCatalog);
  }
}

