#ifndef ID_HPP
#define ID_HPP

#include <iostream>

namespace simploce {

  /**
   * Identifier. T is the entity type and V is identifier value type (e.g. std::size_t). 
   * V must implement the += operator. The Id value is increased sequentially. The start and 
   * increment value is controlled by the method <code>use(V start, V increment)</code>.
   */
  template <typename T, typename V>
  class Id {
  public:
    
    /**
     * Default constructor. Always set identifier value to V{}.
     */
    Id() {} 
    
    /**
     * Constructor.
     * @param id Identifier value. Sets identifier value to V{id}.
     */
    Id(V id) : id_{id} {}
    
    /**
     * Returns identifier value.
     * @return Value. This value is 0 if the identifier is not assigned.
     */
    V value() const { return id_; }
    
    /**
     * Returns identifier value.
     * @return Value. This value is 0 if the identifier is not assigned.
     */
    V operator () () const { return id_; }
    
    /**
     * Returns next identifier.
     */
    static Id<T,V> next();
    
    /**
     * Sets start value and increment for id. Use this to override start value of 1
     * and increment value of 1.
     * @param start Start value.
     * @param increment Increment value.
     */
    static void use(V start, V increment) { ID = start; INCREMENT = increment; }
    
  protected:
    
    /**
     * Returns next identifier value. 
     * @return Value of type T.
     */
    static V nextIdValue() { V next = ID; ID += INCREMENT; return next; }
    
  private:
    
    // Next value.
    static V ID;
    
    static V INCREMENT;
    
    V id_{};
  };
  
  template <typename T, typename V>
  Id<T,V> Id<T,V>::next()
  {
    V id = Id<T,V>::nextIdValue();
    return Id<T,V>{id};
  }
  
  template <typename T, typename V>
  V Id<T,V>::ID{1};
  
  template <typename T, typename V>
  V Id<T,V>::INCREMENT{1};
  
  /**
   * Smaller than operator.
   * @param id1 Identifier.
   * @param id2 Identifier.
   * @return Result.
   */
  template <typename T, typename V>
  inline bool operator < (const Id<T,V>& id1, const Id<T,V>& id2)
  {
    return id1() < id2();
  }
  
  /**
   * Greater than operator.
   * @param id1 Identifier.
   * @param id2 Identifier.
   * @return Result.
   */
  template <typename T, typename V>
  inline bool operator > (const Id<T,V>& id1, const Id<T,V>& id2)
  {
    return id1() > id2();
  }
  
  /**
   * Is equal operator.
   * @param id1 Identifier.
   * @param id2 Identifier.
   * @return Result.
   */
  template <typename T, typename V>
  inline bool operator == (const Id<T,V>& id1, const Id<T,V>& id2) 
  { 
    return id1() == id2(); 
  }
  
  /**
   * Is not equal operator.
   * @param id1 Identifier.
   * @param id2 Identifier.
   * @return Result.
   */
  template <typename T, typename V>
  inline bool operator != (const Id<T,V>& id1, const Id<T,V>& id2) 
  { 
    return id1() != id2(); 
  }
  
  /**
   * Write identifier value to output stream.
   * @param id Identifier.
   * @param stream Output stream.
   * @return Output stream.
   */
  template <typename T, typename V>
  std::ostream & operator << (std::ostream& stream, const Id<T,V>& id)
  {
    stream << id();
    return stream;
  }
  
}

#endif
