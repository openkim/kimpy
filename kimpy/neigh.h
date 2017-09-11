#ifndef _NEIGHBORLIST_H_
#define _NEIGHBORLIST_H_

// neighborlist structure
typedef struct
{
  int* Nneighbors;
  int* neighborList;
  int* beginIndex;
} NeighList;

// In the following, NeighList means a pointer to the `NeighList` struct, and its
// contents indicate pointers to `Nneighbors`, `neighborList', and `beginIndex'.

// free previous one if existing, and then initialize NeighList and its contents
int nbl_initialize(void* kimmdl);

// free previous Neighlist contents, and create new
int nbl_build_neighborlist(void* kimmdl, double cutoff, const int* is_padding,
    int padding_need_neigh);

// free Neighlist and its contents
int nbl_clean(void* kimmdl);

// used by model
int nbl_get_neigh(void* kimmdl, int* mode, int* request, int *atom, int* numnei,
    int** nei1atom, double** rij);

// helper function
int nbl_free_neigh_content(void* kimmdl);

void safefree(void *ptr);

void coords_to_index(double *x, int *size, int *index, double *max, double *min);


#endif
