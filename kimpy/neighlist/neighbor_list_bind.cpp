#include "neighbor_list.h"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

namespace py = pybind11;


namespace
{
struct PyNeighListDestroy
{
  void operator()(NeighList * neighList) const { nbl_clean(&neighList); }
};
}  // namespace


PYBIND11_MODULE(neighlist, module)
{
  module.doc() = "Python binding to neighbor list.";

  py::class_<NeighList, std::unique_ptr<NeighList, PyNeighListDestroy> >(
      module, "NeighList", py::module_local())
      .def(py::init());

  module
      .def(
          "create",
          []() {
    NeighList * neighList;

    nbl_initialize(&neighList);

    return std::unique_ptr<NeighList, PyNeighListDestroy>(std::move(neighList));
          }, R"pbdoc(
             Create a new NeighList object.

             Returns:
                 NeighList: neighList
             )pbdoc"
          );

      module
      .def(
          "initialize",
          []() {
    NeighList * neighList;

    nbl_initialize(&neighList);

    return std::unique_ptr<NeighList, PyNeighListDestroy>(std::move(neighList));
          }, R"pbdoc(
             Create a new NeighList object.

             Returns:
                 NeighList: neighList
             )pbdoc"
          );

      module.def(
          "build",
          [](NeighList * const neighList,
             py::array_t<double> coords,
             double const influenceDistance,
             py::array_t<double> cutoffs,
             py::array_t<int> need_neigh) {
            int Natoms_1 = static_cast<int>(coords.size() / 3);
            int Natoms_2 = static_cast<int>(need_neigh.size());

            if (Natoms_1 != Natoms_2)
            {
              throw std::runtime_error(
                  "\"coords\" size and \"need_neigh\" size do not match!");
            }

            int Natoms = Natoms_1 <= Natoms_2 ? Natoms_1 : Natoms_2;

            double const * c = coords.data();
            int numberOfCutoffs = static_cast<int>(cutoffs.size());
            double const * pcutoffs = cutoffs.data();
            int const * nn = need_neigh.data();

            int error = nbl_build(neighList,
                                  Natoms,
                                  c,
                                  influenceDistance,
                                  numberOfCutoffs,
                                  pcutoffs,
                                  nn);
            if (error == 1)
            {
              throw std::runtime_error(
                  "Cell size too large! (partilces fly away) or\n"
                  "Collision of atoms happened!");
            }
          },
          "Build neighList.",
          py::arg("neighList"),
          py::arg("coords").noconvert(),
          py::arg("influenceDistance"),
          py::arg("cutoffs").noconvert(),
          py::arg("need_neigh").noconvert());

  module.def("get_neigh",
             [](NeighList const *const neighList,
                py::array_t<double> cutoffs,
                int const neighborListIndex,
                int const particleNumber) {
    int numberOfCutoffs = static_cast<int>(cutoffs.size());
    double const * pcutoffs = cutoffs.data();
    int numberOfNeighbors = 0;
    int const * neighOfAtom;

    int error = nbl_get_neigh(neighList,
                              numberOfCutoffs,
                              pcutoffs,
                              neighborListIndex,
                              particleNumber,
                              &numberOfNeighbors,
                              &neighOfAtom);
    if (error == 1)
    {
      throw std::runtime_error(
          "neighborListIndex >= neighList->numberOfNeighborLists!  or\n"
          "cutoffs[neighborListIndex] > "
          "neighList->lists[neighborListIndex]->cutoff!  or\n"
          "particleNumber >= numberOfParticles!  or\n"
          "particleNumber < 0!");
    }

    // pack as a numpy array
    auto neighborsOfParticle = py::array(py::buffer_info(
        const_cast<int *>(neighOfAtom),  // data pointer
        sizeof(int),  // size of one element
        py::format_descriptor<int>::format(),  // Python struct-style
                                               // format descriptor
        1,  // dimension
        {numberOfNeighbors},  // size of each dimension
        {sizeof(int)}  // stride of each dimension
        ));

    py::tuple re(2);
    re[0] = numberOfNeighbors;
    re[1] = neighborsOfParticle;
    return re;
  }, R"pbdoc(
     Get the neighList's numberOfNeighbors and neighborsOfParticle.

     Returns:
         int, 1darray: numberOfNeighbors, neighborsOfParticle
     )pbdoc",
     py::arg("neighList"),
     py::arg("cutoffs").noconvert(),
     py::arg("neighborListIndex"),
     py::arg("particleNumber"));

  // cannot bind `nbl_get_neigh_kim` directly, since it has pointer arguments
  // so we return a pointer to this function
  module.def("get_neigh_kim", []() {
    // the allowed return pointer type by pybind11 is: void const *
    // so cast the function pointer to it, and we need to cast back when
    // using it
    return (void const *) &nbl_get_neigh;
  });

  module.def("create_paddings",
             [](double const influenceDistance,
                py::array_t<double> cell,
                py::array_t<int> PBC,
                py::array_t<double> coords,
                py::array_t<int> species) {
    int Natoms_1 = static_cast<int>(coords.size() / 3);
    int Natoms_2 = static_cast<int>(species.size());

    if (Natoms_1 != Natoms_2)
    {
      throw std::runtime_error(
          "\"coords\" size and \"species\" size do not match!");
    }

    int Natoms = Natoms_1 <= Natoms_2 ? Natoms_1 : Natoms_2;

    double const * cell2 = cell.data();
    int const * PBC2 = PBC.data();
    double const * coords2 = coords.data();
    int const * species2 = species.data();

    int Npad;
    std::vector<double> pad_coords;
    std::vector<int> pad_species;
    std::vector<int> pad_image;

    int error = nbl_create_paddings(Natoms,
                                    influenceDistance,
                                    cell2,
                                    PBC2,
                                    coords2,
                                    species2,
                                    Npad,
                                    pad_coords,
                                    pad_species,
                                    pad_image);
    if (error == 1)
    {
      throw std::runtime_error(
          "In inverting the cell matrix, the determinant is 0!");
    }

    // pack as a 2D numpy array
    auto coordinates_of_paddings
        = py::array(py::buffer_info(pad_coords.data(),
                                    sizeof(double),
                                    py::format_descriptor<double>::format(),
                                    2,
                                    {Npad, 3},
                                    {sizeof(double) * 3, sizeof(double)}));

    // pack as a numpy array
    auto species_code_of_paddings
        = py::array(py::buffer_info(pad_species.data(),
                                    sizeof(int),
                                    py::format_descriptor<int>::format(),
                                    1,
                                    {Npad},
                                    {sizeof(int)}));

    // pack as a numpy array
    auto master_particle_of_paddings
        = py::array(py::buffer_info(pad_image.data(),
                                    sizeof(int),
                                    py::format_descriptor<int>::format(),
                                    1,
                                    {Npad},
                                    {sizeof(int)}));

    py::tuple re(3);
    re[0] = coordinates_of_paddings;
    re[1] = species_code_of_paddings;
    re[2] = master_particle_of_paddings;
    return re;
  }, R"pbdoc(
     Create padding.

     Returns:
         2darray, 1darray, 1darray: coordinates_of_paddings,
             species_code_of_paddings, master_particle_of_paddings
     )pbdoc",
     py::arg("influenceDistance"),
     py::arg("cell").noconvert(),
     py::arg("PBC").noconvert(),
     py::arg("coords").noconvert(),
     py::arg("species").noconvert());
}
