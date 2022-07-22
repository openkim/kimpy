// This file is generated automatically by generate_UnitSystem_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_ChargeUnit.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(charge_unit, module)
{
  module.doc() = "Python binding to KIM_ChargeUnit.hpp";

  // classes

  py::class_<ChargeUnit> cl(module, "ChargeUnit");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &ChargeUnit::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &ChargeUnit::ToString);

  // functions

  module.def("get_charge_unit", [](int const index) {
    ChargeUnit charge_unit;

    int error = CHARGE_UNIT::GetChargeUnit(index, &charge_unit);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfChargeUnits!");
    }

    return charge_unit;
    }, R"pbdoc(
       Get the identity of each defined standard ChargeUnit.

       Returns:
           ChargeUnit: charge_unit
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_charge_units", []() {
    int number_of_charge_units;

    CHARGE_UNIT::GetNumberOfChargeUnits(&number_of_charge_units);

    return number_of_charge_units;
    }, R"pbdoc(
       Get the number of standard ChargeUnit.

       Returns:
           int: number_of_charge_units
       )pbdoc"
    );

  // attributes

  module.attr("unused") = CHARGE_UNIT::unused;
  module.attr("C") = CHARGE_UNIT::C;
  module.attr("e") = CHARGE_UNIT::e;
  module.attr("statC") = CHARGE_UNIT::statC;

}
