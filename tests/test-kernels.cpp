#include "simploce/bem/kernels.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/util/util.hpp"
#include <boost/numeric/ublas/io.hpp>
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>

using namespace simploce;

int main(int argc, char *argv[])
{
  if ( argc == 1 ) {
    throw std::domain_error("Missing input triangulated surface file name.");
  }
  std::string fileName(argv[1]);
  std::ifstream stream;
  openInputFile(stream, fileName);

  tri_surface_ptr_t surface = TriangulatedSurface::make(stream);

  permittivity_t epsI{20.0};
  permittivity_t epsO{80.0};
  ionic_strength_t I{0.0};

  matrix_t L;  
  Kernels kernels(epsI, epsO, I);
  kernels.L0(*surface, L);

  std::cout << L << std::endl;

}
