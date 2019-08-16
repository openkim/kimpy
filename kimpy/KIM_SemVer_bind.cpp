#include "KIM_SemVer.hpp"
#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(sem_ver, module)
{
  module.doc() = "Python binding to KIM_SymVer.hpp";

  module.def("get_sem_ver", []() {
    std::string const & version = SEM_VER::GetSemVer();
    return version;
  });

  module.def(
      "is_less_than",
      [](std::string const & versionA, std::string const & versionB) {
        int isLessThan;
        int error = SEM_VER::IsLessThan(versionA, versionB, &isLessThan);

        py::tuple re(2);
        re[0] = isLessThan;
        re[1] = error;
        return re;
      },
      py::arg("versionA"),
      py::arg("versionB"),
      "Return(IsLessThan, error)");

  module.def(
      "parse_sem_ver",
      [](std::string const & version) {
        int major;
        int minor;
        int patch;
        std::string prerelease;
        std::string buildMetadata;
        int error = SEM_VER::ParseSemVer(
            version, &major, &minor, &patch, &prerelease, &buildMetadata);

        py::tuple re(6);
        re[0] = major;
        re[1] = minor;
        re[2] = patch;
        re[3] = prerelease;
        re[4] = buildMetadata;
        re[5] = error;
        return re;
      },
      py::arg("version"),
      "Return(major, minor, patch, prerelease, buildMetadata, error)");
}
