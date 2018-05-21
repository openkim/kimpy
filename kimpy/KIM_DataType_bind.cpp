#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/embed.h>

#include "KIM_SimulatorHeaders.hpp"
//@ TODO replace the header with the following. (need to solve forward declaration)
//#include "KIM_Model.hpp"

namespace py = pybind11;
using namespace py::literals;

using namespace KIM;


PYBIND11_MODULE(data_type, module) {
  module.doc() = "Python binding to ... ";


  // classes

  py::class_<DataType> cl (module, "get_data_type_original");
  cl.def(py::init(
    [](int const index, py::array_t<int> error) {
      DataType dataType;
      auto e = error.mutable_data(0);
      e[0] = DATA_TYPE::GetDataType(index, &dataType);
      return dataType;
    }
  ))
  .def(py::self == py::self)
  .def(py::self != py::self)
  .def("__repr__", &DataType::String);


  // functions

  // wrapper to call get_data_type_original to deal with `error`
  module.def("get_data_type",
    [](int const index) {
      auto locals = py::dict("index"_a=index);

      // embed python code
      py::exec(R"(
        from kimpy import data_type
        import numpy as np
        index = locals()['index']
        e = np.array([0], dtype='intc')
        data_type = data_type.get_data_type_original(index, e)
        error = e[0]
      )", py::globals(), locals);

      auto dataType = locals["data_type"].cast<DataType>();
      bool error = locals["error"].cast<bool>();

      py::tuple re(2);
      re[0] = dataType;
      re[1] = error;
      return re;
    },
    py::arg("index"),
    "Return(DataType, error)"
  );

  module.def("get_number_of_data_types",
    []() {
      int numberOfDataTypes;
      DATA_TYPE::GetNumberOfDataTypes(&numberOfDataTypes);
      return numberOfDataTypes;
    }
  );


  // attrributes
  module.attr("Integer") = DATA_TYPE::Integer;
  module.attr("Double") = DATA_TYPE::Double;

}

