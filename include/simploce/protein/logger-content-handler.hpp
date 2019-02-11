#ifndef LOGGER_CONTENT_HANDLER_HPP
#define LOGGER_CONTENT_HANDLER_HPP

#include "content-handler.hpp"
#include <iostream>

namespace simploce {
  
  /**
   * Simply logs everything it receives. It keeps track of the number of atoms, atom groups,
   * molecules, and the total charge received.
   */
  class LoggerContentHandler : public ContentHandler {
  public:

    /**
     * Constructor
     */
    LoggerContentHandler();

    virtual ~LoggerContentHandler() {}

    virtual void start(const std::string &title) override;

    virtual void end() override;

    virtual void startAtomGroup(const std::string &atomGroupName) override;

    virtual void endAtomGroup() override;
    
    virtual void startMolecule(const std::string &moleculeName) override;
    
    virtual void endMolecule() override;
    
    virtual void startAtom(const std::string &atomName) override;
    
    virtual void atomPosition(const position_t& r) override;
    
    virtual void atomCharge(const charge_t& charge) override;
    
    virtual void endAtom() override;
    
    virtual void index(int index) override;

    /**
     * Returns title.
     */
    std::string title() const;
    
    /**
     * Returns current number of molecules received.
     * @return Number of molecules received
     */
    std::size_t numberOfMolecules() const;
    
    /**
     * Returns the current number of atom groups received
     * @return Number of atom groups received
     */
    std::size_t numberOfAtomGroups() const;
    
    /**
     * Returns the current number of atoms received
     * @return Number of atoms received
     */
    std::size_t numberOfAtoms() const;
    
    /**
     * Returns total charge received.
     */
    charge_t totalCharge() const;
    
  private:

    std::string title_;
    std::size_t nMolecules_;
    std::size_t nAtomGroups_;
    std::size_t nAtoms_;
    charge_t totalCharge_;
  };
  
}

#endif
