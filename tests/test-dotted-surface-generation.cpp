#include "simploce/surface/dotted-surface-generator.hpp"
#include "simploce/surface/surface.hpp"
#include "simploce/surface/vertex.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/tetrahedron-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/atom.hpp"
#include "simploce/factory.hpp"

using namespace simploce;

int main()
{
  // Regular tetrahedron with side length 2.
  std::clog << "Four Edge points of regular tetrahedron:" << std::endl;
  real_t el = 0.2;
  std::clog << "Edge length: " << el << std::endl;
  real_t c = 1.0/std::sqrt(el);
  std::clog << "Value of c: " << c << std::endl;

  atom_catalog_ptr_t atomCatalog = Factory::atomCatalog();
  atom_spec_ptr_t spec = atomCatalog->lookup("N");
  std::vector<atom_ptr_t> atoms;
  
  atom_ptr_t p1 = Atom::make(std::string{"N1"}, position_t{-1.0, 0.0, -c}, spec);
  atoms.push_back(p1);
  
  atom_ptr_t p2 = Atom::make(std::string{"N2"}, position_t{+1.0, 0.0, -c}, spec);
  atoms.push_back(p2);

  atom_ptr_t p3 = Atom::make(std::string{"N3"}, position_t{0.0, +1.0, -c}, spec);
  atoms.push_back(p3);
  
  atom_ptr_t p4 = Atom::make(std::string{"N4"}, position_t{0.0, -1.0, -c}, spec);
  atoms.push_back(p4);

  Surface surface = generateDottedSurface(atoms, 600);
  std::cout << surface << std::endl;

  return 0;
}
