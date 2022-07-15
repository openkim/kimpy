// This file is generated automatically by generate_Numbering_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_Numbering.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(numbering, module)
{
  module.doc() = "Python binding to KIM_Numbering.hpp";

  // classes

  py::class_<Numbering> cl(module, "Numbering");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &Numbering::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &Numbering::ToString);

  // functions

  module.def("get_numbering", [](int const index) {
    Numbering numbering;

    int error = NUMBERING::GetNumbering(index, &numbering);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfNumberings!");
    }

    return numbering;
    }, R"pbdoc(
       Get the identity of each defined standard Numbering.

       Returns:
           Numbering: numbering
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_numberings", []() {
    int number_of_numberings;

    NUMBERING::GetNumberOfNumberings(&number_of_numberings);

    return number_of_numberings;
    }, R"pbdoc(
       Get the number of standard Numbering.

       Returns:
           int: number_of_numberings
       )pbdoc"
    );

  // attributes

  module.attr("zeroBased") = NUMBERING::zeroBased;
  module.attr("oneBased") = NUMBERING::oneBased;

}
