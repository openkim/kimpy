// This file is generated automatically by generate_ComputeCallbackName_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_ComputeCallbackName.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(compute_callback_name, module)
{
  module.doc() = "Python binding to KIM_ComputeCallbackName.hpp";

  // classes

  py::class_<ComputeCallbackName> cl(module, "ComputeCallbackName");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &ComputeCallbackName::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &ComputeCallbackName::ToString);

  // functions

  module.def("get_compute_callback_name", [](int const index) {
    ComputeCallbackName compute_callback_name;

    int error = COMPUTE_CALLBACK_NAME::GetComputeCallbackName(index, &compute_callback_name);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfComputeCallbackNames!");
    }

    return compute_callback_name;
    }, R"pbdoc(
       Get the identity of each defined standard ComputeCallbackName.

       Returns:
           ComputeCallbackName: compute_callback_name
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_compute_callback_names", []() {
    int number_of_compute_callback_names;

    COMPUTE_CALLBACK_NAME::GetNumberOfComputeCallbackNames(&number_of_compute_callback_names);

    return number_of_compute_callback_names;
    }, R"pbdoc(
       Get the number of standard ComputeCallbackName.

       Returns:
           int: number_of_compute_callback_names
       )pbdoc"
    );

  // attributes

  module.attr("GetNeighborList") = COMPUTE_CALLBACK_NAME::GetNeighborList;
  module.attr("ProcessDEDrTerm") = COMPUTE_CALLBACK_NAME::ProcessDEDrTerm;
  module.attr("ProcessD2EDr2Term") = COMPUTE_CALLBACK_NAME::ProcessD2EDr2Term;

}
