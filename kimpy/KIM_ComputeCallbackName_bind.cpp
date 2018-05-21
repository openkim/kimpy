#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/embed.h>
#include "KIM_ComputeCallbackName.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace KIM;


PYBIND11_MODULE(compute_callback_name, module) {
  module.doc() = "Python binding to ... ";


  // classes

  py::class_<ComputeCallbackName> cl (module, "ComputeCallbackName");
  cl.def(py::init(
    [](int const index, py::array_t<int> error) {
      ComputeCallbackName computeCallbackName;
      auto e = error.mutable_data(0);
      e[0] = COMPUTE_CALLBACK_NAME::GetComputeCallbackName(index, &computeCallbackName);
      return computeCallbackName;
    }
  ))
  .def(py::self == py::self)
  .def(py::self != py::self)
  .def("__repr__", &ComputeCallbackName::String);


  // functions

  // wrapper to call ComputeCallbackName to deal with `error`
  module.def("get_compute_callback_name",
    [](int const index) {
      auto locals = py::dict("index"_a=index);

      // embed python code
      py::exec(R"(
        from kimpy import compute_callback_name
        import numpy as np
        index = locals()['index']
        e = np.array([0], dtype='intc')
        instance = compute_callback_name.ComputeCallbackName(index, e)
        error = e[0]
      )", py::globals(), locals);

      auto computeCallbackName = locals["instance"].cast<ComputeCallbackName>();
      bool error = locals["error"].cast<bool>();

      py::tuple re(2);
      re[0] = computeCallbackName;
      re[1] = error;
      return re;
    },
    py::arg("index"),
    "Return(ComputeCallbackName, error)"
  );

  module.def("get_number_of_compute_callbacks",
    []() {
      int numberOfComputeCallbacks;
      COMPUTE_CALLBACK_NAME::GetNumberOfComputeCallbacks(&numberOfComputeCallbacks);
      return numberOfComputeCallbacks;
    }
  );


  // attrributes
  module.attr("GetNeighborList") = COMPUTE_CALLBACK_NAME::GetNeighborList;
  module.attr("ProcessDEDrTerm") = COMPUTE_CALLBACK_NAME:: ProcessDEDrTerm;
  module.attr("ProcessD2EDr2Term") = COMPUTE_CALLBACK_NAME:: ProcessD2EDr2Term;

}

