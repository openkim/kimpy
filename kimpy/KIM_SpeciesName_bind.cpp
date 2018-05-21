#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/embed.h>
#include "KIM_SpeciesName.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace KIM;


PYBIND11_MODULE(species_name, module) {
  module.doc() = "Python binding to ... ";


  // classes

  py::class_<SpeciesName> cl (module, "SpeciesName");
  cl.def(py::init(
    [](int const index, py::array_t<int> error) {
      SpeciesName speciesName;
      auto e = error.mutable_data(0);
      e[0] = SPECIES_NAME::GetSpeciesName(index, &speciesName);
      return speciesName;
    }
  ))
  .def(py::self == py::self)
  .def(py::self != py::self)
  .def("__repr__", &SpeciesName::String);


  // functions

  // wrapper to call SpeciesName to deal with `error`
  module.def("get_species_name",
    [](int const index) {
      auto locals = py::dict("index"_a=index);

      // embed python code
      py::exec(R"(
        from kimpy import species_name
        import numpy as np
        index = locals()['index']
        e = np.array([0], dtype='intc')
        instance = species_name.SpeciesName(index, e)
        error = e[0]
      )", py::globals(), locals);

      auto speciesName = locals["instance"].cast<SpeciesName>();
      bool error = locals["error"].cast<bool>();

      py::tuple re(2);
      re[0] = speciesName;
      re[1] = error;
      return re;
    },
    py::arg("index"),
    "Return(SpeciesName, error)"
  );

  module.def("get_number_of_species_names",
    []() {
      int numberOfSpeciesNames;
      SPECIES_NAME::GetNumberOfSpeciesNames(&numberOfSpeciesNames);
      return numberOfSpeciesNames;
    }
  );


  // attrributes
  #include "SpeciesName_attributes.cpp"
}
