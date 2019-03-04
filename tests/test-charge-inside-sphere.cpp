#include "simploce/bem/flat-triangles-bem.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/util/util.hpp"
#include "simploce/factory.hpp"
#include "simploce/types.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>

using namespace simploce;
using bem_t = FlatTrianglesBEM;

int main(int argc, char *argv[])
{
  if ( argc == 1 ) {
    throw std::domain_error("Missing input triangulated surface file name.");
  }
  std::string fileName(argv[1]);
  std::ifstream stream;
  openInputFile(stream, fileName);

  // Spherical surface with radius 2.0 nm.
  tri_surface_ptr_t surface = TriangulatedSurface::make(stream);

  permittivity_t epsI{4.0};
  permittivity_t epsO{78.5};
  ionic_strength_t I{0.0};

  matrix_t S{};  
  bem_t bem(epsI, epsO, I);
  bem.kernels(*surface, S);
  //std::cout << S << std::endl;
  //std::cout << std::endl;

  atom_catalog_ptr_t atomCatalog = Factory::atomCatalog();
  atom_spec_ptr_t spec = atomCatalog->lookup("T");
  
  vector_t b{}, x{};
  std::vector<Atom> atoms;
  std::vector<position_t> positions;
  std::vector<elec_pot_t> potentials;
  for (std::size_t i = 0; i != 19; ++i) {
    positions.clear();
    atoms.clear();
    position_t r{0.0, 0.0, i * 0.1};
    positions.push_back(r);
    atoms.push_back(Atom{"T", r, spec});
    bem.rhs(atoms, *surface, b);
    //std::cout << "RHS:" << std::endl;
    //std::cout << b << std::endl;
    //std::cout << std::endl;
    bem.solve(S, b, x);
    //std::cout << "Solution:" << std::endl;
    //std::cout << x << std::endl;
    //std::cout << std::endl;
    bem.integrate(*surface, x, positions, potentials);
    //std::cout << "Potentials:" << std::endl;
    for (std::size_t k = 0; k != potentials.size(); ++k) {
      position_t r = positions[k];
      std::cout << r[2] << " " << potentials[k] << std::endl;
    }
  }
}
