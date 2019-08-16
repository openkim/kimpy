// This file is generated automatically by scripts
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "KIM_CollectionItemType.hpp"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(collection_item_type, module) {
  module.doc() = "Python binding to KIM_CollectionItemType.hpp";


  // classes

  py::class_<CollectionItemType> cl (module, "CollectionItemType");
  cl.def(py::init<>())
  .def(py::init<int const>())
  .def(py::init<std::string const>())
  .def("known", &CollectionItemType::Known)
  .def(py::self == py::self)
  .def(py::self != py::self)
  .def("__repr__", &CollectionItemType::ToString);


  // functions

  module.def("get_collection_item_type",
    [](int const index) {
      CollectionItemType collectionItemType;
      int error = COLLECTION_ITEM_TYPE::GetCollectionItemType(index, &collectionItemType);

      py::tuple re(2);
      re[0] = collectionItemType;
      re[1] = error;
      return re;
    },
    py::arg("index"),
    "Return(CollectionItemType, error)"
  );

  module.def("get_number_of_collection_item_types",
    []() {
      int numberOfCollectionItemTypes;
      COLLECTION_ITEM_TYPE::GetNumberOfCollectionItemTypes(&numberOfCollectionItemTypes);
      return numberOfCollectionItemTypes;
    }
  );


  // attrributes
  module.attr("modelDriver") = COLLECTION_ITEM_TYPE::modelDriver;
  module.attr("portableModel") = COLLECTION_ITEM_TYPE::portableModel;
  module.attr("simulatorModel") = COLLECTION_ITEM_TYPE::simulatorModel;


}

