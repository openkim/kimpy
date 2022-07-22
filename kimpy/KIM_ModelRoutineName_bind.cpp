// This file is generated automatically by generate_ModelRoutineName_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_ModelRoutineName.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(model_routine_name, module)
{
  module.doc() = "Python binding to KIM_ModelRoutineName.hpp";

  // classes

  py::class_<ModelRoutineName> cl(module, "ModelRoutineName");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &ModelRoutineName::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &ModelRoutineName::ToString);

  // functions

  module.def("get_model_routine_name", [](int const index) {
    ModelRoutineName model_routine_name;

    int error = MODEL_ROUTINE_NAME::GetModelRoutineName(index, &model_routine_name);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfModelRoutineNames!");
    }

    return model_routine_name;
    }, R"pbdoc(
       Get the identity of each defined standard ModelRoutineName.

       Returns:
           ModelRoutineName: model_routine_name
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_model_routine_names", []() {
    int number_of_model_routine_names;

    MODEL_ROUTINE_NAME::GetNumberOfModelRoutineNames(&number_of_model_routine_names);

    return number_of_model_routine_names;
    }, R"pbdoc(
       Get the number of standard ModelRoutineName.

       Returns:
           int: number_of_model_routine_names
       )pbdoc"
    );

  // attributes

  module.attr("Create") = MODEL_ROUTINE_NAME::Create;
  module.attr("ComputeArgumentsCreate") = MODEL_ROUTINE_NAME::ComputeArgumentsCreate;
  module.attr("Compute") = MODEL_ROUTINE_NAME::Compute;
  module.attr("Extension") = MODEL_ROUTINE_NAME::Extension;
  module.attr("Refresh") = MODEL_ROUTINE_NAME::Refresh;
  module.attr("WriteParameterizedModel") = MODEL_ROUTINE_NAME::WriteParameterizedModel;
  module.attr("ComputeArgumentsDestroy") = MODEL_ROUTINE_NAME::ComputeArgumentsDestroy;
  module.attr("Destroy") = MODEL_ROUTINE_NAME::Destroy;

}
