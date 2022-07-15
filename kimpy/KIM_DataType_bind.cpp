// This file is generated automatically by generate_DataType_bind_test.py.
// Do not modify this file, but modify the script instead.
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <string>

#include "KIM_DataType.hpp"

namespace py = pybind11;
using namespace KIM;

PYBIND11_MODULE(data_type, module)
{
  module.doc() = "Python binding to KIM_DataType.hpp";

  // classes

  py::class_<DataType> cl(module, "DataType");

  cl.def(py::init<>())
      .def(py::init<int const>())
      .def(py::init<std::string const>())
      .def("known", &DataType::Known)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("__repr__", &DataType::ToString);

  // functions

  module.def("get_data_type", [](int const index) {
    DataType data_type;

    int error = DATA_TYPE::GetDataType(index, &data_type);
    if (error == 1)
    {
      throw std::runtime_error("index < 0 or index >= numberOfDataTypes!");
    }

    return data_type;
    }, R"pbdoc(
       Get the identity of each defined standard DataType.

       Returns:
           DataType: data_type
       )pbdoc",
       py::arg("index"));

  module.def("get_number_of_data_types", []() {
    int number_of_data_types;

    DATA_TYPE::GetNumberOfDataTypes(&number_of_data_types);

    return number_of_data_types;
    }, R"pbdoc(
       Get the number of standard DataType.

       Returns:
           int: number_of_data_types
       )pbdoc"
    );

  // attributes

  module.attr("Integer") = DATA_TYPE::Integer;
  module.attr("Double") = DATA_TYPE::Double;

}
