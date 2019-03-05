#include "simploce/util/lu.hpp"
//#include "simploce/bem/matrix-inversion.hpp"
#include "simploce/types.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

using namespace simploce;

int main()
{
  matrix_t O(3, 3);
  O(0,0) = 1.0;
  O(0,1) = 2.0;
  O(0,2) = 3.0;
  O(1,0) = 4.0;
  O(1,1) = 5.0;
  O(1,2) = 6.0;
  O(2,0) = 7.0;
  O(2,1) = 8.0;
  O(2,2) = 10.0;
  rvector_t b(3);
  b(0) = 3.0;
  b(1) = 3.0;
  b(2) = 4.0;

  matrix_t A = O;
  
  std::cout << "Original matrix: " << std::endl << O << std::endl;
  std::cout << "RHS: " << std::endl << b << std::endl;
  std::cout << std::endl;
  
  /*
    boost::numeric::ublas::vector<std::size_t> indx(3);
  luDecomposition<real_t,
		  boost::numeric::ublas::matrix,
		  boost::numeric::ublas::vector>(A, 3, indx);
  std::cout << "Factorized  matrix: " << A << std::endl;
  std::cout << "Indexes: " << indx << std::endl;
  std::cout << std::endl;
  */
  A = A.inverse();
  rvector_t x = A * b;
  std::cout << "Solution (from inverse): " << std::endl << x << std::endl;

  
  /**
     inverseNoLU<real_t,
	      boost::numeric::ublas::matrix,
	      boost::numeric::ublas::vector>(A, 3, indx);
     matrix_t I = prod(O,A);
  */
  matrix_t I = O * A;
  std::cout << "Inverse matrix: "  << std::endl << A << std::endl;
  std::cout << "A^-1 * A: " << std::endl << I << std::endl;
  std::cout << std::endl;

  A = O;
  /*
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
  */

  Eigen::ColPivHouseholderQR<matrix_t> luQR = A.colPivHouseholderQr();
  x = luQR.solve(b);
  std::cout << "Solution (from LU): " << std::endl << x << std::endl;
  A = luQR.inverse();
  I = O * A;
  std::cout << "Inverse matrix: "  << std::endl << A << std::endl;
  std::cout << "A^-1 * A: " << std::endl << I << std::endl;
  std::cout << std::endl;
  
  return 0;
}
