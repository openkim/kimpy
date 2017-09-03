#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <string>
#include <iostream>

#include "KIM_API_C.h"
#include "KIM_API_status.h"

namespace py = pybind11;


PYBIND11_PLUGIN(openkim) {
  py::module m("openkim", "ASE calculator based on OpenKIM");

  m.def("get_version",
    []() {
      const char* version;
      int status =  KIM_API_get_version(&version);
      py::tuple t(2);
      t[0] = version;
      t[1] = status;
      return t;
    }
  );

  m.def("file_init",
    [](char* simkimfile, char* modelname) {
      void* kimmdl;
      int status =  KIM_API_file_init(&kimmdl, simkimfile, modelname);
      py::tuple t(2);
      t[0] = kimmdl;
      t[1] = status;
      return t;
    },
    py::arg("simkimfile"),
    py::arg("modelname")
  );

  m.def("model_info",
    [](void* kimmdl, char* modelname) {
      int status =  KIM_API_model_info(kimmdl, modelname);
      return status;
    },
    py::arg("kimmdl"),
    py::arg("modelname")
  );

  m.def("string_init",
    [](char* siminputstring, char* modelname) {
      void* kimmdl;
      int status =  KIM_API_string_init(&kimmdl, siminputstring, modelname);
      py::tuple t(2);
      t[0] = kimmdl;
      t[1] = status;
      return t;
    },
    py::arg("siminputstring"),
    py::arg("modelname")
  );

  m.def("allocate",
    [](void* kimmdl, int nparts, int nspecies) {
      int status;
      KIM_API_allocate(kimmdl, nparts, nspecies, &status);
      return status;
    },
    py::arg("kimmdl"),
    py::arg("nparts"),
    py::arg("nspecies")
  );

  m.def("free",
    [](void* kimmdl) {
      int status;
      KIM_API_free(&kimmdl, &status);
      return status;
    },
    py::arg("kimmdl")
  );

  m.def("print",
    [](void* kimmdl) {
      int status;
      KIM_API_print(kimmdl, &status);
      return status;
    },
    py::arg("kimmdl")
  );

  m.def("model_compute",
    [](void* kimmdl) {
      int status = KIM_API_model_compute(kimmdl);
      return status;
    },
    py::arg("kimmdl")
  );

  m.def("model_init",
    [](void* kimmdl) {
      int status = KIM_API_model_init(kimmdl);
      return status;
    },
    py::arg("kimmdl")
  );

  m.def("model_destroy",
    [](void* kimmdl) {
      int status = KIM_API_model_destroy(kimmdl);
      return status;
    },
    py::arg("kimmdl")
  );

  m.def("model_reinit",
    [](void* kimmdl) {
      int status = KIM_API_model_reinit(kimmdl);
      return status;
    },
    py::arg("kimmdl")
  );

  m.def("get_model_kim_str_len",
    [](char* modelname) {
      int modelStrLen;
      int status =  KIM_API_get_model_kim_str_len(modelname, &modelStrLen);
      py::tuple t(2);
      t[0] = modelStrLen;
      t[1] = status;
      return t;
    },
    py::arg("modelname")
  );

  m.def("get_model_kim_str",
    [](char* modelname) {
      char* kimString;
      int status =  KIM_API_get_model_kim_str(modelname, &kimString);
      py::tuple t(2);
      t[0] = kimString;
      t[1] = status;
      return t;
    },
    py::arg("modelname")
  );

  m.def("get_num_model_species",
    [](void* kimmdl) {
      int numberSpecies;
      int maxStrLen;
      int status =  KIM_API_get_num_model_species(kimmdl, &numberSpecies, &maxStrLen);
      py::tuple t(3);
      t[0] = numberSpecies;
      t[1] = maxStrLen;
      t[2] = status;
      return t;
    },
    py::arg("kimmdl")
  );

  m.def("get_model_species",
    [](void* kimmdl, int index) {
      const char* speciesString;
      int status =  KIM_API_get_model_species(kimmdl, index, &speciesString);
      py::tuple t(2);
      t[0] = speciesString;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    py::arg("index")
  );

  m.def("get_species_code",
    [](void* kimmdl, char* species) {
      int status;
      int code =  KIM_API_get_species_code(kimmdl, species, &status);
      py::tuple t(2);
      t[0] = code;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    py::arg("species")
  );

  m.def("set_species_code",
    [](void* kimmdl, char* species, int code) {
      int status;
      KIM_API_set_species_code(kimmdl, species, code, &status);
      return status;
    },
    py::arg("kimmdl"),
    py::arg("species"),
    py::arg("code")
  );

  m.def("get_num_params",
    [](void* kimmdl) {
      int numberParameters;
      int maxStrLen;
      int status = KIM_API_get_num_params(kimmdl, &numberParameters, &maxStrLen);
      py::tuple t(3);
      t[0] = numberParameters;
      t[1] = maxStrLen;
      t[2] = status;
      return t;
    },
    py::arg("kimmdl")
  );

  m.def("get_parameter",
    [](void* kimmdl, int index) {
      const char* parameterString;
      int status = KIM_API_get_parameter(kimmdl, index, &parameterString);
      py::tuple t(2);
      t[0] = parameterString;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    py::arg("index"),
    "Return (parameterString, status)"
  );

  m.def("get_num_free_params",
    [](void* kimmdl) {
      int numberFreeParameters;
      int maxStrLen;
      int status = KIM_API_get_num_free_params(kimmdl, &numberFreeParameters, &maxStrLen);
      py::tuple t(3);
      t[0] = numberFreeParameters;
      t[1] = maxStrLen;
      t[2] = status;
      return t;
    },
    py::arg("kimmdl"),
    "Return (numberFreeParameters, maxStringLength, status)"
  );

  m.def("get_free_parameter",
    [](void* kimmdl, int index) {
      const char* freeParameterString;
      int status = KIM_API_get_free_parameter(kimmdl, index, &freeParameterString);
      py::tuple t(2);
      t[0] = freeParameterString;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    py::arg("index"),
    "Return (freeParameterString, status)"
  );

  m.def("get_NBC_method",
    [](void* kimmdl) {
      const char* NBC_String;
      int status = KIM_API_get_NBC_method(kimmdl, &NBC_String);
      py::tuple t(2);
      t[0] = NBC_String;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    "Return (NBC_String, status)"
  );

  m.def("get_status_msg",
    [](int status_code) {
      const char* status_msg;
      int status = KIM_API_get_status_msg(status_code, &status_msg);
      py::tuple t(2);
      t[0] = status_msg;
      t[1] = status;
      return t;
    },
    py::arg("status_code"),
    "Return (status_msg, status)"
  );

//TODO, let fl and ln be determined in this file, not python
  m.def("report_error",
    [](int ln, char* fl, char* usermsg, int error) {
      int status =  KIM_API_report_error(ln, fl, usermsg, error);
      return status;
    },
    py::arg("line"),
    py::arg("file"),
    py::arg("usermsg"),
    py::arg("status_code")
  );

  m.def("get_model_index_shift",
    [](void* kimmdl) {
      int shift = KIM_API_get_model_index_shift(kimmdl);
      return shift;
    },
    py::arg("kimmdl")
  );

  m.def("is_half_neighbors",
    [](void* kimmdl) {
      int status;
      int is_half = KIM_API_is_half_neighbors(kimmdl, &status);
      py::tuple t(2);
      t[0] = is_half;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    "Return (is_half, status)"
  );

// TODO tempalte this and set_data_double
  m.def("set_data_int",
    [](void* kimmdl, char* name, int size, py::array_t<int> data) {
      int status = KIM_API_set_data(kimmdl, name, static_cast<intptr_t> (size),
          static_cast<void *> (data.mutable_data(0)));
      return status;
    },
    py::arg("kimmdl"),
    py::arg("name"),
    py::arg("size"),
    py::arg("data").noconvert()
  );

  m.def("set_data_double",
    [](void* kimmdl, char* name, int size, py::array_t<double> data) {
      int status = KIM_API_set_data(kimmdl, name, static_cast <intptr_t>(size),
          static_cast<void *> (data.mutable_data(0)));
      return status;
    },
    py::arg("kimmdl"),
    py::arg("name"),
    py::arg("size"),
    py::arg("data").noconvert()
  );

// TODO tempalte this and get_data_double
  m.def("get_data_int",
    [](void* kimmdl, char* name) {
      int status;
      size_t size = static_cast<size_t> ( KIM_API_get_size(kimmdl, name, &status) );
      if (status != KIM_STATUS_OK) {
        KIM_API_report_error(__LINE__, __FILE__, "KIM_API_get_size", status);
        //TODO cannot return here, otherwise it confilts with `return t' below
        //return status;
      }
      void* dp = KIM_API_get_data(kimmdl, name, &status);

      // pack dp into a buffer that numpy array can understand
      auto data = py::array (py::buffer_info (
        dp,   // data pointer
        sizeof(int),  // size of one element
        py::format_descriptor<int>::format(),  //Python struct-style format descriptor
        1,  // dimension
        {size},  // size of each dimension
        {sizeof(int) * size}  // stride of each dimension
      ));

      py::tuple t(2);
      t[0] = data;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    py::arg("name"),
    "Return (data, status)"
  );

  m.def("get_data_double",
    [](void* kimmdl, char* name) {
      int status;
      size_t size = static_cast<size_t> ( KIM_API_get_size(kimmdl, name, &status) );
      if (status != KIM_STATUS_OK) {
        KIM_API_report_error(__LINE__, __FILE__, "KIM_API_get_size", status);
        //TODO cannot return here, otherwise it confilts with `return t' below
        //return status;
      }
      void* dp = KIM_API_get_data(kimmdl, name, &status);

      // pack dp into a buffer that numpy array can understand
      auto data = py::array (py::buffer_info (
        dp,   // data pointer
        sizeof(double),  // size of one element
        py::format_descriptor<double>::format(),  //Python struct-style format descriptor
        1,  // dimension
        {size},  // size of each dimension
        {sizeof(double) * size}  // stride of each dimension
      ));

      py::tuple t(2);
      t[0] = data;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    py::arg("name"),
    "Return (data, status)"
  );

  m.def("set_compute",
    [](void* kimmdl, char* name, int flag) {
      int status;
      KIM_API_set_compute(kimmdl, name, flag, &status);
      return status;
    },
    py::arg("kimmdl"),
    py::arg("name"),
    py::arg("flag")
  );

  m.def("get_compute",
    [](void* kimmdl, char* name) {
      int status;
      int flag = KIM_API_get_compute(kimmdl, name, &status);
      py::tuple t(2);
      t[0] = flag;
      t[1] = status;
      return t;
    },
    py::arg("kimmdl"),
    py::arg("name")
  );


  return m.ptr();
}

