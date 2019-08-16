#include "KIM_Log.hpp"
#include "KIM_LogVerbosity.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(log, module)
{
  module.doc() = "Python binding to KIM_Log.hpp";

  // C++ class constructor and destructor are private, which can not be wrapped
  // directly.
  // So we need to call the C++ class factory function in py::init, and use
  // `py::nodelete` to avoid calling the destructor when python instance is
  // destroyed. It is crucial that the instance is deallocated by calling the
  // destroy function from the C++ side to avoid memory leaks. For more info,
  // see http://pybind11.readthedocs.io/en/stable/advanced/classes.html

  py::class_<Log, std::unique_ptr<Log, py::nodelete> > cl(module, "Log");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([]() {
      Log * log;
      Log::Create(&log);
      return log;
    }))


      .def("pop_default_verbosity", &Log::PopDefaultVerbosity)

      .def("get_id", &Log::GetID)

      .def("set_id", &Log::SetID)

      .def("push_verbosity", &Log::PushVerbosity)

      .def("pop_verbosity", &Log::PopVerbosity)

      // cannot simply do wrapping for the following overloaded functions as for
      // set_parameters in KIM_Model.hpp, because here the first argument is a
      // self-defined class. If we do that, pybind11 will report argument type
      // matching error.
      .def(
          "log_entry",
          [](Log & self,
             LogVerbosity const logVerbosity,
             std::string const & message,
             int const lineNumber,
             std::string const & fileName) {
            self.LogEntry(logVerbosity, message, lineNumber, fileName);
          },
          py::arg("logVerbosity"),
          py::arg("message"),
          py::arg("lineNumber"),
          py::arg("fileNmae"))

      .def(
          "log_entry",
          [](Log & self,
             LogVerbosity const logVerbosity,
             std::stringstream const & message,
             int const lineNumber,
             std::string const & fileName) {
            self.LogEntry(logVerbosity, message, lineNumber, fileName);
          },
          py::arg("logVerbosity"),
          py::arg("message"),
          py::arg("lineNumber"),
          py::arg("fileNmae"));


  // module functions

  module.def("create", []() {
    Log * log;
    Log::Create(&log);
    return log;
  });

  module.def("destroy", [](Log * self) { Log::Destroy(&self); });
}
