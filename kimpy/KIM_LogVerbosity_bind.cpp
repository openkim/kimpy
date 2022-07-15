// This file is generated automatically by generate_LogVerbosity_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_LogVerbosity.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(log_verbosity, module)
{
  module.doc() = "Python binding to KIM_LogVerbosity.hpp";

  // classes

  py::class_<LogVerbosity> cl(module, "LogVerbosity");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &LogVerbosity::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &LogVerbosity::ToString);

  // functions

  module.def("get_log_verbosity", [](int const index) {
    LogVerbosity log_verbosity;

    int error = LOG_VERBOSITY::GetLogVerbosity(index, &log_verbosity);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfLogVerbosities!");
    }

    return log_verbosity;
    }, R"pbdoc(
       Get the identity of each defined standard LogVerbosity.

       Returns:
           LogVerbosity: log_verbosity
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_log_verbosities", []() {
    int number_of_log_verbosities;

    LOG_VERBOSITY::GetNumberOfLogVerbosities(&number_of_log_verbosities);

    return number_of_log_verbosities;
    }, R"pbdoc(
       Get the number of standard LogVerbosity.

       Returns:
           int: number_of_log_verbosities
       )pbdoc"
    );

  // attributes

  module.attr("silent") = LOG_VERBOSITY::silent;
  module.attr("fatal") = LOG_VERBOSITY::fatal;
  module.attr("error") = LOG_VERBOSITY::error;
  module.attr("warning") = LOG_VERBOSITY::warning;
  module.attr("information") = LOG_VERBOSITY::information;
  module.attr("debug") = LOG_VERBOSITY::debug;

}
