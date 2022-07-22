// This file is generated automatically by generate_CollectionItemType_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_CollectionItemType.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(collection_item_type, module)
{
  module.doc() = "Python binding to KIM_CollectionItemType.hpp";

  // classes

  py::class_<CollectionItemType> cl(module, "CollectionItemType");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &CollectionItemType::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &CollectionItemType::ToString);

  // functions

  module.def("get_collection_item_type", [](int const index) {
    CollectionItemType collection_item_type;

    int error = COLLECTION_ITEM_TYPE::GetCollectionItemType(index, &collection_item_type);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfCollectionItemTypes!");
    }

    return collection_item_type;
    }, R"pbdoc(
       Get the identity of each defined standard CollectionItemType.

       Returns:
           CollectionItemType: collection_item_type
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_collection_item_types", []() {
    int number_of_collection_item_types;

    COLLECTION_ITEM_TYPE::GetNumberOfCollectionItemTypes(&number_of_collection_item_types);

    return number_of_collection_item_types;
    }, R"pbdoc(
       Get the number of standard CollectionItemType.

       Returns:
           int: number_of_collection_item_types
       )pbdoc"
    );

  // attributes

  module.attr("modelDriver") = COLLECTION_ITEM_TYPE::modelDriver;
  module.attr("portableModel") = COLLECTION_ITEM_TYPE::portableModel;
  module.attr("simulatorModel") = COLLECTION_ITEM_TYPE::simulatorModel;

}
