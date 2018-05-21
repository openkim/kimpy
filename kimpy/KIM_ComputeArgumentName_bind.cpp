#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/embed.h>
#include "KIM_ComputeArgumentName.hpp"
#include "KIM_DataType.hpp"  // Forward declaration in KIM_ComputeArgumentName.hpp

namespace py = pybind11;
using namespace py::literals;
using namespace KIM;


PYBIND11_MODULE(compute_argument_name, module) {
  module.doc() = "Python binding to ... ";


  // classes

  py::class_<ComputeArgumentName> cl (module, "get_compute_argument_name_original");
  cl.def(py::init(
    [](int const index, py::array_t<int> error) {
      ComputeArgumentName computeArgumentName;
      auto e = error.mutable_data(0);
      e[0] = COMPUTE_ARGUMENT_NAME::GetComputeArgumentName(index, &computeArgumentName);
      return computeArgumentName;
    }
  ))
  .def(py::self == py::self)
  .def(py::self != py::self)
  .def("__repr__", &ComputeArgumentName::String);


  // functions

  // wrapper to call get_compute_argument_name_original to deal with `error`
  module.def("get_compute_argument_name",
    [](int const index) {
      auto locals = py::dict("index"_a=index);

      // embed python code
      py::exec(R"(
        from kimpy import compute_argument_name
        import numpy as np
        index = locals()['index']
        e = np.array([0], dtype='intc')
        name = compute_argument_name.get_compute_argument_name_original(index, e)
        error = e[0]
      )", py::globals(), locals);

      auto computeArgumentName = locals["name"].cast<ComputeArgumentName>();
      bool error = locals["error"].cast<bool>();

      py::tuple re(2);
      re[0] = computeArgumentName;
      re[1] = error;
      return re;
    },
    py::arg("index"),
    "Return(ComputeArgumentName, error)"
  );

  module.def("get_number_of_compute_arguments",
    []() {
      int numberOfComputeArguments;
      COMPUTE_ARGUMENT_NAME::GetNumberOfComputeArguments(&numberOfComputeArguments);
      return numberOfComputeArguments;
    }
  );

  module.def("get_compute_argument_data_type",
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
    "Return(DataType, error)"
  );


  // attrributes
  module.attr("numberOfParticles") = COMPUTE_ARGUMENT_NAME::numberOfParticles;
  module.attr("particleSpeciesCodes") = COMPUTE_ARGUMENT_NAME::particleSpeciesCodes;
  module.attr("particleContributing") = COMPUTE_ARGUMENT_NAME::particleContributing;
  module.attr("coordinates") = COMPUTE_ARGUMENT_NAME::coordinates;
  module.attr("partialEnergy") = COMPUTE_ARGUMENT_NAME::partialEnergy;
  module.attr("partialForces") = COMPUTE_ARGUMENT_NAME::partialForces;
  module.attr("partialParticleEnergy") = COMPUTE_ARGUMENT_NAME::partialParticleEnergy;
  module.attr("partialVirial") = COMPUTE_ARGUMENT_NAME::partialVirial;
  module.attr("partialParticleVirial") = COMPUTE_ARGUMENT_NAME::partialParticleVirial;

}

