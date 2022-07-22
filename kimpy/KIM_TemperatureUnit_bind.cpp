// This file is generated automatically by generate_UnitSystem_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_TemperatureUnit.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(temperature_unit, module)
{
  module.doc() = "Python binding to KIM_TemperatureUnit.hpp";

  // classes

  py::class_<TemperatureUnit> cl(module, "TemperatureUnit");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &TemperatureUnit::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &TemperatureUnit::ToString);

  // functions

  module.def("get_temperature_unit", [](int const index) {
    TemperatureUnit temperature_unit;

    int error = TEMPERATURE_UNIT::GetTemperatureUnit(index, &temperature_unit);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfTemperatureUnits!");
    }

    return temperature_unit;
    }, R"pbdoc(
       Get the identity of each defined standard TemperatureUnit.

       Returns:
           TemperatureUnit: temperature_unit
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_temperature_units", []() {
    int number_of_temperature_units;

    TEMPERATURE_UNIT::GetNumberOfTemperatureUnits(&number_of_temperature_units);

    return number_of_temperature_units;
    }, R"pbdoc(
       Get the number of standard TemperatureUnit.

       Returns:
           int: number_of_temperature_units
       )pbdoc"
    );

  // attributes

  module.attr("unused") = TEMPERATURE_UNIT::unused;
  module.attr("K") = TEMPERATURE_UNIT::K;

}
