#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <new>

#include "KIM_API_C.h"
#include "KIM_API_status.h"
#include "neigh.h"

#define DIM 3


int nbl_initialize(void* kimmdl) {

  int status;
  const char* NBC;

  // check NBC, only `NEIGH_PURE_F' is supported
  status = KIM_API_get_NBC_method(kimmdl, &NBC);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__, "KIM_API_get_NBC_method", status);
    return status;
  }
  if (strcmp(NBC, "NEIGH_PURE_F") != 0) {
    KIM_API_report_error(__LINE__, __FILE__, "NBC method unsupported", status);
    return status;
  }

  // free neighbor list content, and the neighbor list itself
  nbl_clean(kimmdl);

  // setup a blank neighborlist
  NeighList *nl = new NeighList[1];
  nl->Nneighbors = nullptr;
  nl->neighborList = nullptr;
  nl->beginIndex = nullptr;

  // register neigh object and get_neigh method in KIM API
  status = KIM_API_set_data(kimmdl, "neighObject", 1, nl);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__, "KIM_API_set_data", status);
    return status;
  }
  status = KIM_API_set_method(kimmdl, "get_neigh", 1, (func_ptr) &nbl_get_neigh);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__, "KIM_API_set_method", status);
    return status;
  }

  return KIM_STATUS_OK;
}


// set up the neighbor list
int nbl_build_neighborlist(void* kimmdl, const int* is_padding, int padding_need_neigh)
{

  int status;
  int* Natoms;
  double* coords;
  double* cutoff;
  NeighList *nl;

  int i, j, k, ii, jj, kk;
  int num_neigh;
  double dx[DIM];
  double rcut, cutsq, rsq;
  int size[DIM];
  int size_total;
  int index[DIM];
  int idx;
  int total;
  int n;
  double min[DIM];
  double max[DIM];


  // get neigh object
  nl = (NeighList*) KIM_API_get_data(kimmdl, "neighObject", &status);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__, "KIM_API_get_data", status);
    return status;
  }

  // get necessary data for constructing neighborlist
  KIM_API_getm_data(kimmdl,  &status,  3*3,
      "numberOfParticles",   &Natoms,  1,
      "coordinates",         &coords,  1,
      "cutoff",              &cutoff,  1);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__, "KIM_API_getm_data", status);
    return status;
  }

  rcut = *cutoff;
  cutsq = rcut*rcut;

// init max and min of coords to that of the first atom
  for (k=0; k<DIM; k++){
    min[k] = coords[k];
    max[k] = coords[k] + 1; // +1 to prevent max==min for 1D and 2D case
  }

  for (i=0; i<*Natoms; i++){
    for (j=0; j<DIM; j++){
      if (max[j] < coords[DIM*i+j]) max[j] = coords[DIM*i+j];
      if (min[j] > coords[DIM*i+j]) min[j] = coords[DIM*i+j];
    }
  }

  /* make the cell box */
  size_total = 1;
  for (i=0; i<DIM; i++){
    size[i] = (int)((max[i]-min[i])/rcut);
    size[i] = size[i] <= 0 ? 1 : size[i];
    size_total *= size[i];
  }
  if (size_total > 1000000000) {
    std::cerr << "kimpy: Cell size" << size[0]<<" x " <<size[1]<<" x "<<size[2]<<
        "? You thinks me a bit ambitious." <<std::endl;
    std::cerr << "kimpy: Check if you have particles at 1e10." << std::endl;
    std::cerr << "kimpy: Attempting to proceed..." << std::endl;
  }


  // assign atoms into cells
  std::vector<std::vector<int>> cells(size_total);
  for (i=0; i<*Natoms; i++){
    coords_to_index(&coords[DIM*i], size, index, max, min);
    idx = index[0] + index[1]*size[0] + index[2]*size[0]*size[1];
    cells[idx].push_back(i);
  }


  // create neighbors

  // free previous neigh content first
  nbl_free_neigh_content(kimmdl);
  nl->Nneighbors = new int[*Natoms];
  nl->beginIndex = new int[*Natoms];

  // temporary neigh container
  std::vector<int> tmp_neigh;

  total = 0;
  for (i=0; i<*Natoms; i++){
    num_neigh = 0;

    if (!is_padding[i] || (is_padding[i] && padding_need_neigh)) {

      coords_to_index(&coords[DIM*i], size, index, max, min);

      // loop over neighborling cells and the cell atom i resides
      for (ii=std::max(0, index[0]-1); ii<=std::min(index[0]+1, size[0]-1); ii++)
      for (jj=std::max(0, index[1]-1); jj<=std::min(index[1]+1, size[1]-1); jj++)
      for (kk=std::max(0, index[2]-1); kk<=std::min(index[2]+1, size[2]-1); kk++){

        idx = ii + jj*size[0] + kk*size[0]*size[1];

        for (j=0; j<(int)cells[idx].size(); j++) {
          n = cells[idx][j];
          if (n != i) {
            rsq = 0.0;
            for (k=0; k<DIM; k++) {
              dx[k] = coords[DIM*n+k] - coords[DIM*i+k];
              rsq += dx[k]*dx[k];
            }
            if (rsq < cutsq) {
              tmp_neigh.push_back(n);
              num_neigh++;
            }
          }
        }
      }
    }

    nl->Nneighbors[i] = num_neigh;
    nl->beginIndex[i] = total;
    total += num_neigh;
  }

  // copy tmp_neigh to NeighList
  nl->neighborList = new int[total];
  std::memcpy(nl->neighborList, tmp_neigh.data(), sizeof(int)*total);


  return KIM_STATUS_OK;
}


