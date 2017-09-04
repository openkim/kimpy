#ifndef _PADDING_H_
#define _PADDING_H_

void set_padding(int Natoms, double* cell, int* PBC, double cutoff, double** coords,
  char (**species)[4], int** pad_image, int* Npad);

#endif
