#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "KIM_SimulatorHeaders.hpp"
#include "sim_buffer.h"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(model, module)
{
  module.doc() = "Python binding to KIM_Model.hpp";

  // C++ class constructor and destructor are private, which can not be wrapped
  // directly.
  // So we need to call the C++ class factory function in py::init, and use
  // `py::nodelete` to avoid calling the destructor when python instance is
  // destroyed. It is crucial that the instance is deallocated by calling the
  // destroy function from the C++ side to avoid memory leaks. For more info,
  // see http://pybind11.readthedocs.io/en/stable/advanced/classes.html

  py::class_<Model, std::unique_ptr<Model, py::nodelete> > cl(module, "Model");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([](Numbering const numbering,
                     LengthUnit const requestedLengthUnit,
                     EnergyUnit const requestedEnergyUnit,
                     ChargeUnit const requestedChargeUnit,
                     TemperatureUnit const requestedTemperatureUnit,
                     TimeUnit const requestedTimeUnit,
                     std::string const & modelName,
                     py::array_t<int> requestedUnitsAccepted,
                     py::array_t<int> error) {
      Model * mo;
      int * e = error.mutable_data(0);
      int * r = requestedUnitsAccepted.mutable_data(0);
      e[0] = Model::Create(numbering,
                           requestedLengthUnit,
                           requestedEnergyUnit,
                           requestedChargeUnit,
                           requestedTemperatureUnit,
                           requestedTimeUnit,
                           modelName,
                           r,
                           &mo);
      return mo;
    }))

      .def(
          "is_routine_present",
          [](Model & self, ModelRoutineName const modelRoutineName) {
            int present;
            int required;
            int error
                = self.IsRoutinePresent(modelRoutineName, &present, &required);

            py::tuple re(3);
            re[0] = present;
            re[1] = required;
            re[2] = error;
            return re;
          },
          py::arg("ModelRoutineName"),
          "Return(present, required, error)")

      .def(
          "get_influence_distance",
          [](Model & self) {
            double influenceDistance;
            self.GetInfluenceDistance(&influenceDistance);

            return influenceDistance;
          },
          "Return influenceDistance")

      .def(
          "get_neighbor_list_cutoffs_and_hints",
          [](Model & self) {
            int numberOfCutoffs;
            double const * cutoff_ptr;
            int const * paddingNoNeighborHints_ptr;
            self.GetNeighborListPointers(
                &numberOfCutoffs, &cutoff_ptr, &paddingNoNeighborHints_ptr);

            py::array_t<double, py::array::c_style> cutoffs(
                {static_cast<size_t>(numberOfCutoffs)});
            py::array_t<int, py::array::c_style> paddingNoNeighborHints(
                {static_cast<size_t>(numberOfCutoffs)});
            auto cut = cutoffs.mutable_data(0);
            auto pnh = paddingNoNeighborHints.mutable_data(0);
            for (int i = 0; i < numberOfCutoffs; i++)
            {
              cut[i] = cutoff_ptr[i];
              pnh[i] = paddingNoNeighborHints_ptr[i];
            }

            py::tuple re(2);
            re[0] = cutoffs;
            re[1] = paddingNoNeighborHints;
            return re;
          },
          "Return (cutoffs, "
          "model_not_request_neighbors_of_noncontributing_particles)")

      .def(
          "get_units",
          [](Model & self) {
            LengthUnit lengthUnit;
            EnergyUnit energyUnit;
            ChargeUnit chargeUnit;
            TemperatureUnit temperatureUnit;
            TimeUnit timeUnit;
            self.GetUnits(&lengthUnit,
                          &energyUnit,
                          &chargeUnit,
                          &temperatureUnit,
                          &timeUnit);

            py::tuple re(5);
            re[0] = lengthUnit;
            re[1] = energyUnit;
            re[2] = chargeUnit;
            re[3] = temperatureUnit;
            re[4] = timeUnit;
            return re;
          },
          "Return(LengthUnit, EnergyUnit, ChargeUnit, TemperatureUnit, "
          "TimeUnit)")

      .def(
          "compute_arguments_create",
          [](Model & self) {
            ComputeArguments * computeArguments;
            int error = self.ComputeArgumentsCreate(&computeArguments);
            py::tuple re(2);
            re[0] = computeArguments;
            re[1] = error;
            return re;
          },
          "Return(ComputeArguments, error)")

      .def(
          "compute_arguments_destroy",
          [](Model & self, ComputeArguments * computeArguments) {
            SimBuffer * sim_buffer;
            computeArguments->GetSimulatorBufferPointer((void **) &sim_buffer);
            if (sim_buffer != NULL)
            {
              for (size_t i = 0; i < sim_buffer->callbacks.size(); i++)
              { delete sim_buffer->callbacks[i]; }
              delete sim_buffer;
              sim_buffer = NULL;
            }

            int error = self.ComputeArgumentsDestroy(&computeArguments);
            return error;
          },
          py::arg("ComputeArguments"),
          "Return error")

      .def(
          "compute",
          [](Model & self,
             ComputeArguments const * const computeArguments,
             bool release_GIL) {
            if (release_GIL)
            {
              py::gil_scoped_release release;
              int error = self.Compute(computeArguments);
              return error;
            }
            else
            {
              int error = self.Compute(computeArguments);
              return error;
            }
          },
          py::arg("ComputeArguments"),
          py::arg("release_GIL") = false,
          "Return error")

      .def("clear_then_refresh", &Model::ClearThenRefresh)

      .def(
          "write_parameterized_model",
          [](Model & self,
             std::string const & path,
             std::string const & modelName) {
            int error = self.WriteParameterizedModel(path, modelName);

            return error;
          },
          py::arg("path"),
          py::arg("modelName"),
          "Return error")

      .def(
          "get_species_support_and_code",
          [](Model & self, SpeciesName const speciesName) {
            int speciesIsSupported;
            int code = -1;
            int error = self.GetSpeciesSupportAndCode(
                speciesName, &speciesIsSupported, &code);

            py::tuple re(3);
            re[0] = speciesIsSupported;
            re[1] = code;
            re[2] = error;
            return re;
          },
          py::arg("SpeciesName"),
          "Return(speciesIsSupported, code, error)")

      .def(
          "get_number_of_parameters",
          [](Model & self) {
            int numberOfParameters;
            self.GetNumberOfParameters(&numberOfParameters);
            return numberOfParameters;
          },
          "Return numberOfParameters")

      .def(
          "get_parameter_metadata",
          [](Model & self, int const index) {
            DataType dataType;
            int extent;
            std::string const * name;
            std::string const * description;

            int error = self.GetParameterMetadata(
                index, &dataType, &extent, &name, &description);
            py::tuple re(5);
            re[0] = dataType;
            re[1] = extent;
            re[2] = *name;
            re[3] = *description;
            re[4] = error;
            return re;
          },
          py::arg("index"),
          "Return(DataType, extent, description, error)")

      .def(
          "get_parameter_int",
          [](Model & self, int const parameterIndex, int const arrayIndex) {
            int parameterValue;
            int error = self.GetParameter(
                parameterIndex, arrayIndex, &parameterValue);

            py::tuple re(2);
            re[0] = parameterValue;
            re[1] = error;
            return re;
          },
          py::arg("parameterIndex"),
          py::arg("arrayIndex"),
          "Return(parameterValue, error)")

      .def(
          "get_parameter_double",
          [](Model & self, int const parameterIndex, int const arrayIndex) {
            double parameterValue;
            int error = self.GetParameter(
                parameterIndex, arrayIndex, &parameterValue);

            py::tuple re(2);
            re[0] = parameterValue;
            re[1] = error;
            return re;
          },
          py::arg("parameterIndex"),
          py::arg("arrayIndex"),
          "Return(parameterValue, error)")

      // overloaded function
      .def("set_parameter",
           (int (Model::*)(int const, int const, int const))
               & Model::SetParameter)

      .def("set_parameter",
           (int (Model::*)(int const, int const, double const))
               & Model::SetParameter)

      .def("__repr__", &Model::ToString)

      .def("set_log_id", &Model::SetLogID)

      .def("push_log_verbosity", &Model::PushLogVerbosity)

      .def("pop_log_verbosity", &Model::PopLogVerbosity);


  // module functions

  module.def(
      "create",
      [](Numbering const numbering,
         LengthUnit const requestedLengthUnit,
         EnergyUnit const requestedEnergyUnit,
         ChargeUnit const requestedChargeUnit,
         TemperatureUnit const requestedTemperatureUnit,
         TimeUnit const requestedTimeUnit,
         std::string const & modelName) {
        Model * mo;
        int requestedUnitsAccepted;
        int error = Model::Create(numbering,
                                  requestedLengthUnit,
                                  requestedEnergyUnit,
                                  requestedChargeUnit,
                                  requestedTemperatureUnit,
                                  requestedTimeUnit,
                                  modelName,
                                  &requestedUnitsAccepted,
                                  &mo);

        py::tuple re(3);
        re[0] = requestedUnitsAccepted;
        re[1] = mo;
        re[2] = error;
        return re;
      },
      py::arg("Numbering"),
      py::arg("LengthUnit"),
      py::arg("EnergyUnit"),
      py::arg("ChargeUnit"),
      py::arg("TemperatureUnit"),
      py::arg("TimeUnit"),
      py::arg("modelName"),
      "Return(requestedUnitsAccepted, Model, error)");

  module.def("destroy", [](Model * self) { Model::Destroy(&self); });
}
