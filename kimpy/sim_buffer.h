#ifndef SIM_BUFFER_
#define SIM_BUFFER_

#include <pybind11/pybind11.h>
namespace py = pybind11;

typedef struct SimBuffer
{
  std::vector<py::dict *> callbacks;
} SimBuffer;

#endif  // SIM_BUFFER_
