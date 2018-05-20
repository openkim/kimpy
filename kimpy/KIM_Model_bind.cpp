#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
//#include <pybind11/embed.h>
//#include <pybind11/iostream.h>

#include "KIM_SimulatorHeaders.hpp"
//@ TODO replace the header with the following. (need to solve forward declaration)
//#include "KIM_Model.hpp"

namespace py = pybind11;
//using namespace py::literals;

using namespace KIM;


PYBIND11_MODULE(model, module) {
  module.doc() = "Python binding to ... ";

  // C++ class constructor and destructor are private, and as a result, simple ways
  // to construct py::init() does not exist.

  // std::unique_ptr<Model, py::nodelete> avoids calling the destructor
  // of the C++ class. In this case, it is crucial that instances are deallocated
  // on the C++ side to avoid memory leaks. Model_Destroy should do the work.
  py::class_<Model, std::unique_ptr<Model, py::nodelete>> cl (module, "Model");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init(
    [](Numbering const numbering,
      LengthUnit const requestedLengthUnit,
      EnergyUnit const requestedEnergyUnit,
      ChargeUnit const requestedChargeUnit,
      TemperatureUnit const requestedTemperatureUnit,
      TimeUnit const requestedTimeUnit,
      std::string const & modelName,
      py::array_t<int> requestedUnitsAccepted,
      py::array_t<int> error
    ) {
      Model * mo;
      auto e = error.mutable_data(0);
      int* r = requestedUnitsAccepted.mutable_data(0);
      e[0] = Model::Create(numbering, requestedLengthUnit,
          requestedEnergyUnit, requestedChargeUnit, requestedTemperatureUnit,
          requestedTimeUnit, modelName, r, &mo);
      return mo;
    }//,
/*    py::arg("Numbering"),
    py::arg("LengthUnit"),
    py::arg("EnergyUnit"),
    py::arg("ChargeUnit"),
    py::arg("TemperatureUnit"),
    py::arg("TimeUnit"),
    py::arg("modelName"),
    py::arg("requestedUnitsAccepted").noconvert(),
    py::arg("error").noconvert()
*/
//@TODO cannot add py::arg since pybind11 does not support this for py::init function
// This may cause problem when the type of provided `error` and `requestedUnitsAccepted`
// from python is not of `np.intc`, since a copy of them will be made and the
// values obtained here will not be available in python.
// We have the same problem for `ComputeArguments` as well.
// So, should ask pybind11 to support.
  ));


}

