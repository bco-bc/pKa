#ifndef TCVECTOR_HPP
#define TCVECTOR_HPP

#include "../conf.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cassert>

namespace simploce {

  /**
   * Cartesian vector (cvector) with three components of type <code>V</code>, 
   * typically reals. D (for DISCRIMINATOR) ensures that each instantiation of 
   * cvector<V,D> with a different value for D but 
   * with the same type for V is its own type. For example,
   * <pre>
   *   using position_t = simploce::util::cvector<double, 1>;
   *   using velocity_t = simploce::util::cvector<double, 2>;
   * </pre>
   * Thus, position_t and velocity_t represent different types.
   * @see 
   * <a href="http://en.wikipedia.org/wiki/Cartesian_coordinate_system">
   *   Cartesian coordinate system
   * </a>
   * @see 
   * <a href="http://en.wikipedia.org/wiki/Euclidean_vector">
   *   Euclidian vector
   * </a>
   */    
  template <typename V, int D>
  class cvector {
  public:
    
    /**
     * Default constructible.
     */
    cvector() {}
    
    /**
     * Constructor.
     * @param v Vector. Its size must be at least 3.
     */
    cvector(const std::vector<V>& v) : elems_{v[0], v[1], v[2]} {}
    
    /**
     * Constructor.
     * @param x X-component.
     * @param y Y-component.
     * @param z Z-component.
     */
    cvector(V x, V y, V z) : elems_{x, y, z} {}
    
    /**
     * Returns x-component.
     * @return value of type V.
     */
    V x() const { return elems_[0]; }
    
    /**
     * Returns y-component.
     * @return value of type V.
     */
    V y() const { return elems_[1]; }
    
    /**
     * Returns z-component.
     * @return value of type V.
     */
    V z() const { return elems_[2]; }
    
    /**
     * Returns modifiable value of vector component.
     * @param k Identifies component: k=0,1,2 <-> x-, y-, z-component.
     * @return value of type V.
     */
    V& operator [] (int k) {  assert(k < 3); return elems_[k]; }
    
    /**
     * Returns unmodifiable value of vector component.
     * @param k Identifies component: k=0,1,2 <-> x-, y-, z-component.
     * @return value of type V.
     */
    V operator [] (int k) const { return elems_[k]; }
    
    /**
     * Addition.
     * @param v Same type Cartesian vector.
     * @return This Cartesian vector.
     */
    cvector<V,D>& operator += (const cvector<V,D>& v);
    
    /**
     * Subtraction.
     * @param v Same type Cartesian vector.
     * @return This Cartesian vector.
     */
    cvector<V,D>& operator -= (const cvector<V,D>& v);
    
    /**
     * Multiplication with a number.
     * @param t Number of type T.
     * @return This Cartesian vector.
     */
    template <typename T>
    cvector<V,D>& operator *= (T t); 
    
    /**
     * Division by a number,
     * @param t Number of type T.
     * @return This Cartesian vector.
     */
    template <typename T>
    cvector<V,D>& operator /= (T t);     
    
    /**
     * Resets components to zero.
     */
    void reset();
    
  private:
    
    V elems_[3]{0, 0, 0};
  };
  
  template <typename V, int D>
  cvector<V,D>& cvector<V,D>::operator += (const cvector<V,D>& v)
  {
    for (std::size_t k = 0; k != 3; ++k) {
      elems_[k] += v.elems_[k];
    }
    return *this;
  }
  
  template <typename V, int D>
  cvector<V,D>& cvector<V,D>::operator -= (const cvector<V,D>& v)
  {
    for (std::size_t k = 0; k != 3; ++k) {
      elems_[k] -= v.elems_[k];
    }
    return *this;
  }

  template <typename V, int D>
  template <typename T>
  cvector<V,D>& cvector<V,D>::operator *= (T t)
  {
    for (std::size_t k = 0; k != 3; ++k) {
      elems_[k] *= t;
    }
    return *this;
  }
  
  template <typename V, int D>
  template <typename T>
  cvector<V,D>& cvector<V,D>::operator /= (T t)
  {
    for (std::size_t k = 0; k != 3; ++k) {
      elems_[k] /= t;
    }
    return *this;
  }
  
  template <typename V, int D>
  void cvector<V,D>::reset()
  {
    for (std::size_t k = 0; k != 3; ++k) {
      elems_[k] = 0;
    }
  }
  
  /**
   * Adds two vectors of the -same- type.
   * @param v1 Vector 1.
   * @param v2 Vector 2.
   * @return Result: v1 + v2
   */
  template <typename V, int D>
  cvector<V,D> operator + (const cvector<V,D>& v1, const cvector<V,D>& v2)
  {
    cvector<V,D> v{};
    for ( std::size_t k = 0; k != 3; ++k ) {
      v[k] = v1[k] + v2[k];
    }
    return v;
  }
  
