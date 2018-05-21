#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/embed.h>
#include "KIM_SupportStatus.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace KIM;


PYBIND11_MODULE(support_status, module) {
  module.doc() = "Python binding to ... ";


  // classes

  py::class_<SupportStatus> cl (module, "SupportStatus");
  cl.def(py::init(
    [](int const index, py::array_t<int> error) {
      SupportStatus supportStatus;
      auto e = error.mutable_data(0);
      e[0] = SUPPORT_STATUS::GetSupportStatus(index, &supportStatus);
      return supportStatus;
    }
  ))
  .def(py::self == py::self)
  .def(py::self != py::self)
  .def("__repr__", &SupportStatus::String);


  // functions

  // wrapper to call SupportStatus to deal with `error`
  module.def("get_support_status",
    [](int const index) {
      auto locals = py::dict("index"_a=index);

      // embed python code
      py::exec(R"(
        from kimpy import support_status
        import numpy as np
        index = locals()['index']
        e = np.array([0], dtype='intc')
        instance = support_status.SupportStatus(index, e)
        error = e[0]
      )", py::globals(), locals);

      auto supportStatus = locals["instance"].cast<SupportStatus>();
      bool error = locals["error"].cast<bool>();

      py::tuple re(2);
      re[0] = supportStatus;
      re[1] = error;
      return re;
    },
    py::arg("index"),
    "Return(SupportStatus, error)"
  );

  module.def("get_number_of_support_statuses",
    []() {
      int numberOfSupportStatuses;
      SUPPORT_STATUS::GetNumberOfSupportStatuses(&numberOfSupportStatuses);
      return numberOfSupportStatuses;
    }
  );


  // attrributes
  module.attr("requiredByAPI") = SUPPORT_STATUS::requiredByAPI;
  module.attr("notSupported") = SUPPORT_STATUS::notSupported;
  module.attr("required") = SUPPORT_STATUS::required;
  module.attr("optional") = SUPPORT_STATUS::optional;

}

