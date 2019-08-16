// This file is generated automatically by scripts
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "KIM_Collection.hpp"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(collection, module) {
  module.doc() = "Python binding to KIM_Collection.hpp";


  // classes

  py::class_<Collection> cl (module, "Collection");
  cl.def(py::init<>())
  .def(py::init<int const>())
  .def(py::init<std::string const>())
  .def("known", &Collection::Known)
  .def(py::self == py::self)
  .def(py::self != py::self)
  .def("__repr__", &Collection::ToString);


  // functions

  module.def("get_collection",
    [](int const index) {
      Collection collection;
      int error = COLLECTION::GetCollection(index, &collection);

      py::tuple re(2);
      re[0] = collection;
      re[1] = error;
      return re;
    },
    py::arg("index"),
    "Return(Collection, error)"
  );

  module.def("get_number_of_collections",
    []() {
      int numberOfCollections;
      COLLECTION::GetNumberOfCollections(&numberOfCollections);
      return numberOfCollections;
    }
  );


  // attrributes
  module.attr("system") = COLLECTION::system;
  module.attr("user") = COLLECTION::user;
  module.attr("environmentVariable") = COLLECTION::environmentVariable;
  module.attr("currentWorkingDirectory") = COLLECTION::currentWorkingDirectory;


}

