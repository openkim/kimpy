#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "KIM_SimulatorHeaders.hpp"
#include "sim_buffer.h"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(simulator_model, module)
{
  module.doc() = "Python binding to KIM_SimulatorModel.hpp";

  // C++ class constructor and destructor are private, which can not be wrapped
  // directly.
  // So we need to call the C++ class factory function in py::init, and use
  // `py::nodelete` to avoid calling the destructor when python instance is
  // destroyed. It is crucial that the instance is deallocated by calling the
  // destroy function from the C++ side to avoid memory leaks. For more info,
  // see http://pybind11.readthedocs.io/en/stable/advanced/classes.html

  py::class_<SimulatorModel, std::unique_ptr<SimulatorModel, py::nodelete> > cl(
      module, "SimulatorModel");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([](std::string const & simulatorModelName,
                     py::array_t<int> error) {
      SimulatorModel * mo;
      int * e = error.mutable_data(0);
      e[0] = SimulatorModel::Create(simulatorModelName, &mo);
      return mo;
    }))
      .def(
          "get_simulator_name_and_version",
          [](SimulatorModel & self) {
            std::string const * simulatorName;
            std::string const * simulatorVersion;
            self.GetSimulatorNameAndVersion(&simulatorName, &simulatorVersion);
            py::tuple re(2);
            re[0] = *simulatorName;
            re[1] = *simulatorVersion;
            return re;
          },
          "Return(SimulatorName, SimulatorVersion)")

      .def(
          "get_number_of_supported_species",
          [](SimulatorModel & self) {
            int numberOfSupportedSpecies;
            self.GetNumberOfSupportedSpecies(&numberOfSupportedSpecies);
            return numberOfSupportedSpecies;
          },
          "Return numberOfSupportedSpecies")

      .def(
          "get_supported_species",
          [](SimulatorModel & self, int const index) {
            std::string const * speciesName;
            int error = self.GetSupportedSpecies(index, &speciesName);

            py::tuple re(2);
            re[0] = *speciesName;
            re[1] = error;
            return re;
          },
          py::arg("index"),
          "Return(speciesName, error)")

      .def("open_and_initialize_template_map",
           &SimulatorModel::OpenAndInitializeTemplateMap)

      .def("template_map_is_open", &SimulatorModel::TemplateMapIsOpen)

      .def("add_template_map", &SimulatorModel::AddTemplateMap)

      .def("close_template_map", &SimulatorModel::CloseTemplateMap)

      .def(
          "get_number_of_simulator_fields",
          [](SimulatorModel & self) {
            int numberOfSimulatorFields;
            self.GetNumberOfSimulatorFields(&numberOfSimulatorFields);

            return numberOfSimulatorFields;
          },
          "Return numberOfSimulatorFields")

      .def(
          "get_simulator_field_metadata",
          [](SimulatorModel & self, int const fieldIndex) {
            int extent;
            std::string const * fieldName;
            int error = self.GetSimulatorFieldMetadata(
                fieldIndex, &extent, &fieldName);

            py::tuple re(3);
            re[0] = extent;
            re[1] = *fieldName;
            re[2] = error;
            return re;
          },
          py::arg("fieldIndex"),
          "Return(extent, fieldName, error)")

      .def(
          "get_simulator_field_line",
          [](SimulatorModel & self, int const fieldIndex, int const lineIndex) {
            std::string const * lineValue;
            int error
                = self.GetSimulatorFieldLine(fieldIndex, lineIndex, &lineValue);

            py::tuple re(2);
            re[0] = *lineValue;
            re[1] = error;
            return re;
          },
          py::arg("fieldIndex"),
          py::arg("lineIndex"),
          "Return(lineValue, error)")

      .def(
          "get_parameter_file_directory_name",
          [](SimulatorModel & self) {
            std::string const * directoryName;
            self.GetParameterFileDirectoryName(&directoryName);

            return *directoryName;
          },
          "Return directoryName")

      .def(
          "get_specification_file_name",
          [](SimulatorModel & self) {
            std::string const * specificationFileName;
            self.GetSpecificationFileName(&specificationFileName);

            return *specificationFileName;
          },
          "Return specificationFileName")

      .def(
          "get_number_of_parameter_files",
          [](SimulatorModel & self) {
            int numberOfParameterFiles;
            self.GetNumberOfParameterFiles(&numberOfParameterFiles);

            return numberOfParameterFiles;
          },
          "Return numberOfParameterFiles")

      .def(
          "get_parameter_file_name",
          [](SimulatorModel & self, int const index) {
            std::string const * paramFileName;
            int error = self.GetParameterFileName(index, &paramFileName);

            py::tuple re(2);
            re[0] = *paramFileName;
            re[1] = error;
            return re;
          },
          py::arg("index"),
          "Return(paramFileName, error)")

      .def("__repr__", &SimulatorModel::ToString)

      .def("set_log_id", &SimulatorModel::SetLogID)

      .def("push_log_verbosity", &SimulatorModel::PushLogVerbosity)

      .def("pop_log_verbosity", &SimulatorModel::PopLogVerbosity);


  // module functions

  module.def(
      "create",
      [](std::string const & simulatorModelName) {
        SimulatorModel * mo;
        int error = SimulatorModel::Create(simulatorModelName, &mo);

        py::tuple re(2);
        re[0] = mo;
        re[1] = error;
        return re;
      },
      "Return(SimulatorModel, error)");

  module.def("destroy",
             [](SimulatorModel * self) { SimulatorModel::Destroy(&self); });
}
