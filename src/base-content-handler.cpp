#include "simploce/protein/base-content-handler.hpp"

namespace simploce {

  BaseContentHandler::BaseContentHandler() : ContentHandler() {}

  BaseContentHandler::~BaseContentHandler()
  {
  }
  
  void BaseContentHandler::start(const std::string &title)
  {
  }
  
  void BaseContentHandler::end()
  {
  }
  
  void BaseContentHandler::startAtomGroup(const std::string &atomGroupName)
  {
  }
  
  void BaseContentHandler::endAtomGroup()
  {
  }
  
  void BaseContentHandler::startMolecule(const std::string &moleculeName)
  {
  }
  
  void BaseContentHandler::endMolecule()
  {
  }
  
  void BaseContentHandler::startAtom(const std::string &atomName)
  {
  }
  
  void BaseContentHandler::atomPosition(const position_t& r)
  {
  }
  
  void BaseContentHandler::atomCharge(const charge_t& charge)
  {
  }
  
  void BaseContentHandler::endAtom()
  {
  }
  
  void BaseContentHandler::index(int index)
  {
  }
  
}
