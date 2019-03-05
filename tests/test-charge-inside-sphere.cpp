#include "simploce/bem/flat-triangles-bem.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/util/util.hpp"
#include "simploce/util/mu-units.hpp"
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
  radius_t radius = 2.0;
  tri_surface_ptr_t surface = TriangulatedSurface::make(stream);

  permittivity_t epsI{4.0};
  permittivity_t epsO{78.5};
  ionic_strength_t I{0.0};

  bem_t bem(epsI, epsO, I);
  bem.kernels(*surface);
  //std::clog << S << std::endl;
  //std::clog << std::endl;

  atom_catalog_ptr_t atomCatalog = Factory::atomCatalog();
  atom_spec_ptr_t spec = atomCatalog->lookup("T");

  std::size_t N = 200;
  real_t dr = radius / N;
  rvector_t b{};
  std::vector<Atom> atoms;
  std::vector<position_t> positions{};
  std::vector<elec_pot_t> potentials{};
  for (std::size_t i = 0; i != N; ++i) {
    positions.clear();
    atoms.clear();
    position_t r{0.0, 0.0, i * dr};
    //position_t r{0.0, 0.0, 1.0};
    positions.push_back(r);
    atoms.push_back(Atom{"T", r, spec});
    bem.rhs(atoms, *surface, b);
    //    std::clog << "RHS:" << std::endl;
    //std::clog << b << std::endl;
    //std::clog << std::endl;
    bem.solve(b);
    //std::clog << "Solution:" << std::endl;
    //std::clog << b << std::endl;
    //std::clog << std::endl;
    bem.integrate(*surface, b, positions, potentials);
    //std::clog << "Reaction Potentials:" << std::endl;
    for (std::size_t k = 0; k != potentials.size(); ++k) {
      // Convert to V.
      elec_pot_t rp = potentials[k] / MUUnits<real_t>::V_to_kJ_mol_e;
      position_t r = positions[k];
      std::cout << radius - r[2] << " " << rp << std::endl;
    }
  }
}
