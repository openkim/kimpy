#include "KIM_ComputeArgumentName.hpp"
#include "KIM_DataType.hpp"  // Forward declaration in KIM_ComputeArgumentName.hpp
#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

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
      .def("known", &ComputeArgumentName::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &ComputeArgumentName::ToString);


  // functions

  module.def(
      "get_compute_argument_name",
      [](int const index) {
        ComputeArgumentName computeArgumentName;
        int error = COMPUTE_ARGUMENT_NAME::GetComputeArgumentName(
            index, &computeArgumentName);

        py::tuple re(2);
        re[0] = computeArgumentName;
        re[1] = error;
        return re;
      },
      py::arg("index"),
      "Return(ComputeArgumentName, error)");

  module.def("get_number_of_compute_argument_names", []() {
    int numberOfComputeArguments;
    COMPUTE_ARGUMENT_NAME::GetNumberOfComputeArgumentNames(
        &numberOfComputeArguments);
    return numberOfComputeArguments;
  });

  module.def(
      "get_compute_argument_data_type",
      [](ComputeArgumentName const computeArgumentName) {
        DataType dataType;
        bool error = COMPUTE_ARGUMENT_NAME::GetComputeArgumentDataType(
            computeArgumentName, &dataType);

        py::tuple re(2);
        re[0] = dataType;
        re[1] = error;
        return re;
      },
      py::arg("ComputeArgumentName"),
      "Return(DataType, error)");


  // attrributes
  module.attr("numberOfParticles") = COMPUTE_ARGUMENT_NAME::numberOfParticles;
  module.attr("particleSpeciesCodes")
      = COMPUTE_ARGUMENT_NAME::particleSpeciesCodes;
  module.attr("particleContributing")
      = COMPUTE_ARGUMENT_NAME::particleContributing;
  module.attr("coordinates") = COMPUTE_ARGUMENT_NAME::coordinates;
  module.attr("partialEnergy") = COMPUTE_ARGUMENT_NAME::partialEnergy;
  module.attr("partialForces") = COMPUTE_ARGUMENT_NAME::partialForces;
  module.attr("partialParticleEnergy")
      = COMPUTE_ARGUMENT_NAME::partialParticleEnergy;
  module.attr("partialVirial") = COMPUTE_ARGUMENT_NAME::partialVirial;
  module.attr("partialParticleVirial")
      = COMPUTE_ARGUMENT_NAME::partialParticleVirial;
}
