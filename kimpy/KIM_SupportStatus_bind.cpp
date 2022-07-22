// This file is generated automatically by generate_SupportStatus_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_SupportStatus.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(support_status, module)
{
  module.doc() = "Python binding to KIM_SupportStatus.hpp";

  // classes

  py::class_<SupportStatus> cl(module, "SupportStatus");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &SupportStatus::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &SupportStatus::ToString);

  // functions

  module.def("get_support_status", [](int const index) {
    SupportStatus support_status;

    int error = SUPPORT_STATUS::GetSupportStatus(index, &support_status);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfSupportStatuses!");
    }

    return support_status;
    }, R"pbdoc(
       Get the identity of each defined standard SupportStatus.

       Returns:
           SupportStatus: support_status
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_support_statuses", []() {
    int number_of_support_statuses;

    SUPPORT_STATUS::GetNumberOfSupportStatuses(&number_of_support_statuses);

    return number_of_support_statuses;
    }, R"pbdoc(
       Get the number of standard SupportStatus.

       Returns:
           int: number_of_support_statuses
       )pbdoc"
    );

  // attributes

  module.attr("requiredByAPI") = SUPPORT_STATUS::requiredByAPI;
  module.attr("notSupported") = SUPPORT_STATUS::notSupported;
  module.attr("required") = SUPPORT_STATUS::required;
  module.attr("optional") = SUPPORT_STATUS::optional;

}
