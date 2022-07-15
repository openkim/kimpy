// This file is generated automatically by generate_UnitSystem_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_EnergyUnit.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(energy_unit, module)
{
  module.doc() = "Python binding to KIM_EnergyUnit.hpp";

  // classes

  py::class_<EnergyUnit> cl(module, "EnergyUnit");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &EnergyUnit::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &EnergyUnit::ToString);

  // functions

  module.def("get_energy_unit", [](int const index) {
    EnergyUnit energy_unit;

    int error = ENERGY_UNIT::GetEnergyUnit(index, &energy_unit);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfEnergyUnits!");
    }

    return energy_unit;
    }, R"pbdoc(
       Get the identity of each defined standard EnergyUnit.

       Returns:
           EnergyUnit: energy_unit
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_energy_units", []() {
    int number_of_energy_units;

    ENERGY_UNIT::GetNumberOfEnergyUnits(&number_of_energy_units);

    return number_of_energy_units;
    }, R"pbdoc(
       Get the number of standard EnergyUnit.

       Returns:
           int: number_of_energy_units
       )pbdoc"
    );

  // attributes

  module.attr("unused") = ENERGY_UNIT::unused;
  module.attr("amu_A2_per_ps2") = ENERGY_UNIT::amu_A2_per_ps2;
  module.attr("erg") = ENERGY_UNIT::erg;
  module.attr("eV") = ENERGY_UNIT::eV;
  module.attr("Hartree") = ENERGY_UNIT::Hartree;
  module.attr("J") = ENERGY_UNIT::J;
  module.attr("kcal_mol") = ENERGY_UNIT::kcal_mol;

}