/* get neigh function */
int nbl_get_neigh(void* pkim, int* mode, int* request, int *atom, int* numnei,
    int** nei1atom, double** rij)
{

  intptr_t* kimmdl = *((intptr_t**) pkim);
  int status;
  int* Natoms;

  NeighList *nl;

  nl = (NeighList*) KIM_API_get_data(kimmdl, "neighObject", &status);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__,"KIM_API_get_data", status);
    return status;
  }

  Natoms = (int*) KIM_API_get_data(kimmdl, "numberOfParticles", &status);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__,"KIM_API_get_data", status);
    return status;
  }

  // only locator mode is supported
  if(*mode != 1) {
    status = KIM_STATUS_NEIGH_INVALID_MODE;
    KIM_API_report_error(__LINE__, __FILE__,"Invalid mode in get_neigh", status);
     return status;
  }

  // set atom of which we are returning neighbors
  if (*request >= *Natoms || *request < 0) {
    status = KIM_STATUS_PARTICLE_INVALID_ID;
    KIM_API_report_error(__LINE__, __FILE__,"Invalid atom ID in get_neigh", status);
    return status;
  }
  else {
    *atom = *request;
  }

  /* set the returned number of neighbors for the returned atom */
  *numnei = nl->Nneighbors[*atom];

  /* set the location for the returned neighbor list */

  *nei1atom = nl->neighborList + nl->beginIndex[*atom];

  // set rij
  *rij = nullptr;

  return KIM_STATUS_OK;
}


void coords_to_index(double *x, int *size, int *index, double *max, double *min){
  int i;
  for (i=0; i<DIM; i++) {
    index[i] = (int)(((x[i]-min[i])/(max[i]-min[i]) - 1e-14) * size[i]);
  }
}


//  free both the content in the neighbor list and the neighbor list itself
int nbl_clean(void* kimmdl) {
  int status = nbl_free_neigh_content(kimmdl);

  // free neighbor list content
  NeighList *nl = (NeighList*) KIM_API_get_data(kimmdl, "neighObject", &status);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__,"KIM_API_get_data", status);
    return status;
  }

  // free neighbor list itself
  safefree(nl);

  return KIM_STATUS_OK;
}


// only free the content in the neighbor list, not the neighbor list itself
int nbl_free_neigh_content(void* kimmdl)
{
  int status;

  NeighList *nl = (NeighList*) KIM_API_get_data(kimmdl, "neighObject", &status);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__,"KIM_API_get_data", status);
    return status;
  }

  if (nl != nullptr) {
    safefree(nl->Nneighbors);
    safefree(nl->neighborList);
    safefree(nl->beginIndex);
  }

  return KIM_STATUS_OK;
}


void safefree(void *ptr)
{
  if (ptr != nullptr) {
    delete[] ptr;
  }
  ptr = nullptr;
}

