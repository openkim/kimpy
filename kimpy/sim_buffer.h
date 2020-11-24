#ifndef SIM_BUFFER_
#define SIM_BUFFER_

#include <pybind11/pybind11.h>

#include <vector>

namespace py = pybind11;

struct SimBuffer {
  std::vector<py::dict *> callbacks;
};

#endif  // SIM_BUFFER_
