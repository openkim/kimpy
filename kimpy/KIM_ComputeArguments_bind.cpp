#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <memory>

#include "KIM_SimulatorHeaders.hpp"
#include "callbacks.hpp"
#include "py_kim_wrapper.h"
#include "sim_buffer.h"

namespace py = pybind11;
using namespace pybind11::literals;
using namespace KIM;


PYBIND11_MODULE(compute_arguments, module)
{
  module.doc() = "Python binding to KIM_ComputeArguments.hpp";

  py::class_<PyComputeArguments, std::shared_ptr<PyComputeArguments> > cl(
      module, "PyComputeArguments", py::dynamic_attr());

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([](std::shared_ptr<PyModel> model) {
    std::shared_ptr<PyComputeArguments> compute_arguments(
        new PyComputeArguments(model));

    int error = model->_model->ComputeArgumentsCreate(
        &compute_arguments->_compute_arguments);
    if (error == 1)
    {
      throw std::runtime_error(
          "Unable to create a new ComputeArguments object "
          "for the model object!");
    }

    return compute_arguments;
    }))
    .def("get_argument_support_status",
         [](PyComputeArguments &self,
            ComputeArgumentName const &compute_argument_name) {
    SupportStatus support_status;

    int error = self._compute_arguments->GetArgumentSupportStatus(
        compute_argument_name, &support_status);
    if (error == 1)
    {
      throw std::runtime_error("compute_argument_name is unknown!");
    }

    return support_status;
    }, R"pbdoc(
       Get the support_status of a compute_argument_name.

       Returns:
           SupportStatus: support_status
       )pbdoc",
       py::arg("compute_argument_name"))
    .def("get_callback_support_status",
         [](PyComputeArguments &self,
            ComputeCallbackName const &compute_callback_name) {
    SupportStatus support_status;

    int error = self._compute_arguments->GetCallbackSupportStatus(
        compute_callback_name, &support_status);
    if (error == 1)
    {
      throw std::runtime_error("compute_callback_name is unknown!");
    }

    return support_status;
    }, R"pbdoc(
       Get the support_status of a compute_callback_name.

       Returns:
           SupportStatus: support_status
       )pbdoc",
       py::arg("compute_callback_name"))
    .def("set_argument_pointer",
         [](PyComputeArguments &self,
            ComputeArgumentName const &compute_argument_name,
            py::array_t<int> ptr) {
    int * data = ptr.mutable_data(0);

    int error = self._compute_arguments->SetArgumentPointer(
        compute_argument_name, data);
    if (error == 1)
    {
      throw std::runtime_error("compute_argument_name is unknown!");
    }
    }, "Set the data pointer for a compute_argument_name.",
       py::arg("compute_argument_name"),
       py::arg("ptr").noconvert())
    .def("set_argument_pointer",
         [](PyComputeArguments &self,
            ComputeArgumentName const &compute_argument_name,
            py::array_t<double> ptr) {
    double * data = ptr.mutable_data(0);

    int error = self._compute_arguments->SetArgumentPointer(
        compute_argument_name, data);
    if (error == 1)
    {
      throw std::runtime_error("compute_argument_name is unknown!");
    }
    }, "Set the data pointer for a compute_argument_name.",
       py::arg("compute_argument_name"),
       py::arg("ptr").noconvert())
    .def("set_argument_null_pointer",
         [](PyComputeArguments &self,
            ComputeArgumentName const &compute_argument_name) {
    int error = self._compute_arguments->SetArgumentPointer(
        compute_argument_name, static_cast<double *>(nullptr));
    if (error == 1)
    {
      throw std::runtime_error("compute_argument_name is unknown!");
    }
    }, "Set a null data pointer for a compute_argument_name.",
       py::arg("compute_argument_name"))
    .def("set_callback_pointer",
         [](PyComputeArguments &self,
            ComputeCallbackName const &compute_callback_name,
            void const *const fptr, // cannot use: KIM::Function *const fptr
            void *const data_object) {
    // the argument passed in is of type: void const *
    // we cast type explicitly here
    KIM::Function * new_fptr = (KIM::Function *) fptr;

    int error = self._compute_arguments->SetCallbackPointer(
        compute_callback_name, LANGUAGE_NAME::cpp, new_fptr, data_object);
    if (error == 1)
    {
      throw std::runtime_error(
          "compute_callback_name is unknown!  or\n fptr is not NULL "
          "and compute_callback_name is notSupported!");
    }
    }, "Set the function pointer for a compute_callback_name.",
       py::arg("compute_callback_name"),
       py::arg("callback_fn"),
       py::arg("data_object"))
    .def("set_callback",
         [](PyComputeArguments &self,
            ComputeCallbackName const &compute_callback_name,
            py::object &pyfunc,
            py::dict &data_object) {
    // pack python callback function and dataObject into a new dict
    py::dict * pydict
        = new py::dict("pyfunc"_a = pyfunc, "pydata"_a = data_object);

    SimBuffer * sim_buffer;

    self._compute_arguments->GetSimulatorBufferPointer((void **) &sim_buffer);

    if (!sim_buffer)
    {
      sim_buffer = new SimBuffer;

      self._compute_arguments->SetSimulatorBufferPointer((void *) sim_buffer);
    }

    sim_buffer->callbacks.push_back(pydict);

    // select wrapper callback function
    KIM::Function * wrap_fptr;

    if (compute_callback_name == COMPUTE_CALLBACK_NAME::GetNeighborList)
    {
      wrap_fptr = reinterpret_cast<KIM::Function *>(&get_neigh);
    }
    else if (compute_callback_name == COMPUTE_CALLBACK_NAME::ProcessDEDrTerm)
    {
      wrap_fptr = reinterpret_cast<KIM::Function *>(&process_dEdr);
    }
    else if (compute_callback_name == COMPUTE_CALLBACK_NAME::ProcessD2EDr2Term)
    {
      wrap_fptr = reinterpret_cast<KIM::Function *>(&process_d2Edr2);
    }
    else
    {
      throw std::runtime_error("compute_callback_name is unknown!");
    }

    // register callback to kim api
    int error = self._compute_arguments->SetCallbackPointer(
        compute_callback_name,
        LANGUAGE_NAME::cpp,
        wrap_fptr,
        reinterpret_cast<void * const>(pydict));
    if (error == 1)
    {
      throw std::runtime_error(
          "fptr is not NULL and compute_callback_name is not Supported!");
    }
    }, "Set the function pointer for a compute_callback_name.",
       py::arg("compute_callback_name"),
       py::arg("callback_fn"),
       py::arg("data_object"))
    .def("are_all_required_arguments_and_callbacks_present",
         [](PyComputeArguments &self) {
    int result;

    self._compute_arguments->AreAllRequiredArgumentsAndCallbacksPresent(
        &result);

    return result;
    }, R"pbdoc(
       Determine if non-NULL pointers have been set for all
       compute_argument_name's and compute_callback_name's with support_status
       values of required By API.

       Returns:
           int: result
       )pbdoc")
    .def("__repr__", [](PyComputeArguments &self) {
    return self._compute_arguments->ToString();
    }, "Get a string representing the internal state of the "
       "ComputeArguments object")
    .def("set_log_id",
         [](PyComputeArguments &self, std::string const &log_id) {
    self._compute_arguments->SetLogID(log_id);
    }, "Set the identity of the Log object associated with the "
       "ComputeArguments object.",
       py::arg("log_id"))
    .def("push_log_verbosity",
         [](PyComputeArguments &self, LogVerbosity const &log_verbosity) {
    self._compute_arguments->PushLogVerbosity(log_verbosity);
    }, "Push a new log_verbosity onto the ComputeArguments object's Log "
       "object verbosity stack.",
       py::arg("log_verbosity"))
    .def("pop_log_verbosity", [](PyComputeArguments &self) {
    self._compute_arguments->PopLogVerbosity();
    }, "Pop a log_verbosity from the ComputeArguments object's Log object "
       "verbosity stack.");
}
