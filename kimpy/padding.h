#ifndef _PADDING_H_
#define _PADDING_H_
#include<vector>

void set_padding(double* cell, int* PBC, double cutoff,
    int Natoms, const double* coords, const int* species,
    std::vector<double>& pad_coords, std::vector<int>& pad_species,
    std::vector<int>& pad_image);

#endif
