#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
//#include <pybind11/embed.h>
//#include <pybind11/iostream.h>

#include "KIM_SimulatorHeaders.hpp"
//@ TODO replace the header with the following. (need to solve forward declaration)
//#include "KIM_ComputeArguments.hpp"

namespace py = pybind11;
//using namespace py::literals;

using namespace KIM;


PYBIND11_MODULE(compute_arguments, module) {
  module.doc() = "Python binding to ... ";

  // C++ class constructor and destructor are private, and as a result, simple ways
  // to construct py::init() does not exist.

  // std::unique_ptr<ComputeArguments, py::nodelete> avoids calling the destructor
  // of the C++ class. In this case, it is crucial that instances are deallocated
  // on the C++ side to avoid memory leaks. ComputeArguments_Destroy should do the
  // work.
  py::class_<ComputeArguments, std::unique_ptr<ComputeArguments, py::nodelete>>
      cl (module, "ComputeArguments");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init(
    [](py::array_t<int> error, Model* mo) {
      ComputeArguments * ca;
      auto e = error.mutable_data(0);
      e[0] = mo->ComputeArgumentsCreate(&ca);
      return ca;
    }//,
//    py::arg("error").noconvert(),
//    py::arg("Model")
  ));



}

