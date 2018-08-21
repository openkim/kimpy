#include <vector>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>

#include "neighbor_list.h"

#define DIM 3


void write_extendxyz(int numberOfParticles, double const* cell,
    double const* coordinates, int const* speciesCode)
{
  std::fstream fs;

  fs.open("atoms.xyz", std::fstream::out);

  fs << numberOfParticles << std::endl;
  fs << "Lattice=\"";
  for (int i = 0; i < 9; i++) {
    fs << " " << cell[i];
  }
  fs << "\" Properties=speciesCode:S:1:pos:R:3" << std::endl;
  for (int i = 0; i < numberOfParticles; i++) {
    fs << speciesCode[i] << " " << coordinates[i * 3] << " " << coordinates[i * 3 + 1]
       << " " << coordinates[i * 3 + 2] << std::endl;
  }

  fs.close();
}


int main()
{
  double alat = 2.46;
  double d = 3.35;
  double cutoffs[2] = { d + 0.01, d + 0.02 };
  double influenceDistance = cutoffs[1];
  int pbc[3] = { 1, 1, 1 };

  double cell[9] = { alat, 0, 0, 0.5 * alat, sqrt(3) * 0.5 * alat, 0, 0, 0, 2 * d };
  double* cell1 = cell;
  double* cell2 = cell + 3;
  double* cell3 = cell + 6;

  // initialize configurations (AB stacking graphite)
  int numberOfParticles = 4;
  double coordinates[DIM * numberOfParticles];
  int i = 0;

  for (int j = 0; j < 3; j++) {
    coordinates[DIM*i+j] = 0*cell1[j] + 0*cell2[j] + 0*cell3[j];
  }
  i = 1;
  for (int j = 0; j < 3; j++) {
    coordinates[DIM*i+j] = 1/3.*cell1[j] + 1/3.*cell2[j] + 0*cell3[j];
  }
  i = 2;
  for (int j = 0; j < 3; j++) {
    coordinates[DIM*i+j] = 1/3.*cell1[j] + 1/3.*cell2[j] + 1/2.*cell3[j];
  }
  i = 3;
  for (int j = 0; j < 3; j++) {
    coordinates[DIM*i+j] = 2/3.*cell1[j] + 2/3.*cell2[j] + 1/2.*cell3[j];
  }
  int speciesCode[numberOfParticles];
  speciesCode[0] = 1;
  speciesCode[1] = 1;
  speciesCode[2] = 2;
  speciesCode[3] = 2;


  int numberOfPaddings;
  std::vector<double> coordinatesOfPaddings;
  std::vector<int> speciesCodeOfPadding;
  std::vector<int> masterOfPaddings;

  /* create padding atoms */
  nbl_create_paddings(numberOfParticles, influenceDistance, cell, pbc, coordinates,
      speciesCode, numberOfPaddings, coordinatesOfPaddings,
      speciesCodeOfPadding, masterOfPaddings);

  int total = numberOfParticles + numberOfPaddings;

  double* coords_all = new double[total * DIM];
  int* code_all = new int[total * DIM];
  std::memcpy(coords_all, coordinates, sizeof(double) * numberOfParticles * DIM);
  std::memcpy(coords_all + numberOfParticles * DIM, coordinatesOfPaddings.data(),
      sizeof(double) * numberOfPaddings * DIM);
  std::memcpy(code_all, speciesCode, sizeof(int) * numberOfParticles);
  std::memcpy(code_all + numberOfParticles, speciesCodeOfPadding.data(),
      sizeof(int) * numberOfPaddings);


  /* generate neighborlist */
  int* needNeighbors = new int[total];
  for (int i = 0; i < numberOfParticles; i++) {
    needNeighbors[i] = 1;
  }
  for (int i = numberOfParticles; i < total; i++) {
    needNeighbors[i] = 0;
  }

  // create neighbor list
  NeighList* nl;
  nbl_initialize(&nl);

  std::cout << "get_neigh test" << std::endl;
  for (int k = 0; k < 2; k++) {
    nbl_build(nl, total, coords_all, influenceDistance, 2, cutoffs, needNeighbors);

    // use get neigh
    int particleNumber = 0;
    int numberOfNeighbors;
    int const* neighborsOfParticle;
    int neighborListIndex = 0;
    nbl_get_neigh(nl, 2, cutoffs, neighborListIndex, particleNumber,
        &numberOfNeighbors, &neighborsOfParticle);

    std::cout << std::endl << std::endl;
    std::cout << "iteration = " << k << std::endl;
    std::cout << "neighbor list index = " << neighborListIndex << std::endl;
    std::cout << "particleNumber = " << particleNumber;
    std::cout << ", numberOfNeighbors = " << numberOfNeighbors << std::endl;
    for (int i = 0; i < numberOfNeighbors; i++) {
      std::cout << "i = " << i << ", neigh = " << neighborsOfParticle[i] << std::endl;
    }

    neighborListIndex = 1;
    nbl_get_neigh(nl, 2, cutoffs, neighborListIndex, particleNumber,
        &numberOfNeighbors, &neighborsOfParticle);

    std::cout << "neighbor list index = " << neighborListIndex << std::endl;
    std::cout << "particleNumber = " << particleNumber;
    std::cout << ", numberOfNeighbors = " << numberOfNeighbors << std::endl;
    for (int i = 0; i < numberOfNeighbors; i++) {
      std::cout << "i = " << i << ", neigh = " << neighborsOfParticle[i] << std::endl;
    }
  }

  // print to xyz file
  write_extendxyz(total, cell, coords_all, code_all);

  // free neighborlist
  nbl_clean(&nl);

  // free local data
  delete [] coords_all;
  delete [] code_all;
  delete [] needNeighbors;

  return 0;
}
