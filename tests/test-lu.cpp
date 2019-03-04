#include "simploce/util/lu.hpp"
#include "simploce/bem/matrix-inversion.hpp"
#include "simploce/types.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

using namespace simploce;

int main()
{
  matrix_t O(3, 3);
  O(0,0) = 3.0;
  O(0,1) = 5.0;
  O(0,2) = 2.0;
  O(1,0) = 0.0;
  O(1,1) = 8.0;
  O(1,2) = 2.0;
  O(2,0) = 6.0;
  O(2,1) = 2.0;
  O(2,2) = 8.0;

  matrix_t A = O;
  
  std::cout << "Original matrix: " << O << std::endl;
  std::cout << std::endl;
  
  boost::numeric::ublas::vector<std::size_t> indx(3);
  luDecomposition<real_t,
		  boost::numeric::ublas::matrix,
		  boost::numeric::ublas::vector>(A, 3, indx);
  std::cout << "Factorized  matrix: " << A << std::endl;
  std::cout << "Indexes: " << indx << std::endl;
  std::cout << std::endl;
  
  inverseNoLU<real_t,
	      boost::numeric::ublas::matrix,
	      boost::numeric::ublas::vector>(A, 3, indx);
  std::cout << "Inverse matrix: "  << A << std::endl;
  matrix_t I = prod(O,A);
  std::cout << "A^-1 * A: " << I << std::endl;
  std::cout << std::endl;

  A = O;
  inverse<real_t,
	  boost::numeric::ublas::matrix,
	  boost::numeric::ublas::vector>(A, 3);
  std::cout << "Inverse matrix: "  << A << std::endl;
  I = prod(A,O);
  std::cout << "A^-1 * A: " << I << std::endl;
  std::cout << std::endl;

  A = O;
  invertMatrix(A);
  std::cout << "Inverse matrix: "  << A << std::endl;
  I = prod(A,O);
  std::cout << "A^-1 * A: " << I << std::endl;
  std::cout << std::endl;
  
  return 0;
}
