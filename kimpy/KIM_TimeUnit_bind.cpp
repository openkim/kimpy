// This file is generated automatically by generate_UnitSystem_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_TimeUnit.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(time_unit, module)
{
  module.doc() = "Python binding to KIM_TimeUnit.hpp";

  // classes

  py::class_<TimeUnit> cl(module, "TimeUnit");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &TimeUnit::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &TimeUnit::ToString);

  // functions

  module.def("get_time_unit", [](int const index) {
    TimeUnit time_unit;

    int error = TIME_UNIT::GetTimeUnit(index, &time_unit);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfTimeUnits!");
    }

    return time_unit;
    }, R"pbdoc(
       Get the identity of each defined standard TimeUnit.

       Returns:
           TimeUnit: time_unit
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_time_units", []() {
    int number_of_time_units;

    TIME_UNIT::GetNumberOfTimeUnits(&number_of_time_units);

    return number_of_time_units;
    }, R"pbdoc(
       Get the number of standard TimeUnit.

       Returns:
           int: number_of_time_units
       )pbdoc"
    );

  // attributes

  module.attr("unused") = TIME_UNIT::unused;
  module.attr("fs") = TIME_UNIT::fs;
  module.attr("ps") = TIME_UNIT::ps;
  module.attr("ns") = TIME_UNIT::ns;
  module.attr("s") = TIME_UNIT::s;

}
