#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "neigh.h"

namespace py = pybind11;


PYBIND11_MODULE(neighborlist, m) {
  m.doc() = "KIM API neighborlist utility";

  m.def("initialize",
    [](void* kimmdl) {
      int status =  nbl_initialize(kimmdl);
      return status;
    },
    py::arg("kimmdl")
  );

  m.def("build_neighborlist",
    [](void* kimmdl, py::array_t<int> is_padding, bool padding_need_neigh) {
      int need_neigh = 0;
      if (padding_need_neigh) {
        need_neigh = 1;
      }
      int status = nbl_build_neighborlist(kimmdl, is_padding.data(0), need_neigh);
      return status;
    },
    py::arg("kimmdl"),
    py::arg("is_padding").noconvert(),
    py::arg("padding_need_neigh")
  );

   m.def("clean",
    [](void* kimmdl) {
      int status = nbl_clean(kimmdl);
      return status;
    },
    py::arg("kimmdl")
  );


}

