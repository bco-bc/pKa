#ifndef CONTENT_HANDLER_HPP
#define CONTENT_HANDLER_HPP

#include "../types.hpp"
#include <string>

namespace simploce {

  /**
   * Receives notification of chemical content in an input source from a chemical content
   * reader.
   */
  struct ContentHandler {

    virtual ~ContentHandler() {}

    /**
     * Receive notification of beginning of chemical content
     * @param title - Title of chemical content. Serves as an identification of the content.
    */
    virtual void start(const std::string &title) = 0;

    /**
     * Receive notification of beginning of chemical content
     */
    virtual void end() = 0;

    /**
     * Receive notification of the beginning of an atom group.
     * @param atomGroupName - Name of the group
     */
    virtual void startAtomGroup(const std::string &atomGroupName) = 0;

    /**
     * Receive notification of the end of an atom group.
     */
    virtual void endAtomGroup() = 0;

    /**
     * Receive notification of the beginning of a molecule
     * @param moleculeName - Name of the molecule
     */
    virtual void startMolecule(const std::string &moleculeName) = 0;

    /**
     * Receive notification of the end of a molecule
     */
    virtual void endMolecule() = 0;

    /**
     * Receive notification of the beginning of an atom
     * @param atomName - Name of the atom
     */
    virtual void startAtom(const std::string &atomName) = 0;

    /**
     * Receive notification of the atom position.
     * @param r - Atom position.
     */
    virtual void atomPosition(const position_t& r) = 0;

    /**
     * Receive notification of atom charge.
     * @param charge Charge value.
     */
    virtual void atomCharge(const charge_t& charge) = 0;

    /**
     * Receive notification of the end of an atom
     */
    virtual void endAtom() = 0;

    /**
     * Receives notification of an index (e.g. residue number, atom number).
     * @param index - Index
     */
    virtual void index(int index) = 0;

  };
    
}

#endif

