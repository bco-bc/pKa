#ifndef MATRIX_INVERSION_HPP
#define MATRIX_INVERSION_HPP

#include "../types.hpp"

namespace simploce {

  /**
   * Calculates the inverse of a matrix.
   * @param matrix - Matrix. Upon return, replaced with its inverse.
   * @return True if successful.
   */
  bool invertMatrix(matrix_t& matrix);
}

#endif
