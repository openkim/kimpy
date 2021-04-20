
#include <pybind11/pybind11.h>

#include <iostream>
#include <string>

#include "KIM_SemVer.hpp"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(sem_ver, module)
{
  module.doc() = "Python binding to KIM_SymVer.hpp";

  module.def("get_sem_ver", []() {
    std::string const & version = SEM_VER::GetSemVer();

    return version;
  }, R"pbdoc(
     Get the KIM-API complete Semantic Version string.

     Returns:
         str: version
     )pbdoc");

  module.def("is_less_than",
      [](std::string const &version_a, std::string const &version_b) {
    int is_less_than;

    int error = SEM_VER::IsLessThan(version_a, version_b, &is_less_than);
    if (error == 1)
    {
      throw std::runtime_error(
          "ParseSemVer (Parse Semantic Version) returns error for lhs or rhs!");
    }

    return is_less_than;
  }, R"pbdoc(
     Compare two Semantic Version strings.

     Returns:
         int: is_less_than
     )pbdoc",
     py::arg("version_a"),
     py::arg("version_b"));

  module.def("parse_sem_ver", [](std::string const &version) {
    int major;
    int minor;
    int patch;
    std::string prerelease;
    std::string build_metadata;

    int error = SEM_VER::ParseSemVer(
        version, &major, &minor, &patch, &prerelease, &build_metadata);
    if (error == 1)
    {
      throw std::runtime_error(
          "minor and/or patch are missing!  or\n"
          "major number has a leading zero or is not a valid integer!  or\n"
          "minor number has a leading zero or is not a valid integer!  or\n"
          "patch number has a leading zero or is not a valid integer!  or\n"
          "the prerelease string is invalid!  or\n"
          "the build metadata string is invalid!");
    }

    py::tuple re(5);
    re[0] = major;
    re[1] = minor;
    re[2] = patch;
    re[3] = prerelease;
    re[4] = build_metadata;
    return re;
  }, R"pbdoc(
     Parse Semantic Version string into its six components.

     Returns:
         int, int, int, str, str: major, minor, patch, prerelease,
             build_metadata
     )pbdoc",
     py::arg("version"));
}
