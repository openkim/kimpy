#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/embed.h>
#include <pybind11/iostream.h>

#include "KIM_SimulatorHeaders.hpp"
//@ TODO replace the header with the following. (need to solve forward declaration)
//#include "KIM_Model.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace KIM;


PYBIND11_MODULE(model, module) {
  module.doc() = "Python binding to ... ";

  // C++ class constructor and destructor are private, and as a result, simple ways
  // to construct py::init() does not exist.
  // We use factory function to initialize instance.
  // std::unique_ptr<Model, py::nodelete> avoids calling the destructor
  // of the C++ class. In this case, it is crucial that instances are deallocated
  // on the C++ side to avoid memory leaks. Model_Destroy should do the work.
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
    [](Model& mo) {
      double influenceDistance;
      mo.GetInfluenceDistance(&influenceDistance);
      return influenceDistance;
    }
  )

  .def("get_neighbor_list_cutoffs",
    [](Model& mo) {
      int numberOfCutoffs;
      double const * cutoff_ptr;
      mo.GetNeighborListCutoffsPointer(&numberOfCutoffs, &cutoff_ptr);

      py::array_t<double, py::array::c_style> cutoffs (
          {static_cast<size_t> (numberOfCutoffs)});
      auto cut = cutoffs.mutable_data(0);
      for(int i=0; i<numberOfCutoffs; i++) {
        cut[i] = cutoff_ptr[i];
      }

      return cutoffs;
    }
  )

  .def("get_units",
    [](Model& mo) {
      LengthUnit lengthUnit;
      EnergyUnit energyUnit;
      ChargeUnit chargeUnit;
      TemperatureUnit temperatureUnit;
      TimeUnit timeUnit;
      mo.GetUnits(&lengthUnit, &energyUnit, &chargeUnit, &temperatureUnit, &timeUnit);

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

  // wrapper to call constructor of ComputeArguments to deal with `error`
  .def("compute_arguments_create",
    [](Model& mo) {
      auto locals = py::dict( "mo"_a = mo);

      // embed python code
      py::exec(R"(
        from kimpy import compute_arguments
        import numpy as np

        mo = locals()['mo']
        e = np.array([0], dtype='intc')

        instance = compute_arguments.ComputeArguments(mo, e)
        error = e[0]
      )", py::globals(), locals);

      // cannot cast, because ~ComputeArguments() is private
      //ComputeArguments computeArguments = locals["instance"].cast<ComputeArguments>();
      auto computeArguments = locals["instance"];
      int error = locals["error"].cast<int>();

      py::tuple re(2);
      re[0] = computeArguments;
      re[1] = error;
      return re;
    },
    "Return(ComputeArguments, error)"
  )

  .def("compute_arguments_destroy",
    [](Model& mo, ComputeArguments * computeArguments) {
      int error = mo.ComputeArgumentsDestroy(&computeArguments);
      return error;
    }
  )

  .def("compute", &Model::Compute)

  .def("clean_influence_distance_and_cutoffs_then_refresh_model",
      &Model::ClearInfluenceDistanceAndCutoffsThenRefreshModel)

  .def("get_species_support_and_code",
    [](Model& mo, SpeciesName const speciesName) {
      int speciesIsSupported;
      int code;
      int error = mo.GetSpeciesSupportAndCode(speciesName, &speciesIsSupported, &code);

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
    [](Model& mo) {
      int numberOfParameters;
      mo.GetNumberOfParameters(&numberOfParameters);
      return numberOfParameters;
    }
  )

  .def("get_parameter_data_type_extend_and_description",
    [](Model& mo, int const index) {
      DataType dataType;
      int extent;
      std::string const * description;

      int error = mo.GetParameterDataTypeExtentAndDescription(
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

  .def("__repr__", &Model::String);


  // module functions

  // wrapper to call constructor of Model to deal with `error` and
  // `requestedUnitsAccepted`
  module.def("create",
    [](Numbering const numbering,
      LengthUnit const requestedLengthUnit,
      EnergyUnit const requestedEnergyUnit,
      ChargeUnit const requestedChargeUnit,
      TemperatureUnit const requestedTemperatureUnit,
      TimeUnit const requestedTimeUnit,
      std::string const & modelName
    ) {

      auto locals = py::dict(
        "numbering"_a = numbering,
        "requestedLengthUnit"_a = requestedLengthUnit,
        "requestedEnergyUnit"_a = requestedEnergyUnit,
        "requestedChargeUnit"_a = requestedChargeUnit,
        "requestedTemperatureUnit"_a = requestedTemperatureUnit,
        "requestedTimeUnit"_a = requestedTimeUnit,
        "modelName"_a = modelName
      );

      // embed python code
      py::exec(R"(
        from kimpy import model
        import numpy as np

        numbering = locals()['numbering']
        requestedLengthUnit = locals()['requestedLengthUnit']
        requestedEnergyUnit = locals()['requestedEnergyUnit']
        requestedChargeUnit = locals()['requestedChargeUnit']
        requestedTemperatureUnit = locals()['requestedTemperatureUnit']
        requestedTimeUnit = locals()['requestedTimeUnit']
        modelName = locals()['modelName']

        requestedUnitsAccepted = np.array([0], dtype='intc')
        e = np.array([0], dtype='intc')
        instance = model.Model(
          numbering,
          requestedLengthUnit,
          requestedEnergyUnit,
          requestedChargeUnit,
          requestedTemperatureUnit,
          requestedTimeUnit,
          modelName,
          requestedUnitsAccepted,
          e
        )
        error = e[0]
        rua = requestedUnitsAccepted[0]
      )", py::globals(), locals);

      // cannot cast, because ~Model() is private
      //Model mo = locals["instance"].cast<Model>();
      auto mo = locals["instance"];
      int requestedUnitsAccepted = locals["rua"].cast<int>();
      int error = locals["error"].cast<int>();

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
    [](Model * mo) {
      Model::Destroy(&mo);
    }
  );

}

