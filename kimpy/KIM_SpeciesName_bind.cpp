// This file is generated automatically by generate_SpeciesName_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_SpeciesName.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(species_name, module)
{
  module.doc() = "Python binding to KIM_SpeciesName.hpp";

  // classes

  py::class_<SpeciesName> cl(module, "SpeciesName");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &SpeciesName::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &SpeciesName::ToString);

  // functions

  module.def("get_species_name", [](int const index) {
    SpeciesName species_name;

    int error = SPECIES_NAME::GetSpeciesName(index, &species_name);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfSpeciesNames!");
    }

    return species_name;
    }, R"pbdoc(
       Get the identity of each defined standard SpeciesName.

       Returns:
           SpeciesName: species_name
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_species_names", []() {
    int number_of_species_names;

    SPECIES_NAME::GetNumberOfSpeciesNames(&number_of_species_names);

    return number_of_species_names;
    }, R"pbdoc(
       Get the number of standard SpeciesName.

       Returns:
           int: number_of_species_names
       )pbdoc"
    );

  // attributes

  module.attr("electron") = SPECIES_NAME::electron;
  module.attr("H") = SPECIES_NAME::H;
  module.attr("He") = SPECIES_NAME::He;
  module.attr("Li") = SPECIES_NAME::Li;
  module.attr("Be") = SPECIES_NAME::Be;
  module.attr("B") = SPECIES_NAME::B;
  module.attr("C") = SPECIES_NAME::C;
  module.attr("N") = SPECIES_NAME::N;
  module.attr("O") = SPECIES_NAME::O;
  module.attr("F") = SPECIES_NAME::F;
  module.attr("Ne") = SPECIES_NAME::Ne;
  module.attr("Na") = SPECIES_NAME::Na;
  module.attr("Mg") = SPECIES_NAME::Mg;
  module.attr("Al") = SPECIES_NAME::Al;
  module.attr("Si") = SPECIES_NAME::Si;
  module.attr("P") = SPECIES_NAME::P;
  module.attr("S") = SPECIES_NAME::S;
  module.attr("Cl") = SPECIES_NAME::Cl;
  module.attr("Ar") = SPECIES_NAME::Ar;
  module.attr("K") = SPECIES_NAME::K;
  module.attr("Ca") = SPECIES_NAME::Ca;
  module.attr("Sc") = SPECIES_NAME::Sc;
  module.attr("Ti") = SPECIES_NAME::Ti;
  module.attr("V") = SPECIES_NAME::V;
  module.attr("Cr") = SPECIES_NAME::Cr;
  module.attr("Mn") = SPECIES_NAME::Mn;
  module.attr("Fe") = SPECIES_NAME::Fe;
  module.attr("Co") = SPECIES_NAME::Co;
  module.attr("Ni") = SPECIES_NAME::Ni;
  module.attr("Cu") = SPECIES_NAME::Cu;
  module.attr("Zn") = SPECIES_NAME::Zn;
  module.attr("Ga") = SPECIES_NAME::Ga;
  module.attr("Ge") = SPECIES_NAME::Ge;
  module.attr("As") = SPECIES_NAME::As;
  module.attr("Se") = SPECIES_NAME::Se;
  module.attr("Br") = SPECIES_NAME::Br;
  module.attr("Kr") = SPECIES_NAME::Kr;
  module.attr("Rb") = SPECIES_NAME::Rb;
  module.attr("Sr") = SPECIES_NAME::Sr;
  module.attr("Y") = SPECIES_NAME::Y;
  module.attr("Zr") = SPECIES_NAME::Zr;
  module.attr("Nb") = SPECIES_NAME::Nb;
  module.attr("Mo") = SPECIES_NAME::Mo;
  module.attr("Tc") = SPECIES_NAME::Tc;
  module.attr("Ru") = SPECIES_NAME::Ru;
  module.attr("Rh") = SPECIES_NAME::Rh;
  module.attr("Pd") = SPECIES_NAME::Pd;
  module.attr("Ag") = SPECIES_NAME::Ag;
  module.attr("Cd") = SPECIES_NAME::Cd;
  module.attr("In") = SPECIES_NAME::In;
  module.attr("Sn") = SPECIES_NAME::Sn;
  module.attr("Sb") = SPECIES_NAME::Sb;
  module.attr("Te") = SPECIES_NAME::Te;
  module.attr("I") = SPECIES_NAME::I;
  module.attr("Xe") = SPECIES_NAME::Xe;
  module.attr("Cs") = SPECIES_NAME::Cs;
  module.attr("Ba") = SPECIES_NAME::Ba;
  module.attr("La") = SPECIES_NAME::La;
  module.attr("Ce") = SPECIES_NAME::Ce;
  module.attr("Pr") = SPECIES_NAME::Pr;
  module.attr("Nd") = SPECIES_NAME::Nd;
  module.attr("Pm") = SPECIES_NAME::Pm;
  module.attr("Sm") = SPECIES_NAME::Sm;
  module.attr("Eu") = SPECIES_NAME::Eu;
  module.attr("Gd") = SPECIES_NAME::Gd;
  module.attr("Tb") = SPECIES_NAME::Tb;
  module.attr("Dy") = SPECIES_NAME::Dy;
  module.attr("Ho") = SPECIES_NAME::Ho;
  module.attr("Er") = SPECIES_NAME::Er;
  module.attr("Tm") = SPECIES_NAME::Tm;
  module.attr("Yb") = SPECIES_NAME::Yb;
  module.attr("Lu") = SPECIES_NAME::Lu;
  module.attr("Hf") = SPECIES_NAME::Hf;
  module.attr("Ta") = SPECIES_NAME::Ta;
  module.attr("W") = SPECIES_NAME::W;
  module.attr("Re") = SPECIES_NAME::Re;
  module.attr("Os") = SPECIES_NAME::Os;
  module.attr("Ir") = SPECIES_NAME::Ir;
  module.attr("Pt") = SPECIES_NAME::Pt;
  module.attr("Au") = SPECIES_NAME::Au;
  module.attr("Hg") = SPECIES_NAME::Hg;
  module.attr("Tl") = SPECIES_NAME::Tl;
  module.attr("Pb") = SPECIES_NAME::Pb;
  module.attr("Bi") = SPECIES_NAME::Bi;
  module.attr("Po") = SPECIES_NAME::Po;
  module.attr("At") = SPECIES_NAME::At;
  module.attr("Rn") = SPECIES_NAME::Rn;
  module.attr("Fr") = SPECIES_NAME::Fr;
  module.attr("Ra") = SPECIES_NAME::Ra;
  module.attr("Ac") = SPECIES_NAME::Ac;
  module.attr("Th") = SPECIES_NAME::Th;
  module.attr("Pa") = SPECIES_NAME::Pa;
  module.attr("U") = SPECIES_NAME::U;
  module.attr("Np") = SPECIES_NAME::Np;
  module.attr("Pu") = SPECIES_NAME::Pu;
  module.attr("Am") = SPECIES_NAME::Am;
  module.attr("Cm") = SPECIES_NAME::Cm;
  module.attr("Bk") = SPECIES_NAME::Bk;
  module.attr("Cf") = SPECIES_NAME::Cf;
  module.attr("Es") = SPECIES_NAME::Es;
  module.attr("Fm") = SPECIES_NAME::Fm;
  module.attr("Md") = SPECIES_NAME::Md;
  module.attr("No") = SPECIES_NAME::No;
  module.attr("Lr") = SPECIES_NAME::Lr;
  module.attr("Rf") = SPECIES_NAME::Rf;
  module.attr("Db") = SPECIES_NAME::Db;
  module.attr("Sg") = SPECIES_NAME::Sg;
  module.attr("Bh") = SPECIES_NAME::Bh;
  module.attr("Hs") = SPECIES_NAME::Hs;
  module.attr("Mt") = SPECIES_NAME::Mt;
  module.attr("Ds") = SPECIES_NAME::Ds;
  module.attr("Rg") = SPECIES_NAME::Rg;
  module.attr("Cn") = SPECIES_NAME::Cn;
  module.attr("Nh") = SPECIES_NAME::Nh;
  module.attr("Fl") = SPECIES_NAME::Fl;
  module.attr("Mc") = SPECIES_NAME::Mc;
  module.attr("Lv") = SPECIES_NAME::Lv;
  module.attr("Ts") = SPECIES_NAME::Ts;
  module.attr("Og") = SPECIES_NAME::Og;
  module.attr("user01") = SPECIES_NAME::user01;
  module.attr("user02") = SPECIES_NAME::user02;
  module.attr("user03") = SPECIES_NAME::user03;
  module.attr("user04") = SPECIES_NAME::user04;
  module.attr("user05") = SPECIES_NAME::user05;
  module.attr("user06") = SPECIES_NAME::user06;
  module.attr("user07") = SPECIES_NAME::user07;
  module.attr("user08") = SPECIES_NAME::user08;
  module.attr("user09") = SPECIES_NAME::user09;
  module.attr("user10") = SPECIES_NAME::user10;
  module.attr("user11") = SPECIES_NAME::user11;
  module.attr("user12") = SPECIES_NAME::user12;
  module.attr("user13") = SPECIES_NAME::user13;
  module.attr("user14") = SPECIES_NAME::user14;
  module.attr("user15") = SPECIES_NAME::user15;
  module.attr("user16") = SPECIES_NAME::user16;
  module.attr("user17") = SPECIES_NAME::user17;
  module.attr("user18") = SPECIES_NAME::user18;
  module.attr("user19") = SPECIES_NAME::user19;
  module.attr("user20") = SPECIES_NAME::user20;

}
