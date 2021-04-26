
#include <pybind11/pybind11.h>

#include <memory>
#include <string>

#include "KIM_Log.hpp"
#include "KIM_LogVerbosity.hpp"

namespace py = pybind11;
using namespace KIM;


namespace
{
struct PyLogDestroy
{
  void operator()(Log * log) const { Log::Destroy(&log); }
};
}  // namespace


PYBIND11_MODULE(log, module)
{
  module.doc() = "Python binding to KIM_Log.hpp";

  py::class_<Log, std::unique_ptr<Log, PyLogDestroy> > cl(module, "Log");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([]() {
      Log * log;
      Log::Create(&log);
      return std::unique_ptr<Log, PyLogDestroy>(std::move(log));
    }))
      .def("push_default_verbosity",
           &Log::PushDefaultVerbosity,
           "Push a new default log_verbosity onto the KIM-API "
           "global default verbosity stack.")
      .def("pop_default_verbosity",
           &Log::PopDefaultVerbosity,
           "Pop a log_verbosity from the KIM-API global default "
           "verbosity stack.")
      .def("get_id", &Log::GetID, "Get the identity of the Log object.")
      .def("set_id",
           &Log::SetID,
           "Set the identity of the Log object.",
           py::arg("id"))
      .def("push_verbosity",
           &Log::PushVerbosity,
           "Push a new log_verbosity onto the Log object's verbosity stack.",
           py::arg("log_verbosity"))
      .def("pop_verbosity",
           &Log::PopVerbosity,
           "Pop a log_verbosity from the Log object's verbosity stack.")
      // cannot simply do wrapping for the following overloaded functions as for
      // set_parameters in KIM_Model.hpp, because here the first argument is a
      // self-defined class. If we do that, pybind11 will report argument type
      // matching error.
      .def(
          "log_entry",
          [](Log & self,
             LogVerbosity const & log_verbosity,
             std::string const & message,
             int const line_number,
             std::string const & file_name) {
            self.LogEntry(log_verbosity, message, line_number, file_name);
          },
          "Write a log entry into the log file.",
          py::arg("log_verbosity"),
          py::arg("message"),
          py::arg("line_number"),
          py::arg("file_name"))
      .def(
          "log_entry",
          [](Log & self,
             LogVerbosity const & log_verbosity,
             std::stringstream const & message,
             int const line_number,
             std::string const & file_name) {
            self.LogEntry(log_verbosity, message, line_number, file_name);
          },
          "Write a log entry into the log file.",
          py::arg("log_verbosity"),
          py::arg("message"),
          py::arg("line_number"),
          py::arg("file_name"));

  // module functions

  module.def(
      "create",
      []() {
    Log * log;
    Log::Create(&log);
    return std::unique_ptr<Log, PyLogDestroy>(std::move(log));
      }, R"pbdoc(
         Create a new KIM-API Log object.

         Returns:
            Log: log
         )pbdoc"
      );
}
