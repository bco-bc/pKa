#include "simploce/bem/matrix-inversion.hpp"

// REMEMBER to update "lu.hpp" header includes from boost-CVS
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace simploce {

  using namespace boost::numeric::ublas;

  /**
   * See http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?LU_Matrix_Inversion
   */
  bool invertMatrix(matrix_t& matrix)
  {
    using pmatrix = permutation_matrix<std::size_t>;
    
    // Create a working copy of the input
    matrix_t A(matrix);
    
    // Create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());

    // Perform LU-factorization
    int res = lu_factorize(A, pm);
    if ( res != 0 ) {
      return false;
    }

    // Create identity matrix of "inverse"
    matrix.assign(identity_matrix<real_t>(A.size1()));

    // Backsubstitute to get the inverse
    lu_substitute(A, pm, matrix);

    return true;
  }
}

