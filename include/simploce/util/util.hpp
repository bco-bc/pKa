#ifndef UTIL_HPP
#define UTIL_HPP

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <future>
#include <cfenv>

namespace simploce {

  /**
   * Open input file.
   * @param stream Input stream.
   * @param fileName Input file name.
   * @return Input stream.
   */
  std::ifstream& openInputFile(std::ifstream& stream, const std::string& fileName);
  
  /**
   * Open input file.
   * @param stream Output stream.
   * @param fileName Output file name.
   * @return Output stream.
   */
  std::ofstream& openOutputFile(std::ofstream& stream, const std::string& fileName);
  
  /**
   * Signum.
   * T is any type that supports the default constructor T{0} and the < operator.
   * @param val Some value of type T.
   * @return -1 (negative number), 1 (positive number), or 0 (zero).
   * @see <a href="http://en.wikipedia.org/wiki/Sign_function">Sign function</a>
   */
  template <typename T> 
  int sgn(T val)
  {
    return (T{0} < val) - (val < T{0});
  }
  
  /**
   * Sign copying function. The type T of the return value is that of a and b.
   * It must support the T{0} constructor and the >= operator.
   * If b >= 0 then the result is abs(a), else it is -abs(a). 
   * @return Value of a with the sign of b. 
   */
  template <typename T>
  T signCopy(T a, T b)
  {
    return ( b >= T{0} ? std::fabs(a) : -std::fabs(a) );
  }
  
  /**
   * Very -simple- random number generator.
   * V is the value type of real numbers.
   * @return Random value in the range [0,1].
   */
  template <typename V>
  V random()
  {
    static bool init = false;
    if ( !init ) {
      std::time_t timer;
      V seed = time(&timer);
      srand(seed);
      init = true;
    }
    int n = std::rand();
    V v = V(n) / RAND_MAX;
    return v;
  }

  /**
   * Returns nearest integer.
   * V is the value type.
   * @param val Val.
   * @return Nearest integer.
   */
  template <typename V, typename I = int>
  I nint(V val)
  {
    std::fesetround(FE_TONEAREST);
    return std::nearbyint(val);
  }
  
  /**
   * Returns seed value.
   * @return Seed value.
   */
  template <typename V>
  V seed()
  {
    std::time_t timer;
    V seed = time(&timer);
    srand(seed);
    seed += std::rand(); 
    return seed;
  }
  
  /**
   * Waits for all future results before returning. T is result type stored in the future.
   * @param futures Futures.
   * @return Results of futures.
   */
  template<typename T>
  std::vector<T> wait_for_all(std::vector<std::future<T> >& futures)
  {
    std::vector<T> results;
    for (auto& f : futures)
      results.push_back(f.get());
    return results;
  }    

  /**
   * Compresses data.
   * @param data Data
   * @return Compressed as string.
   * @see https://stackoverflow.com/questions/27529570/simple-zlib-c-string-compression-and-decompression
   */
  template <typename T>
  std::string compress(const T& data)
  {
    static bool init{false};
    static boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    if ( !init ) {
      out.push(boost::iostreams::bzip2_compressor());
      init = true;
    }

    std::stringstream compressed;
    std::stringstream original;
    original << data;
    out.push(original);
    boost::iostreams::copy(out, compressed);

    /**need to encode here *
    std::string compressed_encoded = 
         base64_encode(reinterpret_cast<const unsigned char*>(compressed.c_str()), 
                                                              compressed.length());
    
    return compressed_encoded.str();
    */
    return compressed.str();
  }

  /**
   * Decompresses a compressed data.
   * @param compressed Compressed data.
   * @return Original data.
   * @see https://stackoverflow.com/questions/27529570/simple-zlib-c-string-compression-and-decompression
   */
  template <typename T>
  T decompress(const std::string& compressed)
  {
    static bool init{false};
    static boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    if ( !init ) {
      in.push(boost::iostreams::bzip2_decompressor());
      init = true;
    }
    
    //std::stringstream original_encoded;
    std::stringstream original;
    std::stringstream decompressed;
    original << compressed;

    /** first decode  then decompress **/
    //std::string compressed = base64_decode(compressed_encoded);

    in.push(original);
    boost::iostreams::copy(in, decompressed);
    return boost::lexical_cast<T,std::string>(decompressed.str());
 }

}

#endif
