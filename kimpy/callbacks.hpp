#include <pybind11/pybind11.h>

namespace py = pybind11;


// get_neigh wrapper
// The corresponding python get_neigh function should be defined as:
//
// def get_neigh(data, cutoffs, neighbor_list_index, particle_number):
//   ...
//   return (neighbors, error)
//
//
// Paramters:
//
// data: dictionary, registered when calling `set_callback()`
// cutoffs: numpy array
// neighbor_list_index: int
// particle_number: int
//
// Returns:
//  neighbors: one-dimensional numpy array
//    neighbors of particle
//  error: int
//    error code
//
int get_neigh(void const * const data_object,
              int const number_of_cutoffs,
              double const * const cutoffs,
              int const neighbor_list_index,
              int const particle_number,
              int * const number_of_neighbors,
              int const ** const neighbors_of_particle)
{
  // get python callback function and data
  auto d1 = const_cast<void *>(data_object);
  py::dict d = *(reinterpret_cast<py::dict *>(d1));
  py::object pyfunc = d["pyfunc"];
  py::dict pydata = d["pydata"];

  // pack cutoffs as py::array
  py::array_t<double, py::array::c_style> pycutoffs(number_of_cutoffs);
  auto p_pycutoffs = pycutoffs.mutable_data(0);
  for (int i = 0; i < number_of_cutoffs; i++) { p_pycutoffs[i] = cutoffs[i]; }

  // call python get_neigh function
  py::tuple out
      = pyfunc(pydata, pycutoffs, neighbor_list_index, particle_number);
  auto neigh = static_cast<py::array_t<int> >(out[0]);
  auto error = out[1].cast<int>();
  *neighbors_of_particle = neigh.data();
  *number_of_neighbors = static_cast<int>(neigh.size());

  return error;
}


// process_dEdr wrapper
// The corresponding python process_dEdr function should be defined as:
//
// def process_dEdr(data, de, r, dx, i, j):
//   ...
//   return error
//
// Paramters:
//
// data: dictionary, registered when calling `set_callback()`
// de: double
// r: dobule
// dx: numpy array of shape (3,)
// i: int
// j: int
//
// Returns:
//  error: int
//    error code
//
int process_dEdr(void const * const data_object,
                 double const de,
                 double const r,
                 double const * const dx,
                 int const i,
                 int const j)
{
  // get python callback function and data
  auto d1 = const_cast<void *>(data_object);
  py::dict d = *(reinterpret_cast<py::dict *>(d1));
  py::object pyfunc = d["pyfunc"];
  py::dict pydata = d["pydata"];

  // pack dx as py::array
  py::array_t<double, py::array::c_style> pydx(3);
  auto p_pydx = pydx.mutable_data(0);
  for (int ii = 0; ii < 3; ii++) { p_pydx[ii] = dx[ii]; }

  // call python process_dEdr function
  auto out = pyfunc(pydata, de, r, pydx, i, j);
  auto error = out.cast<int>();

  return error;
}


// process_dEdr wrapper
// The corresponding python process_dEdr function should be defined as:
//
// def process_dEdr(data, de, r, dx, i, j):
//   ...
//   return error
//
// Paramters:
//
// data: dictionary, registered when calling `set_callback()`
// de: double
// r: numpy array of shape (2,)
// dx: numpy array of shape (6,)
// i: numpy array of shape (2,)
// j: numpy array of shape (2,)
//
// Returns:
//  error: int
//    error code
//
int process_d2Edr2(void const * const data_object,
                   double const de,
                   double const * const r,
                   double const * const dx,
                   int const * const i,
                   int const * const j)
{
  // get python callback function and data
  auto d1 = const_cast<void *>(data_object);
  py::dict d = *(reinterpret_cast<py::dict *>(d1));
  py::object pyfunc = d["pyfunc"];
  py::dict pydata = d["pydata"];

  // pack data as py::array
  py::array_t<double, py::array::c_style> pyr(2);
  py::array_t<int, py::array::c_style> pyi(2);
  py::array_t<int, py::array::c_style> pyj(2);
  auto p_pyr = pyr.mutable_data(0);
  auto p_pyi = pyi.mutable_data(0);
  auto p_pyj = pyj.mutable_data(0);
  for (int ii = 0; ii < 2; ii++)
  {
    p_pyr[ii] = r[ii];
    p_pyi[ii] = i[ii];
    p_pyj[ii] = j[ii];
  }
  py::array_t<double, py::array::c_style> pydx(6);
  auto p_pydx = pydx.mutable_data(0);
  for (int ii = 0; ii < 6; ii++) { p_pydx[ii] = dx[ii]; }

  // call python process_dEdr function
  auto out = pyfunc(pydata, de, pyr, pydx, pyi, pyj);
  auto error = out.cast<int>();

  return error;
}
