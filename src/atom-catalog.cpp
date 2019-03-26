#include "simploce/protein/atom-catalog.hpp"
#include "simploce/protein/atom-spec.hpp"
#include "simploce/conf.hpp"
#include <utility>
#include <string>

namespace simploce {

  using pair = std::pair<std::string, atom_spec_ptr_t>;

  // R_WATER is specified in conf.hpp.
  
  static radius_t R_H{R_WATER + 0.12};
  static radius_t R_C{R_WATER + 0.17};
  static radius_t R_O{R_WATER + 0.152};
  static radius_t R_N{R_WATER + 0.155};
  static radius_t R_S{R_WATER + 0.18};
  static radius_t R_T{R_WATER + 0.10};  
  static charge_t zero{0.0};
  static charge_t one{1.0};

  static void setup_(std::map<std::string, atom_spec_ptr_t>& specs)
  {
    std::string H = "H";
    mass_t mass = 1.008;
    atom_spec_ptr_t specH = AtomSpec::make(H, R_H, zero, mass);
    pair p = std::make_pair(H, specH);
    specs.insert(p);

    std::string C = "C";
    mass = 12.0096;
    atom_spec_ptr_t specC = AtomSpec::make(C, R_C, zero, mass);
    p = std::make_pair(C, specC);
    specs.insert(p);

    std::string O = "O";
    mass = 15.99903;
    atom_spec_ptr_t specO = AtomSpec::make(O, R_O, zero, mass);
    p = std::make_pair(O, specO);
    specs.insert(p);

    std::string N = "N";
    mass = 14.00643;
    atom_spec_ptr_t specN = AtomSpec::make(N, R_N, zero, mass);
    p = std::make_pair(N, specN);
    specs.insert(p);
    
    std::string S = "S";
    mass = 32.059;
    atom_spec_ptr_t specS = AtomSpec::make(S, R_S, zero, mass);
    p = std::make_pair(S, specS);
    specs.insert(p);

    // Test charge.
    std::string T = "T";
    mass = 1;
    atom_spec_ptr_t specT = AtomSpec::make(H, R_T, one, mass);
    p = std::make_pair(T, specT);
    specs.insert(p);
  }

  AtomCatalog::AtomCatalog() :
    specs_{}
  {
    setup_(specs_);
  }

  atom_spec_ptr_t AtomCatalog::lookup(const std::string& name) const
  {
    using iter_t = std::map<std::string,atom_spec_ptr_t>::const_iterator;

    // Use first character only.
    std::string s = name.substr(0,1);
    iter_t iter = specs_.find(s);
    if ( iter == specs_.end() ) {
      throw std::runtime_error(name + ": No such atom specification.");
    }
    return (*iter).second;
  }

  atom_catalog_ptr_t AtomCatalog::make()
  {
    return std::make_shared<AtomCatalog>();
  }
  
}
