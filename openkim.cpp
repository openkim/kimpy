#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <iostream>
#include <KIM_API_C.h>


namespace py = pybind11;


void triple(double* in, double* out) {
  for (int i=0; i<3; i++) {
      out[i] = in[i]*0.1;
    }
}



PYBIND11_PLUGIN(openkim) {
  py::module m("openkim", "ASE calculator based on OpenKIM");

  m.def("triple",
    [](py::array_t<double, py::array::c_style> x) {
      // proxy of x; not check bounds
      auto in = x.mutable_unchecked<1>();

      py::array_t<double, py::array::c_style> out({in.shape(0)});

      triple(in.mutable_data(0), out.mutable_data(0));
      return out;
    },
    py::arg("x")
  );



  return m.ptr();
}
