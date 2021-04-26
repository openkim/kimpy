#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <memory>
#include <string>

#include "KIM_SimulatorHeaders.hpp"
#include "sim_buffer.h"

namespace py = pybind11;
using namespace KIM;


namespace
{
struct PySimulatorModelDestroy
{
  void operator()(SimulatorModel * simulator_model) const
  {
    SimulatorModel::Destroy(&simulator_model);
  }
};
}  // namespace


PYBIND11_MODULE(simulator_model, module)
{
  module.doc() = "Python binding to KIM_SimulatorModel.hpp";

  py::class_<SimulatorModel,
             std::unique_ptr<SimulatorModel, PySimulatorModelDestroy> >
      cl(module, "SimulatorModel");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([](std::string const &simulator_model_name) {
    SimulatorModel * simulator_model;

    int error = SimulatorModel::Create(simulator_model_name, &simulator_model);
    if (error == 1)
    {
      throw std::runtime_error("Unable to create a new KIM-API Model object!");
    }

    return std::unique_ptr<SimulatorModel, PySimulatorModelDestroy>(
        std::move(simulator_model));
    }))
    .def("get_simulator_name_and_version", [](SimulatorModel &self) {
    std::string const * simulator_name;
    std::string const * simulator_version;

    self.GetSimulatorNameAndVersion(&simulator_name, &simulator_version);

    py::tuple re(2);
    re[0] = *simulator_name;
    re[1] = *simulator_version;
    return re;
    }, R"pbdoc(
       Get the SimulatorModel's simulator name and version.

       Returns:
           str, str: simulator_name, simulator_version
       )pbdoc")
    .def("get_number_of_supported_species", [](SimulatorModel &self) {
    int number_of_supported_species;

    self.GetNumberOfSupportedSpecies(&number_of_supported_species);

    return number_of_supported_species;
    }, R"pbdoc(
       Get the number of species supported by the SimulatorModel.

       Returns:
           int: number_of_supported_species
       )pbdoc")
    .def("get_supported_species", [](SimulatorModel &self, int const index) {
    std::string const * species_name;

    int error = self.GetSupportedSpecies(index, &species_name);
    if (error == 1) { throw std::runtime_error("index is invalid!"); }

    return *species_name;
    }, R"pbdoc(
       Get a species name supported by the SimulatorModel.

       Returns:
           str: species_name
       )pbdoc",
       py::arg("index"))
    .def("open_and_initialize_template_map",
         &SimulatorModel::OpenAndInitializeTemplateMap,
         "Open and initialize the template map for simulator field line "
         "substitutions.")
    .def("template_map_is_open", &SimulatorModel::TemplateMapIsOpen,
         "Determine if the template map is open.")
    .def("add_template_map",
         [](SimulatorModel &self,
            std::string const &key,
            std::string const &value) {
    int error = self.AddTemplateMap(key, value);
    if (error == 1)
    {
      throw std::runtime_error(
          "the template map has been closed by a call to "
          "close_template_map!  or\nkey contains invalid characters!");
    }
    }, "Add a new key-value entry to the template map.",
       py::arg("key"),
       py::arg("value"))
    .def("close_template_map", &SimulatorModel::CloseTemplateMap,
         "Close the template map and perform template substitutions.")
    .def("get_number_of_simulator_fields", [](SimulatorModel &self) {
    int number_of_simulator_fields;

    self.GetNumberOfSimulatorFields(&number_of_simulator_fields);

    return number_of_simulator_fields;
    }, R"pbdoc(
       Get the number of simulator fields provided by the SimulatorModel.

       Returns:
           int: number_of_simulator_fields
       )pbdoc")
    .def("get_simulator_field_metadata",
         [](SimulatorModel &self, int const field_index) {
    int extent;
    std::string const * field_name;

    int error
        = self.GetSimulatorFieldMetadata(field_index, &extent, &field_name);
    if (error == 1) { throw std::runtime_error("field_index is invalid!"); }

    py::tuple re(2);
    re[0] = extent;
    re[1] = *field_name;
    return re;
    }, R"pbdoc(
       Get the metadata for the simulator field of interest.

       Returns:
           int, str: extent, field_name
       )pbdoc",
       py::arg("field_index"))
    .def("get_simulator_field_line",
         [](SimulatorModel &self,
            int const field_index,
            int const line_index) {
    std::string const * line_value;

    int error
        = self.GetSimulatorFieldLine(field_index, line_index, &line_value);
    if (error == 1)
    {
      throw std::runtime_error("the template map is open!  or\n"
                               "field_index is invalid!    or\n"
                               "line_index is invalid!");
    }

    return *line_value;
    }, R"pbdoc(
       Get a line for the simulator field of interest with all template
       substitutions performed (Requires the template map is closed).

       Returns:
           str: line_value
       )pbdoc",
       py::arg("field_index"),
       py::arg("line_index"))
    .def("get_parameter_file_directory_name", [](SimulatorModel &self) {
    std::string const * directory_name;

    self.GetParameterFileDirectoryName(&directory_name);

    return *directory_name;
    }, R"pbdoc(
       Get absolute path name of the temporary directory where parameter
       files provided by the SimulatorModel are written.

       Returns:
           str: directory_name
       )pbdoc")
    .def("get_specification_file_name", [](SimulatorModel &self) {
    std::string const * specification_file_name;

    self.GetSpecificationFileName(&specification_file_name);

    return *specification_file_name;
    }, R"pbdoc(
       Get the SimulatorModel's specification file basename (file name without
       path).  The file is located in the SimulatorModel's parameter file
       directory.

       Returns:
           str: specification_file_name
       )pbdoc")
    .def("get_number_of_parameter_files", [](SimulatorModel &self) {
    int number_of_parameter_files;

    self.GetNumberOfParameterFiles(&number_of_parameter_files);

    return number_of_parameter_files;
    }, R"pbdoc(
       Get the number of parameter files provided by the SimulatorModel.

       Returns:
           int: number_of_parameter_files
       )pbdoc")
    .def("get_parameter_file_name",
         [](SimulatorModel &self, int const index) {
    std::string const * parameter_file_name;

    int error = self.GetParameterFileName(index, &parameter_file_name);
    if (error == 1) { throw std::runtime_error("index is invalid!"); }

    return *parameter_file_name;
    }, R"pbdoc(
       Get the basename (file name without path) of a particular parameter
       file.  The file is located in the SimulatorModel's parameter file
       directory.

       Returns:
           str: parameter_file_name
       )pbdoc",
       py::arg("index"))
    .def("get_parameter_file_base_name",
         [](SimulatorModel &self, int const index) {
    std::string const * param_file_base_name;

    int error = self.GetParameterFileBasename(index, &param_file_base_name);
    if (error == 1) { throw std::runtime_error("index is invalid!"); }

    return *param_file_base_name;
    }, R"pbdoc(
       Get the basename (file name without path) of a particular parameter
       file.  The file is located in the SimulatorModel's parameter file
       directory.

       Returns:
           str: param_file_base_name
       )pbdoc",
       py::arg("index"))
    .def("__repr__", &SimulatorModel::ToString)
    .def("set_log_id", &SimulatorModel::SetLogID,
         "Set the identity of the Log object associated with the "
         "SimulatorModel object.",
         py::arg("log_id"))
    .def("push_log_verbosity", &SimulatorModel::PushLogVerbosity,
         "Push a new log_verbosity onto the SimulatorModel object's Log "
         "object verbosity stack.",
         py::arg("log_verbosity"))
    .def("pop_log_verbosity", &SimulatorModel::PopLogVerbosity,
         "Pop a log_verbosity from the SimulatorModel object's Log object "
         "verbosity stack.");

  // module functions

  module.def(
      "create",
      [](std::string const & simulator_model_name) {
    SimulatorModel * simulator_model;

    int error = SimulatorModel::Create(simulator_model_name, &simulator_model);
    if (error == 1)
    {
      throw std::runtime_error("Unable to create a new KIM-API Model object!");
    }

    return std::unique_ptr<SimulatorModel, PySimulatorModelDestroy>(
        std::move(simulator_model));
      }, R"pbdoc(
         Create a new KIM-API SimulatorModel object.

         Returns:
             SimulatorModel: simulator_model
         )pbdoc"
      );
}
