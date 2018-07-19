#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "KIM_SimulatorHeaders.hpp"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(model, module) {
  module.doc() = "Python binding to KIM_Model.hpp";

  // C++ class constructor and destructor are private, which can not be wrapped
  // directly.
  // So we need to call the C++ class factory function in py::init, and use
  // `py::nodelete` to avoid calling the destructor when python instance is destroyed.
  // It is crucial that the instance is deallocated by calling the destroy function
  // from the C++ side to avoid memory leaks.
  // For more info, see http://pybind11.readthedocs.io/en/stable/advanced/classes.html

  py::class_<Model, std::unique_ptr<Model, py::nodelete>> cl (module, "Model");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init(
    [](Numbering const numbering,
      LengthUnit const requestedLengthUnit,
      EnergyUnit const requestedEnergyUnit,
      ChargeUnit const requestedChargeUnit,
      TemperatureUnit const requestedTemperatureUnit,
      TimeUnit const requestedTimeUnit,
      std::string const & modelName,
      py::array_t<int> requestedUnitsAccepted,
      py::array_t<int> error
    ) {
      Model * mo;
      int* e = error.mutable_data(0);
      int* r = requestedUnitsAccepted.mutable_data(0);
      e[0] = Model::Create(numbering, requestedLengthUnit,
          requestedEnergyUnit, requestedChargeUnit, requestedTemperatureUnit,
          requestedTimeUnit, modelName, r, &mo);
      return mo;
    }
  ))

  .def("get_influence_distance",
    [](Model& self) {
      double influenceDistance;
      self.GetInfluenceDistance(&influenceDistance);
      return influenceDistance;
    }
  )

  .def("get_neighbor_list_cutoffs_and_hints",
    [](Model& self) {
      int numberOfCutoffs;
      double const * cutoff_ptr;
      int const * paddingNeighborHints_ptr;
      int const * halfListHints_ptr;
      self.GetNeighborListPointers(&numberOfCutoffs, &cutoff_ptr,
          &paddingNeighborHints_ptr, &halfListHints_ptr);

      py::array_t<double, py::array::c_style> cutoffs (
          {static_cast<size_t> (numberOfCutoffs)});
      py::array_t<int, py::array::c_style> paddingNeighborHints (
          {static_cast<size_t> (numberOfCutoffs)});
      py::array_t<int, py::array::c_style> halfListHints (
          {static_cast<size_t> (numberOfCutoffs)});
      auto cut = cutoffs.mutable_data(0);
      auto pnh = paddingNeighborHints.mutable_data(0);
      auto hlh = halfListHints.mutable_data(0);
      for(int i=0; i<numberOfCutoffs; i++) {
        cut[i] = cutoff_ptr[i];
        pnh[i] = paddingNeighborHints_ptr[i];
        hlh[i] = halfListHints_ptr[i];
      }

      py::tuple re(3);
      re[0] = cutoffs;
      re[1] = paddingNeighborHints;
      re[2] = halfListHints;

      return re;
    }
  )

  .def("get_units",
    [](Model& self) {
      LengthUnit lengthUnit;
      EnergyUnit energyUnit;
      ChargeUnit chargeUnit;
      TemperatureUnit temperatureUnit;
      TimeUnit timeUnit;
      self.GetUnits(&lengthUnit, &energyUnit, &chargeUnit, &temperatureUnit, &timeUnit);

      py::tuple re(5);
      re[0] = lengthUnit;
      re[1] = energyUnit;
      re[2] = chargeUnit;
      re[3] = temperatureUnit;
      re[4] = timeUnit;
      return re;
    },
    "Return(LengthUnit, EnergyUnit, ChargeUnit, TemperatureUnit, TimeUnit)"
  )

  .def("compute_arguments_create",
    [](Model& self) {
      ComputeArguments * computeArguments;
      int error = self.ComputeArgumentsCreate(&computeArguments);
      py::tuple re(2);
      re[0] = computeArguments;
      re[1] = error;
      return re;
    },
    "Return(ComputeArguments, error)"
  )

  .def("compute_arguments_destroy",
    [](Model& self, ComputeArguments * computeArguments) {
      int error = self.ComputeArgumentsDestroy(&computeArguments);
      return error;
    },
    py::arg("ComputeArguments")
  )

  .def("compute",
    [](Model& self, ComputeArguments const * const computeArguments, bool release_GIL) {
      if (release_GIL) {
        py::gil_scoped_release release;
        int error = self.Compute(computeArguments);
        return error;
      }
      else {
        int error = self.Compute(computeArguments);
        return error;
      }
    },
      py::arg("ComputeArguments"),
      py::arg("release_GIL") = false
    )

  .def("clear_then_refresh",
      &Model::ClearThenRefresh)

  .def("get_species_support_and_code",
    [](Model& self, SpeciesName const speciesName) {
      int speciesIsSupported;
      int code;
      int error = self.GetSpeciesSupportAndCode(speciesName, &speciesIsSupported, &code);

      py::tuple re(3);
      re[0] = speciesIsSupported;
      re[1] = code;
      re[2] = error;
      return re;
    },
    py::arg("SpeciesName"),
    "Return(speciesIsSupported, code, error)"
  )

  .def("get_number_of_parameters",
    [](Model& self) {
      int numberOfParameters;
      self.GetNumberOfParameters(&numberOfParameters);
      return numberOfParameters;
    }
  )

  .def("get_parameter_data_type_extent_and_description",
    [](Model& self, int const index) {
      DataType dataType;
      int extent;
      std::string const * description;

      int error = self.GetParameterDataTypeExtentAndDescription(
        index, &dataType, &extent, &description);
      py::tuple re(4);
      re[0] = dataType;
      re[1] = extent;
      re[2] = *description;
      re[3] = error;
      return re;
    },
    py::arg("index"),
    "Return(DataType, extent, description, error)"
  )

  .def("get_parameter_int",
    [](Model& self, int const parameterIndex, int const arrayIndex) {
      int parameterValue;
      int error = self.GetParameter(parameterIndex, arrayIndex, &parameterValue);

      py::tuple re(2);
      re[0] = parameterValue;
      re[1] = error;
      return re;
    },
    py::arg("parameterIndex"),
    py::arg("arrayIndex"),
    "Return(parameterValue, error)"
  )

  .def("get_parameter_double",
    [](Model& self, int const parameterIndex, int const arrayIndex) {
      double parameterValue;
      int error = self.GetParameter(parameterIndex, arrayIndex, &parameterValue);

      py::tuple re(2);
      re[0] = parameterValue;
      re[1] = error;
      return re;
    },
    py::arg("parameterIndex"),
    py::arg("arrayIndex"),
    "Return(parameterValue, error)"
  )

  // overloaded function
  .def("set_parameter",
      (int (Model::*)(int const, int const, int const)) &Model::SetParameter)
  .def("set_parameter",
      (int (Model::*)(int const, int const, double const)) &Model::SetParameter)

  .def("__repr__", &Model::String)

  .def("set_log_id", &Model::SetLogID)

  .def("push_log_verbosity", &Model::PushLogVerbosity)

  .def("pop_log_verbosity", &Model::PopLogVerbosity);


  // module functions

  module.def("create",
    [](Numbering const numbering,
      LengthUnit const requestedLengthUnit,
      EnergyUnit const requestedEnergyUnit,
      ChargeUnit const requestedChargeUnit,
      TemperatureUnit const requestedTemperatureUnit,
      TimeUnit const requestedTimeUnit,
      std::string const & modelName
    ) {

      Model * mo;
      int requestedUnitsAccepted;
      int error = Model::Create(numbering, requestedLengthUnit,
          requestedEnergyUnit, requestedChargeUnit, requestedTemperatureUnit,
          requestedTimeUnit, modelName, &requestedUnitsAccepted, &mo);

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
    "Return(requestedUnitsAccepted, Model, error)"
  );

  module.def("destroy",
    [](Model * self) {
      Model::Destroy(&self);
    }
  );

}


