// This file is generated automatically by generate_Collection_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_Collection.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(collection, module)
{
  module.doc() = "Python binding to KIM_Collection.hpp";

  // classes

  py::class_<Collection> cl(module, "Collection");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &Collection::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &Collection::ToString);

  // functions

  module.def("get_collection", [](int const index) {
    Collection collection;

    int error = COLLECTION::GetCollection(index, &collection);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfCollections!");
    }

    return collection;
    }, R"pbdoc(
       Get the identity of each defined standard Collection.

       Returns:
           Collection: collection
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_collections", []() {
    int number_of_collections;

    COLLECTION::GetNumberOfCollections(&number_of_collections);

    return number_of_collections;
    }, R"pbdoc(
       Get the number of standard Collection.

       Returns:
           int: number_of_collections
       )pbdoc"
    );

  // attributes

  module.attr("system") = COLLECTION::system;
  module.attr("user") = COLLECTION::user;
  module.attr("environmentVariable") = COLLECTION::environmentVariable;
  module.attr("currentWorkingDirectory") = COLLECTION::currentWorkingDirectory;

}
