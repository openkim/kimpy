#ifndef _NEIGHBORLIST_H_
#define _NEIGHBORLIST_H_

// neighborlist structure
typedef struct
{
  int* Nneighbors;
  int* neighborList;
  int* beginIndex;
} NeighList;


int nbl_initialize(void* kimmdl);

int nbl_build_neighborlist(void* kimmdl, const int* is_padding, int padding_need_neigh);

int nbl_clean(void* kimmdl);

// used by model
int nbl_get_neigh(void* kimmdl, int* mode, int* request, int *atom, int* numnei,
    int** nei1atom, double** rij);

// helper function
int nbl_free_neigh_object(void* kimmdl);

void safefree(void *ptr);

void coords_to_index(double *x, int *size, int *index, double *max, double *min);


#endif
