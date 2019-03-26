#include "simploce/bem/coulomb.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/factory.hpp"

using namespace simploce;

int main(int argc, char *argv[])
{
  atom_catalog_ptr_t atomCatalog = factory::atomCatalog();
  atom_spec_ptr_t spec = atomCatalog->lookup("T");
  
  permittivity_t epsI{4.0};
  Coulomb coulomb{epsI};
  
  std::size_t n{200};
  real_t dz{0.01};

  std::vector<Atom> atoms;
  position_t r{0.0, 0.0, 0.0};
  Atom atom1{"T1", r, spec};
  atoms.push_back(atom1);
  r[2] = n * dz;
  Atom atom2{"T2", r, spec};
  atoms.push_back(atom2);

  std::vector<elec_pot_t> potentials{};
  std::vector<position_t> positions;
  for (std::size_t i = 1; i != n; ++i) {
    positions.clear();
    real_t z = i * dz;
    position_t r0{0, 0, z};
    positions.push_back(r0);
    coulomb.potentials(atoms, positions, potentials);
    std::cout << z << " " << potentials[0] << std::endl;
  }
  
  return 0;
}