  /**
   * Subtracts two vectors of the -same- type.
   * @param v1 Vector 1.
   * @param v2 Vector 2.
   * @return Result: v1 - v2
   */
  template <typename V, int D>
  cvector<V,D> operator - (const cvector<V,D>& v1, const cvector<V,D>& v2)
  {
    cvector<V,D> v{};
    for ( std::size_t k = 0; k != 3; ++k ) {
      v[k] = v1[k] - v2[k];
    }
    return v;
  }
  
  /**
   * Multiplies vector with number from the left.
   * @param number Number of type T.
   * @param v Vector.
   * @return Result: number * v.
   */
  template <typename T, typename V, int D>
  cvector<V,D> operator * (T number, const cvector<V,D>& v)
  {
    cvector<V,D> result;
    for (std::size_t k = 0; k != 3; ++k) {
      result[k] = number * v[k];
    }
    return result;
  }
  
  /**
   * Multiplies vector with number from the right.
   * @param v Vector.
   * @param number Number of type T.
   * @return Result: v * number.
   */
  template <typename T, typename V, int D>
  inline cvector<V,D> operator * (const cvector<V,D>& v, T number)
  {
    return number * v;
  }
  
  /**
   * Division by number.
   * @param number Number of type T.
   * @param v Vector.
   * @return Result: v / number.
   */
  template <typename T, typename V, int D>
  inline cvector<V,D> operator / (const cvector<V,D>& v, T number)
  {
    return ( T{1.0}/number ) * v;
  }
  
  /**
   * Equality operator. Vectors should be the -same- type.
   * @param v1 Vector 1.
   * @param v2 Vector 2.
   * @return Result: v1 == v2.
   */
  template <typename V, int D>
  bool operator == (const cvector<V,D>& v1, const cvector<V,D>& v2)
  {
    V eps = std::numeric_limits<V>::epsilon();
    return ( std::fabs(v1[0] - v2[0]) < eps &&
	     std::fabs(v1[1] - v2[1]) < eps &&
	     std::fabs(v1[2] - v2[2]) < eps );
  }
  
  /**
   * Not equal operator. Vectors should be the -same- type.
   * @param v1 Vector 1.
   * @param v2 Vector 2.
   * @return Result: v1 != v2.
   */
  template <typename V, int D>
  inline bool operator != (const cvector<V,D>& v1, const cvector<V,D>& v2)
  {
    return ( v1 == v2 ? false : true );
  }
  
  /**
   * Returns norm (magnitude, length) of Cartesian vector.
   * @param v Cartesian vector.
   * @return Norm. A number of type T.
   */
  template <typename T, typename V, int D>
  T norm(const cvector<V,D>& v)
  {
    return std::sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
  }

  /**
   * Returns square of norm (magnitude, length) of Cartesian vector.
   * @param v Cartesian vector.
   * @return Square norm. A number of type T.
   */
  template <typename T, typename V, int D>
  T norm2(const cvector<V,D>& v)
  {
    return v.x() * v.x() + v.y() * v.y() + v.z() * v.z(); 
  }
  
  /**
   * Inner (or dot) product between two Cartesian vectors, possibly of different types.
   * @param v1 Vector of type defined by V1 and D1.
   * @param v2 Vector of type defined by V2 and D2.
   * @return Result. A number of type T.
   * @see <a href="https://en.wikipedia.org/wiki/Dot_product">Dot product</a>
   */
  template <typename T, typename V1, int D1, typename V2, int D2>
  T inner(const cvector<V1,D1>& v1, const cvector<V2,D2>& v2)
  {
    T result{0};
    for (std::size_t k = 0; k != 3; ++k) {
      result += v1[k] * v2[k];
    }
    return result;      
  }
  
  /**
   * Returns angle between two Cartesian vectors, possibly of different types.
   * @param v1 Vector of type defined by V1 and D1. Should have a non-zero norm.
   * @param v2 Vector of type defined by V2 and D2. Should have a non-zero norm.
   * @return Angle in rad (radian) in the range [0;pi].
   */
  template <typename T, typename V1, int D1, typename V2, int D2>
  T angle(const cvector<V1,D1>& v1, const cvector<V2,D2>& v2)
  { 
    T ip = inner<T>(v1, v2);
    T norm1 = norm<T>(v1);
    T norm2 = norm<T>(v2);
    T cos_angle = ip / (norm1 * norm2);
    return std::acos(cos_angle);
  }
  
  /**
   * Writes each vector component to output stream, with precision 5 and width 13. Note that 
   * stream.setf(...) is not called.
   * @param stream Output stream.
   * @param v Cartesian vector.
   * @return Output stream.
   */
  template <typename V, int D>
  std::ostream& operator << (std::ostream& stream, const cvector<V,D>& v)
  {
    stream.precision(PRECISION);
    stream << std::setw(WIDTH) << v.x();
    stream << SPACE << std::setw(WIDTH) << v.y();
    stream << SPACE << std::setw(WIDTH) << v.z();
    return stream;
  }
  
}

#endif
