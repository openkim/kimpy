#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <memory>
#include <string>

#include "KIM_SimulatorHeaders.hpp"
#include "py_kim_wrapper.h"
#include "sim_buffer.h"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(model, module)
{
  module.doc() = "Python binding to KIM_Model.hpp";

  py::class_<PyModel, std::shared_ptr<PyModel> > cl(module, "PyModel");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init([](Numbering const &numbering,
                     LengthUnit const &requested_length_unit,
                     EnergyUnit const &requested_energy_unit,
                     ChargeUnit const &requested_charge_unit,
                     TemperatureUnit const &requested_temperature_unit,
                     TimeUnit const &requested_time_unit,
                     std::string const &model_name,
                     py::array_t<int> requested_units_accepted) {
    int * kim_requested_units_accepted
        = requested_units_accepted.mutable_data(0);

    std::shared_ptr<PyModel> model(new PyModel());

    int error = Model::Create(numbering,
                              requested_length_unit,
                              requested_energy_unit,
                              requested_charge_unit,
                              requested_temperature_unit,
                              requested_time_unit,
                              model_name,
                              kim_requested_units_accepted,
                              &model->_model);
    if (error == 1)
    {
      throw std::runtime_error("Unable to create a new KIM-API Model object!");
    }

    return model;
    }))
    .def("is_routine_present",
         [](PyModel &self, ModelRoutineName const &model_routine_name) {
    int present;
    int required;

    int error = self._model->IsRoutinePresent(
        model_routine_name, &present, &required);
    if (error == 1)
    {
      throw std::runtime_error("model_routine_name = "
                               + model_routine_name.ToString() + "is unknown!");
    }

    py::tuple re(2);
    re[0] = present;
    re[1] = required;
    return re;
    }, R"pbdoc(
       Determine presence and required status of the given model_routine_name.

       Returns:
           int, int: present, required
       )pbdoc",
       py::arg("model_routine_name"))
    .def("get_influence_distance", [](PyModel &self) {
    double influence_distance;

    self._model->GetInfluenceDistance(&influence_distance);

    return influence_distance;
    }, R"pbdoc(
       Get the model's influence distance.

       Returns:
           float: influence_distance
       )pbdoc")
    .def("get_neighbor_list_cutoffs_and_hints", [](PyModel &self) {
    int number_of_cutoffs;
    double const * cutoff_ptr;
    int const * kim_model_not_request_neighbors_of_noncontributing_particles;

    self._model->GetNeighborListPointers(
        &number_of_cutoffs,
        &cutoff_ptr,
        &kim_model_not_request_neighbors_of_noncontributing_particles);

    py::array_t<double, py::array::c_style> cutoffs(number_of_cutoffs);
    py::array_t<int, py::array::c_style>
        model_not_request_neighbors_of_noncontributing_particles(
            number_of_cutoffs);

    auto cut = cutoffs.mutable_data(0);
    auto pnh
        = model_not_request_neighbors_of_noncontributing_particles.mutable_data(
            0);

    for (int i = 0; i < number_of_cutoffs; i++)
    {
      cut[i] = cutoff_ptr[i];
      pnh[i] = kim_model_not_request_neighbors_of_noncontributing_particles[i];
    }

    py::tuple re(2);
    re[0] = cutoffs;
    re[1] = model_not_request_neighbors_of_noncontributing_particles;
    return re;
    }, R"pbdoc(
       Get the model's neighbor list information.

       Returns:
           1darray, 1darray: cutoffs,
               model_not_request_neighbors_of_noncontributing_particles
       )pbdoc")
    .def("get_units", [](PyModel &self) {
    LengthUnit length_unit;
    EnergyUnit energy_unit;
    ChargeUnit charge_unit;
    TemperatureUnit temperature_unit;
    TimeUnit time_unit;

    self._model->GetUnits(&length_unit,
                          &energy_unit,
                          &charge_unit,
                          &temperature_unit,
                          &time_unit);

    py::tuple re(5);
    re[0] = length_unit;
    re[1] = energy_unit;
    re[2] = charge_unit;
    re[3] = temperature_unit;
    re[4] = time_unit;
    return re;
    }, R"pbdoc(
       Get the model's base unit values.

       Returns:
           LengthUnit, EnergyUnit, ChargeUnit, TemperatureUnit, TimeUnit:
               length_unit, energy_unit, charge_unit, temperature_unit,
               time_unit
       )pbdoc")
    .def("compute_arguments_create", [](std::shared_ptr<PyModel> self) {
    std::shared_ptr<PyComputeArguments> py_compute_arguments(
        new PyComputeArguments(self));

    int error = self->_model->ComputeArgumentsCreate(
        &py_compute_arguments->_compute_arguments);
    if (error == 1)
    {
      throw std::runtime_error("Unable to create a new compute_arguments "
                               "object for the model object!");
    }

    return py_compute_arguments;
    }, R"pbdoc(
       Create a new compute_arguments object for the model object.

       Returns:
           ComputeArguments: compute_arguments
       )pbdoc")
    .def("compute",
         [](PyModel &self,
            std::shared_ptr<PyComputeArguments> compute_arguments,
            bool release_GIL) {
    ComputeArguments const * const kim_compute_arguments
        = compute_arguments->_compute_arguments;

    int error;
    if (release_GIL)
    {
      py::gil_scoped_release release;
      error = self._model->Compute(kim_compute_arguments);
    }
    else
    {
      error = self._model->Compute(kim_compute_arguments);
    }
    if (error == 1)
    {
      throw std::runtime_error(
          "the model does not provide an Extension routine!  or\n"
          "the model's Extension routine returns error!");
    }
    }, "Call the model's Compute routine.",
       py::arg("compute_arguments"),
       py::arg("release_GIL") = false)
    .def("clear_then_refresh", [](PyModel &self) {
    self._model->ClearThenRefresh();
    }, "Clear influence distance and neighbor list pointers and refresh "
       "model object after parameter changes.")
    .def("write_parameterized_model",
         [](PyModel &self,
            std::string const &path,
            std::string const &model_name) {
    int error = self._model->WriteParameterizedModel(path, model_name);
    if (error == 1)
    {
      throw std::runtime_error(
          "the model object is not a parameterized model!  or\n"
          "the model_name is not a valid C identifier!  or\n"
          "the model's WriteParameterizedModel routine returns error!");
    }
    }, "Call the model's WriteParameterizedModel routine.",
       py::arg("path"),
       py::arg("model_name"))
    .def("get_species_support_and_code",
         [](PyModel &self, SpeciesName const &species_name) {
    int species_is_supported;
    int code = -1;

    int error = self._model->GetSpeciesSupportAndCode(
        species_name, &species_is_supported, &code);
    if (error == 1)
    {
      throw std::runtime_error("species_name = " + species_name.ToString()
                               + " is unknown!");
    }

    py::tuple re(2);
    re[0] = species_is_supported;
    re[1] = code;
    return re;
    }, R"pbdoc(
       Get the model's support and code for the requested species_name.

       Returns:
           int, int: species_is_supported, code
       )pbdoc",
       py::arg("species_name"))
    .def("get_number_of_parameters", [](PyModel &self) {
    int number_pf_parameters;

    self._model->GetNumberOfParameters(&number_pf_parameters);

    return number_pf_parameters;
    }, R"pbdoc(
       Get the number of parameter arrays provided by the model.

       Returns:
           int: number_pf_parameters
       )pbdoc")
    .def("get_parameter_metadata",
         [](PyModel &self, int const parameter_index) {
    DataType data_type;
    int extent;
    std::string const * name;
    std::string const * description;

    int error = self._model->GetParameterMetadata(
        parameter_index, &data_type, &extent, &name, &description);
    if (error == 1) { throw std::runtime_error("parameter_index is invalid!"); }

    py::tuple re(4);
    re[0] = data_type;
    re[1] = extent;
    re[2] = *name;
    re[3] = *description;
    return re;
    }, R"pbdoc(
       Get the metadata associated with one of the models's parameter arrays.

       Returns:
           DataType, int, str, str: data_type, extent, name, description
       )pbdoc",
       py::arg("parameterIndex"))
    .def("get_parameter_int",
         [](PyModel &self, int const parameter_index, int const array_index) {
    int parameter_value;

    int error = self._model->GetParameter(
        parameter_index, array_index, &parameter_value);
    if (error == 1)
    {
      throw std::runtime_error(
          "parameter_index = " + std::to_string(parameter_index)
          + " is invalid!  or\nthe specified parameter "
            "and parameter_value are of different data types!  or\n"
            "array_index = "
          + std::to_string(array_index) + "is invalid!");
    }

    return parameter_value;
    }, R"pbdoc(
       Get an int parameter value from the Model.

       Returns:
           int: parameter_value
       )pbdoc",
       py::arg("parameter_index"),
       py::arg("array_index"))
    .def("get_parameter_double",
         [](PyModel &self, int const parameter_index, int const array_index) {
    double parameter_value;

    int error = self._model->GetParameter(
        parameter_index, array_index, &parameter_value);
    if (error == 1)
    {
      throw std::runtime_error(
          "parameter_index = " + std::to_string(parameter_index)
          + " is invalid!  or\nthe specified parameter "
            "and parameter_value are of different data types!  or\n"
            "array_index = "
          + std::to_string(array_index) + " is invalid!");
    }

    return parameter_value;
    }, R"pbdoc(
       Get a double parameter value from the Model.

       Returns:
           float: parameter_value
       )pbdoc",
       py::arg("parameter_index"),
       py::arg("array_index"))
    // overloaded function
    .def("set_parameter",
          [](PyModel &self,
             int const parameter_index,
             int const array_index,
             int const parameter_value) {
    int error = self._model->SetParameter(
        parameter_index, array_index, parameter_value);
    if (error == 1)
    {
      throw std::runtime_error(
          "parameter_index = " + std::to_string(parameter_index)
          + " is invalid!  or\nthe specified parameter "
            "and parameter_value are of different data types!  or\n"
            "array_index = "
          + std::to_string(array_index) + " is invalid!");
    }
    }, "Set an int parameter value for the Model.",
       py::arg("parameter_index"),
       py::arg("array_index"),
       py::arg("parameter_value"))
    .def("set_parameter",
          [](PyModel &self,
             int const parameter_index,
             int const array_index,
             double const parameter_value) {
    int error = self._model->SetParameter(
        parameter_index, array_index, parameter_value);
    if (error == 1)
    {
      throw std::runtime_error(
          "parameter_index = " + std::to_string(parameter_index)
          + " is invalid!  or\nthe specified parameter "
            "and parameter_value are of different data types!  or\n"
            "array_index = "
          + std::to_string(array_index) + " is invalid!");
    }
    }, "Set an int parameter value for the Model.",
       py::arg("parameter_index"),
       py::arg("array_index"),
       py::arg("parameter_value"))
    .def("__repr__", [](PyModel &self) {
    return self._model->ToString();
    },"A string representing the internal state of the model object.")
    .def("set_log_id", [](PyModel &self, std::string const &log_id) {
    self._model->SetLogID(log_id);
    }, "Set the identity of the Log object associated "
       "with the model object.",
       py::arg("log_id"))
    .def("push_log_verbosity",
         [](PyModel &self, LogVerbosity const &log_verbosity) {
    self._model->PushLogVerbosity(log_verbosity);
    }, "Push a new LogVerbosity onto the Model object's Log object "
       "verbosity stack.",
       py::arg("log_verbosity"))
    .def("pop_log_verbosity", [](PyModel &self) {
    self._model->PopLogVerbosity();
    }, "Pop a LogVerbosity from the Model object's Log object "
       "verbosity stack.");

  // module functions

  module.def("create",
             [](Numbering const &numbering,
                LengthUnit const &requested_length_unit,
                EnergyUnit const &requested_energy_unit,
                ChargeUnit const &requested_charge_unit,
                TemperatureUnit const &requested_temperature_unit,
                TimeUnit const &requested_time_unit,
                std::string const &model_name) {
    int requested_units_accepted;

    std::shared_ptr<PyModel> model(new PyModel());

    int error = Model::Create(numbering,
                              requested_length_unit,
                              requested_energy_unit,
                              requested_charge_unit,
                              requested_temperature_unit,
                              requested_time_unit,
                              model_name,
                              &requested_units_accepted,
                              &model->_model);
    if (error == 1)
    {
      throw std::runtime_error("Unable to create a new KIM-API Model object!");
    }

    py::tuple re(2);
    re[0] = requested_units_accepted;
    re[1] = model;
    return re;
  }, R"pbdoc(
     Create a new KIM-API Model object.

     Returns:
         int, Model: requested_units_accepted, model
     )pbdoc",
     py::arg("numbering"),
     py::arg("requested_length_unit"),
     py::arg("requested_energy_unit"),
     py::arg("requested_charge_unit"),
     py::arg("requested_temperature_unit"),
     py::arg("requested_time_unit"),
     py::arg("model_name"));
}
