#ifndef BASE_CONTENT_HANDLER_HPP
#define BASE_CONTENT_HANDLER_HPP

#include "content-handler.hpp"

namespace simploce {

  /**
   * Receives notifications, but ignores them. All content handlers may be derived from this class,
   * by overriding just these methods they require.
   */ 
  struct BaseContentHandler : public ContentHandler {
    
    BaseContentHandler();

    virtual ~BaseContentHandler();
    
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
  };
}

#endif
