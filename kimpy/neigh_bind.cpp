#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>
#include "neigh.h"
#include "padding.h"

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
    [](void* kimmdl, double cutoff, py::array_t<int> is_padding, bool padding_need_neigh) {
      int need_neigh = 0;
      if (padding_need_neigh) {
        need_neigh = 1;
      }
      int status = nbl_build_neighborlist(kimmdl, cutoff, is_padding.data(0), need_neigh);
      return status;
    },
    py::arg("kimmdl"),
    py::arg("cutoff"),
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

  m.def("set_padding",
    [](py::array_t<double> cell, py::array_t<int> PBC, double cutoff,
        py::array_t<double> coords, py::array_t<int> species) {

      std::vector<double> pad_coords;
      std::vector<int> pad_species;
      std::vector<int> pad_image;

      int Natoms = species.size();
      // call implementation
      set_padding(cell.mutable_data(), PBC.mutable_data(), cutoff, Natoms,
          coords.data(), species.data(), pad_coords, pad_species, pad_image);

      // number of padding atoms
      auto Npad = pad_species.size();

      // pack pad_coords into a buffer that numpy array can understand
      auto pad_coords_array = py::array (py::buffer_info (
        pad_coords.data(),   // data pointer
        sizeof(double),  // size of one element
        py::format_descriptor<double>::format(), //Python struct-style format descriptor
        1,  // dimension
        {Npad*3},  // size of each dimension
        {sizeof(double)}  // stride of each dimension
      ));

      // pack pad_species into a buffer that numpy array can understand
      auto pad_species_array = py::array (py::buffer_info (
        pad_species.data(),
        sizeof(int),
        py::format_descriptor<int>::format(),
        1,
        {Npad},
        {sizeof(int)}
      ));

      // pack pad_image into a buffer that numpy array can understand
      auto pad_image_array = py::array (py::buffer_info (
        pad_image.data(),
        sizeof(int),
        py::format_descriptor<int>::format(),
        1,
        {Npad},
        {sizeof(int)}
      ));

      // return
      py::tuple t(3);
      t[0] = pad_coords_array;
      t[1] = pad_species_array;
      t[2] = pad_image_array;

      return t;
    },
    py::arg("cell"),
    py::arg("PBC"),
    py::arg("cutoff"),
    py::arg("coords"),
    py::arg("species_code"),
    "Return (pad_coords, pad_species_code, pad_image)"
  );


}

