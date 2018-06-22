#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/embed.h>

#include "KIM_SimulatorHeaders.hpp"

namespace py = pybind11;
using namespace pybind11::literals;
using namespace KIM;


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
int get_neigh(void const * const dataObject, int const numberOfCutoffs,
    double const * const cutoffs, int const neighborListIndex,
    int const particleNumber, int * const numberOfNeighbors,
    int const ** const neighborsOfParticle)
{

  // get python callback function and data
  auto d1 = const_cast<void *> (dataObject);
  py::dict d = *(reinterpret_cast<py::dict *> (d1));
  py::object pyfunc = d["pyfunc"];
  py::dict pydata = d["pydata"];

  // pack cutoffs as py::array
   py::array_t<double, py::array::c_style> pycutoffs (
       {static_cast<size_t> (numberOfCutoffs)});
   auto p_pycutoffs = pycutoffs.mutable_data(0);
   for(int i=0; i<numberOfCutoffs; i++) {
     p_pycutoffs[i] = cutoffs[i];
   }

  // call python get_neigh function
  py::tuple out = pyfunc(pydata, pycutoffs, neighborListIndex, particleNumber);
  auto neigh = static_cast<py::array_t<int>> (out[0]);
  auto error = out[1].cast<int>();
  *neighborsOfParticle = neigh.data();
  *numberOfNeighbors = neigh.size();

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
int process_dEdr(void const * const dataObject, double const de, double const r,
    double const * const dx, int const i, int const j)
{

  // get python callback function and data
  auto d1 = const_cast<void *> (dataObject);
  py::dict d = *(reinterpret_cast<py::dict *> (d1));
  py::object pyfunc = d["pyfunc"];
  py::dict pydata = d["pydata"];

 // pack dx as py::array
   py::array_t<double, py::array::c_style> pydx ({3});
   auto p_pydx = pydx.mutable_data(0);
   for(int ii=0; ii<3; ii++) {
     p_pydx[ii] = dx[ii];
   }

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
int process_d2Edr2(void const * const dataObject, double const de,
    double const * const r, double const * const dx, int const * const i,
    int const * const j)
{
  // get python callback function and data
  auto d1 = const_cast<void *> (dataObject);
  py::dict d = *(reinterpret_cast<py::dict *> (d1));
  py::object pyfunc = d["pyfunc"];
  py::dict pydata = d["pydata"];

  // pack data as py::array
   py::array_t<double, py::array::c_style> pyr ({2});
   py::array_t<int, py::array::c_style> pyi ({2});
   py::array_t<int, py::array::c_style> pyj ({2});
   auto p_pyr = pyr.mutable_data(0);
   auto p_pyi = pyi.mutable_data(0);
   auto p_pyj = pyj.mutable_data(0);
   for(int ii=0; ii<2; ii++) {
     p_pyr[ii] = r[ii];
     p_pyi[ii] = i[ii];
     p_pyj[ii] = j[ii];
   }
   py::array_t<double, py::array::c_style> pydx ({6});
   auto p_pydx= pydx.mutable_data(0);
   for(int ii=0; ii<6; ii++) {
     p_pydx[ii] = dx[ii];
   }

  // call python process_dEdr function
  auto out = pyfunc(pydata, de, pyr, pydx, pyi, pyj);
  auto error = out.cast<int>();

  return error;
}



PYBIND11_MODULE(compute_arguments, module) {
  module.doc() = "Python binding to ... ";

  // C++ class constructor and destructor are private, which can not be wrapped
  // directly.
  // So we need to call the C++ class factory function in py::init, and use
  // `py::nodelete` to avoid calling the destructor when python instance is destroyed.
  // It is crucial that the instance is deallocated by calling the destroy function
  // from the C++ side to avoid memory leaks.
  // For more info, see http://pybind11.readthedocs.io/en/stable/advanced/classes.html

  py::class_<ComputeArguments, std::unique_ptr<ComputeArguments, py::nodelete>>
      cl (module, "ComputeArguments", py::dynamic_attr());

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init(
    [](Model& mo, py::array_t<int> error) {
      ComputeArguments * ca;
      auto e = error.mutable_data(0);
      e[0] = mo.ComputeArgumentsCreate(&ca);
      return ca;
    }
  ))

  .def("get_argument_support_status",
    [](ComputeArguments& self, ComputeArgumentName const computeArgumentName) {
      SupportStatus supportStatus;
      int error = self.GetArgumentSupportStatus(computeArgumentName, &supportStatus);

      py::tuple re(2);
      re[0] = supportStatus;
      re[1] = error;
      return re;
    },
    py::arg("ComputeArgumentName"),
    "Return(SupportStatus, error)"
  )

  .def("get_callback_support_status",
    [](ComputeArguments& self, ComputeCallbackName const computeCallbackName) {
      SupportStatus supportStatus;
      int error = self.GetCallbackSupportStatus(computeCallbackName, &supportStatus);

      py::tuple re(2);
      re[0] = supportStatus;
      re[1] = error;
      return re;
    },
    py::arg("ComputeCallbackName"),
    "Return(SupportStatus, error)"
  )

  .def("set_argument_pointer",
    [](ComputeArguments& self,
       ComputeArgumentName const computeArgumentName,
       py::array_t<int> ptr
    ) {
      int* data = ptr.mutable_data(0);
      int error = self.SetArgumentPointer(computeArgumentName, data);
      return error;
    },
    py::arg("ComputeArgumentName"),
    py::arg("ptr").noconvert()
  )

  .def("set_argument_pointer",
    [](ComputeArguments& self,
      ComputeArgumentName const computeArgumentName,
      py::array_t<double> ptr
    ) {
      double* data = ptr.mutable_data(0);
      int error = self.SetArgumentPointer(computeArgumentName, data);
      return error;
    },
    py::arg("ComputeArgumentName"),
    py::arg("ptr").noconvert()
  )

  .def("set_callback_pointer",
    [](ComputeArguments& self,
      ComputeCallbackName const computeCallbackName,
      void const * const fptr,  // cannpt use: KIM::func * const fptr
                                // the argument passed in is of type: void const *
                                // we cast type explicitly in the funciton body
      void const * const dataObject
    ) {
      KIM::func * new_fptr = (KIM::func*) fptr;
      int error = self.SetCallbackPointer(computeCallbackName,
        LANGUAGE_NAME::cpp, new_fptr, dataObject);
      return error;
    },
    py::arg("ComputeCallbackName"),
    py::arg("ptr"),
    py::arg("dataObject")
  )


  .def("set_callback",
    [](ComputeArguments& self,
      ComputeCallbackName const computeCallbackName,
      py::object& pyfunc,
      py::dict& dataObject
    ) {

      // pack python callback function and dataObject into a new dict
      //dataObject["function"] = pyfunc;
      py::dict * d = new py::dict("pyfunc"_a=pyfunc, "pydata"_a = dataObject);

      // Make a copy of dataObject, because it is a copy of the python dictionary,
      // with scope only in this function.
      // TODO check back to see whether pybind11 allows pass arguments by referenc
      //py::dict* d = new py::dict(dataObject);
      auto pd = reinterpret_cast<void const * const> (d);

      // add 'pd' to as attribute of class ComputeArguments, which helps to
      // automatically deallocate the memory of `d`
      // cannot use `d` directly in the below line (valgrind will report memory leak),
      // type matters?
      auto locals = py::dict( "self"_a = self, "callback"_a = pd);
      // embed python code
      py::exec(R"(
        self = locals()['self']
        callback = locals()['callback']
        # add pointer to callback
        if not hasattr(self, 'callback'):
          self.callbacks = []
        self.callbacks.append(callback)
       )", py::globals(), locals);


      // select wrapper callback function
      KIM::func * wrap_fptr;
      int error = false;
      if (computeCallbackName == COMPUTE_CALLBACK_NAME::GetNeighborList) {
        wrap_fptr = reinterpret_cast<KIM::func*> (&get_neigh);
      }
      else if (computeCallbackName == COMPUTE_CALLBACK_NAME::ProcessDEDrTerm) {
        wrap_fptr = reinterpret_cast<KIM::func*> (&process_dEdr);
      }
      else if (computeCallbackName == COMPUTE_CALLBACK_NAME::ProcessD2EDr2Term) {
        wrap_fptr = reinterpret_cast<KIM::func*> (&process_d2Edr2);
      }
      else {
        error = true;
      }

     // register callback to kim api
      error = error || self.SetCallbackPointer(computeCallbackName,
        LANGUAGE_NAME::cpp, wrap_fptr, pd);

      return error;
    },
    py::arg("ComputeCallbackName"),
    py::arg("ptr"),
    py::arg("dataObject")
  )


  .def("are_all_required_arguments_and_callbacks_present",
    [](ComputeArguments& self) {
      int result;
      self.AreAllRequiredArgumentsAndCallbacksPresent(&result);
      return result;
    }
  )

  .def("__repr__", &ComputeArguments::String);

}

