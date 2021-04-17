#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_ComputeArgumentName.hpp"
// Forward declaration in KIM_ComputeArgumentName.hpp
#include "KIM_DataType.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace KIM;


PYBIND11_MODULE(compute_argument_name, module)
{
  module.doc() = "Python binding to KIM_ComputeCallbackName.hpp";

  // classes

  py::class_<ComputeArgumentName> cl(module, "ComputeArgumentName");

  cl.def(py::init<>())
    .def(py::init<int const>())
    .def(py::init<std::string const>())
    .def("known", &ComputeArgumentName::Known,
         "Determines if the object is a quantity known to the KIM-API.")
    .def(py::self == py::self)
    .def(py::self != py::self)
    .def("__repr__", &ComputeArgumentName::ToString)
    .def(py::pickle(
      // __getstate__
      [](ComputeArgumentName const &compute_argument_name) {
        // Return a tuple that fully encodes
        // the state of the compute_argument_name object
        return py::make_tuple(compute_argument_name.computeArgumentNameID);
      },
      // __setstate__
      [](py::tuple t) {
        if (t.size() != 1) {
          throw std::runtime_error("Invalid state!");
        }
        // Create a new instance
        ComputeArgumentName compute_argument_name(t[0].cast<int>());
        return compute_argument_name;
      }
    ));

  // functions

  module.def("get_compute_argument_name", [](int const index) {
    ComputeArgumentName compute_argument_name;

    int error = COMPUTE_ARGUMENT_NAME::GetComputeArgumentName(
      index, &compute_argument_name);
    if (error == 1) {
      throw std::runtime_error(
        "index < 0 or index >= number_of_compute_argument_names!");
    }

    return compute_argument_name;
  }, "Get the identity of each defined standard compute_argument_name.",
     py::arg("index"),
     "Return compute_argument_name");

  module.def("get_number_of_compute_argument_names", []() {
    int number_of_compute_arguments;

    COMPUTE_ARGUMENT_NAME::GetNumberOfComputeArgumentNames(
        &number_of_compute_arguments);

    return number_of_compute_arguments;
  }, "Get the number of standard compute_argument_name's "
     "defined by the KIM-API.",
     "Return number_of_compute_arguments");

  module.def("get_compute_argument_data_type",
      [](ComputeArgumentName const &compute_argument_name) {
    DataType data_type;

    int error = COMPUTE_ARGUMENT_NAME::GetComputeArgumentDataType(
      compute_argument_name, &data_type);
    if (error == 1) {
      throw std::runtime_error("compute_argument_name is unknown!");
    }

    return data_type;
  }, "Get the data_type of each defined standard compute_argument_name.",
     py::arg("compute_argument_name"),
     "Return data_type");

  // attrributes

  module.attr("numberOfParticles") = COMPUTE_ARGUMENT_NAME::numberOfParticles;
  module.attr("particleSpeciesCodes") =
    COMPUTE_ARGUMENT_NAME::particleSpeciesCodes;
  module.attr("particleContributing") =
    COMPUTE_ARGUMENT_NAME::particleContributing;
  module.attr("coordinates") = COMPUTE_ARGUMENT_NAME::coordinates;
  module.attr("partialEnergy") = COMPUTE_ARGUMENT_NAME::partialEnergy;
  module.attr("partialForces") = COMPUTE_ARGUMENT_NAME::partialForces;
  module.attr("partialParticleEnergy") =
    COMPUTE_ARGUMENT_NAME::partialParticleEnergy;
  module.attr("partialVirial") = COMPUTE_ARGUMENT_NAME::partialVirial;
  module.attr("partialParticleVirial") =
    COMPUTE_ARGUMENT_NAME::partialParticleVirial;
}
