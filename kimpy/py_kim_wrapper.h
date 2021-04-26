#ifndef PY_KIM_WRAPPER_H_
#define PY_KIM_WRAPPER_H_

#include <memory>

#include "KIM_ComputeArguments.hpp"
#include "KIM_Model.hpp"
#include "sim_buffer.h"

/**
 * @brief A convenient wrapper over the \c Model and \c ComputeArguments
 * classes.
 *
 * \c Model class provides the primary interface to a %KIM API Model object
 * and is meant to be used by simulators. The incomplete class is not exposed
 * to the user and it is hidden behind a forwarding facade. This class has a
 * private constructor & destructor. It is also creating a new
 * \c ComputeArguments object for the Model object.
 *
 * The naive bindings via pybind11 and using ``py::nodelete`` class requires
 * users to manullay create and destroy the objects. Which could very easily
 * lead to memory leaks/use-after-free (segmentation fault) errors.
 *
 * By using the current wrapper class we remove the need for user requirement
 * to explicitly destroy the object. Also provide information that allows
 * the Python garbage collection to take care of the objects memory management.
 */


class PyModel : public std::enable_shared_from_this<PyModel>
{
 public:
  KIM::Model * _model {nullptr};

  PyModel() = default;
  ~PyModel()
  {
    if (_model) { KIM::Model::Destroy(&_model); }
  }
};


class PyComputeArguments :
    public std::enable_shared_from_this<PyComputeArguments>
{
 public:
  KIM::ComputeArguments * _compute_arguments {nullptr};

  std::shared_ptr<PyModel> _py_model {nullptr};

  PyComputeArguments() = default;
  PyComputeArguments(std::shared_ptr<PyModel> py_model) : _py_model(py_model) {}

  ~PyComputeArguments()
  {
    if (_py_model)
    {
      if (_compute_arguments)
      {
        SimBuffer * sim_buffer;

        _compute_arguments->GetSimulatorBufferPointer((void **) &sim_buffer);

        if (sim_buffer)
        {
          for (std::size_t i = 0; i < sim_buffer->callbacks.size(); ++i)
          {
            if (sim_buffer->callbacks[i])
            {
              delete sim_buffer->callbacks[i];
              sim_buffer->callbacks[i] = nullptr;
            }
          }

          delete sim_buffer;
          sim_buffer = nullptr;
        }

        auto model = _py_model->_model;
        model->ComputeArgumentsDestroy(&_compute_arguments);
      }
    }
  }
};

#endif  // PY_KIM_WRAPPER_H_
