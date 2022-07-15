// This file is generated automatically by generate_UnitSystem_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_LengthUnit.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(length_unit, module)
{
  module.doc() = "Python binding to KIM_LengthUnit.hpp";

  // classes

  py::class_<LengthUnit> cl(module, "LengthUnit");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &LengthUnit::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &LengthUnit::ToString);

  // functions

  module.def("get_length_unit", [](int const index) {
    LengthUnit length_unit;

    int error = LENGTH_UNIT::GetLengthUnit(index, &length_unit);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfLengthUnits!");
    }

    return length_unit;
    }, R"pbdoc(
       Get the identity of each defined standard LengthUnit.

       Returns:
           LengthUnit: length_unit
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_length_units", []() {
    int number_of_length_units;

    LENGTH_UNIT::GetNumberOfLengthUnits(&number_of_length_units);

    return number_of_length_units;
    }, R"pbdoc(
       Get the number of standard LengthUnit.

       Returns:
           int: number_of_length_units
       )pbdoc"
    );

  // attributes

  module.attr("unused") = LENGTH_UNIT::unused;
  module.attr("A") = LENGTH_UNIT::A;
  module.attr("Bohr") = LENGTH_UNIT::Bohr;
  module.attr("cm") = LENGTH_UNIT::cm;
  module.attr("m") = LENGTH_UNIT::m;
  module.attr("nm") = LENGTH_UNIT::nm;

}
