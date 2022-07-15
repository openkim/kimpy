// This file is generated automatically by generate_LanguageName_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_LanguageName.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(language_name, module)
{
  module.doc() = "Python binding to KIM_LanguageName.hpp";

  // classes

  py::class_<LanguageName> cl(module, "LanguageName");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &LanguageName::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &LanguageName::ToString);

  // functions

  module.def("get_language_name", [](int const index) {
    LanguageName language_name;

    int error = LANGUAGE_NAME::GetLanguageName(index, &language_name);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfLanguageNames!");
    }

    return language_name;
    }, R"pbdoc(
       Get the identity of each defined standard LanguageName.

       Returns:
           LanguageName: language_name
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_language_names", []() {
    int number_of_language_names;

    LANGUAGE_NAME::GetNumberOfLanguageNames(&number_of_language_names);

    return number_of_language_names;
    }, R"pbdoc(
       Get the number of standard LanguageName.

       Returns:
           int: number_of_language_names
       )pbdoc"
    );

  // attributes

  module.attr("cpp") = LANGUAGE_NAME::cpp;
  module.attr("c") = LANGUAGE_NAME::c;
  module.attr("fortran") = LANGUAGE_NAME::fortran;

}
