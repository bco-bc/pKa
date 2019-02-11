#include "simploce/protein/logger-content-handler.hpp"

namespace simploce {

  LoggerContentHandler::LoggerContentHandler() :
    ContentHandler{}, title_{}, nMolecules_{0}, nAtomGroups_{0}, nAtoms_{0}, totalCharge_{0}
  {
  }
  
  void LoggerContentHandler::start(const std::string &title)
  {
    std::clog << "Start of chemical content." << std::endl;
    std::clog << "Title: " << title;
    title_ = title;
  }
  
  void LoggerContentHandler::end()
  {
    std::clog << "End of chemical content." << std::endl;
  }
  
  void LoggerContentHandler::startAtomGroup(const std::string &atomGroupName)
  {
    std::clog << "Start of atom group" << std::endl;
    std::clog << "Atom group name: " << atomGroupName << std::endl;
    nAtomGroups_++;
  }
  
  void LoggerContentHandler::endAtomGroup()
  {
    std::clog << "End of atom group." << std::endl;
  }
  
  void LoggerContentHandler::startMolecule(const std::string &moleculeName)
  {
    std::clog << "Start of new molecule." << std::endl;
    std::clog << "Molecule name: " << moleculeName << std::endl;
    nMolecules_++;
  }
  
  void LoggerContentHandler::endMolecule()
  {
    std::clog << "End of molecule." << std::endl;
  }
    
  void LoggerContentHandler::startAtom(const std::string &atomName)
  {
    std::clog << "Start of new atom." << std::endl;
    std::clog << "Atom name: " << atomName << std::endl;
    nAtoms_++;
  }

  void LoggerContentHandler::atomPosition(const position_t& r)
  {
    std::clog << "Atom position: " << r << std::endl;
  }

  void LoggerContentHandler::atomCharge(const charge_t& charge)
  {
    std::clog << "Atom charge: " << charge << std::endl;
    totalCharge_ += charge;
  }
    
  void LoggerContentHandler::endAtom()
  {
    std::clog << "End of atom" << std::endl;
  }
  
  void LoggerContentHandler::index(int index)
  {
    std::clog << "Index: " << index << std::endl;
  }

  std::string LoggerContentHandler::title() const
  {
    return title_;
  }

  std::size_t LoggerContentHandler::numberOfMolecules() const
  {
    return nMolecules_;
  }
  
  std::size_t LoggerContentHandler::numberOfAtomGroups() const
  {
    return nAtomGroups_;
  }
  
  std::size_t LoggerContentHandler::numberOfAtoms() const
  {
    return nAtoms_;
  }

  charge_t LoggerContentHandler::totalCharge() const
  {
    return totalCharge_;
  }
  
}

