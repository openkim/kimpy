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


PYBIND11_MODULE(numbering, module) {
  module.doc() = "Python binding to ... ";


  // classes

  py::class_<Numbering> cl (module, "get_numbering_original");
  cl.def(py::init(
    [](int const index, py::array_t<int> error) {
      Numbering numbering_;
      auto e = error.mutable_data(0);
      e[0] = NUMBERING::GetNumbering(index, &numbering_);
      return numbering_;
    }
  ))
  .def(py::self == py::self)
  .def(py::self != py::self)
  .def("__repr__", &Numbering::String);


  // functions

  // wrapper to call get_numbering_original to deal with `error`
  module.def("get_numbering",
    [](int const index) {
      auto locals = py::dict("index"_a=index);

      // embed python code
      py::exec(R"(
        from kimpy import numbering
        import numpy as np
        index = locals()['index']
        e = np.array([0], dtype='intc')
        numbering_ = numbering.get_numbering_original(index, e)
        error = e[0]
      )", py::globals(), locals);

      auto numbering_ = locals["numbering_"].cast<Numbering>();
      bool error = locals["error"].cast<bool>();

      py::tuple re(2);
      re[0] = numbering_;
      re[1] = error;
      return re;
    },
    py::arg("index"),
    "Return(Numbering, error)"
  );

  module.def("get_number_of_numberings",
    []() {
      int numberOfNumberings;
      NUMBERING::GetNumberOfNumberings(&numberOfNumberings);
      return numberOfNumberings;
    }
  );


  // attrributes
  module.attr("zeroBased") = NUMBERING::zeroBased;
  module.attr("oneBased") = NUMBERING::oneBased;

}

