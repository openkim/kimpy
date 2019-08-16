#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "KIM_SimulatorHeaders.hpp"
#include "callbacks.hpp"
#include "sim_buffer.h"

namespace py = pybind11;
using namespace pybind11::literals;
using namespace KIM;


PYBIND11_MODULE(compute_arguments, module)
{
  module.doc() = "Python binding to KIM_ComputeArguments.hpp";

  // C++ class constructor and destructor are private, which can not be wrapped
  // directly.
  // So we need to call the C++ class factory function in py::init, and use
  // `py::nodelete` to avoid calling the destructor when python instance is
  // destroyed. It is crucial that the instance is deallocated by calling the
  // destroy function from the C++ side to avoid memory leaks. For more info,
  // see http://pybind11.readthedocs.io/en/stable/advanced/classes.html

  py::class_<ComputeArguments, std::unique_ptr<ComputeArguments, py::nodelete> >
      cl(module, "ComputeArguments", py::dynamic_attr());

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([](Model & mo, py::array_t<int> error) {
      ComputeArguments * ca;
      auto e = error.mutable_data(0);
      e[0] = mo.ComputeArgumentsCreate(&ca);
      return ca;
    }))

      .def(
          "get_argument_support_status",
          [](ComputeArguments & self,
             ComputeArgumentName const computeArgumentName) {
            SupportStatus supportStatus;
            int error = self.GetArgumentSupportStatus(computeArgumentName,
                                                      &supportStatus);

            py::tuple re(2);
            re[0] = supportStatus;
            re[1] = error;
            return re;
          },
          py::arg("ComputeArgumentName"),
          "Return(SupportStatus, error)")

      .def(
          "get_callback_support_status",
          [](ComputeArguments & self,
             ComputeCallbackName const computeCallbackName) {
            SupportStatus supportStatus;
            int error = self.GetCallbackSupportStatus(computeCallbackName,
                                                      &supportStatus);

            py::tuple re(2);
            re[0] = supportStatus;
            re[1] = error;
            return re;
          },
          py::arg("ComputeCallbackName"),
          "Return(SupportStatus, error)")

      .def(
          "set_argument_pointer",
          [](ComputeArguments & self,
             ComputeArgumentName const computeArgumentName,
             py::array_t<int> ptr) {
            int * data = ptr.mutable_data(0);
            int error = self.SetArgumentPointer(computeArgumentName, data);
            return error;
          },
          py::arg("ComputeArgumentName"),
          py::arg("ptr").noconvert())

      .def(
          "set_argument_pointer",
          [](ComputeArguments & self,
             ComputeArgumentName const computeArgumentName,
             py::array_t<double> ptr) {
            double * data = ptr.mutable_data(0);
            int error = self.SetArgumentPointer(computeArgumentName, data);
            return error;
          },
          py::arg("ComputeArgumentName"),
          py::arg("ptr").noconvert())

      .def(
          "set_argument_null_pointer",
          [](ComputeArguments & self,
             ComputeArgumentName const computeArgumentName) {
            int error = self.SetArgumentPointer(
                computeArgumentName, reinterpret_cast<double *>(NULL));
            return error;
          },
          py::arg("ComputeArgumentName"))

      .def(
          "set_callback_pointer",
          [](ComputeArguments & self,
             ComputeCallbackName const computeCallbackName,
             void const * const
                 fptr,  // cannot use: KIM::Function * const fptr
                        // the argument passed in is of type: void const *
                        // we cast type explicitly in the funciton body
             void * const dataObject) {
            KIM::Function * new_fptr = (KIM::Function *) fptr;
            int error = self.SetCallbackPointer(
                computeCallbackName, LANGUAGE_NAME::cpp, new_fptr, dataObject);
            return error;
          },
          py::arg("ComputeCallbackName"),
          py::arg("callback_fn"),
          py::arg("dataObject"))

      .def(
          "set_callback",
          [](ComputeArguments & self,
             ComputeCallbackName const computeCallbackName,
             py::object & pyfunc,
             py::dict & dataObject) {
            // pack python callback function and dataObject into a new dict
            py::dict * d
                = new py::dict("pyfunc"_a = pyfunc, "pydata"_a = dataObject);

            SimBuffer * sim_buffer;
            self.GetSimulatorBufferPointer((void **) &sim_buffer);
            if (sim_buffer == NULL)
            {
              sim_buffer = new SimBuffer;
              self.SetSimulatorBufferPointer((void *) sim_buffer);
            }
            sim_buffer->callbacks.push_back(d);

            // select wrapper callback function
            KIM::Function * wrap_fptr;
            int error = false;
            if (computeCallbackName == COMPUTE_CALLBACK_NAME::GetNeighborList)
            { wrap_fptr = reinterpret_cast<KIM::Function *>(&get_neigh); }
            else if (computeCallbackName
                     == COMPUTE_CALLBACK_NAME::ProcessDEDrTerm)
            {
              wrap_fptr = reinterpret_cast<KIM::Function *>(&process_dEdr);
            }
            else if (computeCallbackName
                     == COMPUTE_CALLBACK_NAME::ProcessD2EDr2Term)
            {
              wrap_fptr = reinterpret_cast<KIM::Function *>(&process_d2Edr2);
            }
            else
            {
              error = true;
            }

            // register callback to kim api
            error
                = error
                  || self.SetCallbackPointer(computeCallbackName,
                                             LANGUAGE_NAME::cpp,
                                             wrap_fptr,
                                             reinterpret_cast<void * const>(d));

            return error;
          },
          py::arg("ComputeCallbackName"),
          py::arg("callback_fn"),
          py::arg("dataObject"))


      .def("are_all_required_arguments_and_callbacks_present",
           [](ComputeArguments & self) {
             int result;
             self.AreAllRequiredArgumentsAndCallbacksPresent(&result);
             return result;
           })

      .def("__repr__", &ComputeArguments::ToString)

      .def("set_log_id", &ComputeArguments::SetLogID)

      .def("push_log_verbosity", &ComputeArguments::PushLogVerbosity)

      .def("pop_log_verbosity", &ComputeArguments::PopLogVerbosity);
}
