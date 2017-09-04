#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "KIM_API_C.h"
#include "KIM_API_status.h"
#include "cvec.h"
#include "neigh.h"

#define DIM 3

/* The pure neighlist that incorporates padding atoms and does not have a
   periodic nature. The atoms need to be ordered such that contributing atoms
   comes first than padding atoms.
 */

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

//TODO do not need this
//  nbl_free_neigh_object(kimmdl);

  // setup a blank neighborlist
  NeighList *nl = (NeighList*) malloc(sizeof(NeighList));
  nl->Nneighbors = NULL;
  nl->neighborList = NULL;
  nl->beginIndex = NULL;

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


int nbl_build_neighborlist(void* kimmdl, const int* is_padding, int padding_need_neigh)
{

  int status;
  int* Natoms;
  double* coords;
  double* cutoff;
  NeighList *nl;



  /* local vars */
  int i, j, k, ii, jj, kk;
  int neighbors;
  double dx[DIM];
  double R, R2, dist;
  int size[DIM];
  int size_total;
  int index[DIM];
  int ind;
  int total;
  int n;
  double min[3] = { 1e10,  1e10,  1e10};
  double max[3] = {-1e10, -1e10, -1e10};

  char msg[512];



  // get neigh object
  nl = (NeighList*) KIM_API_get_data(kimmdl, "neighObject", &status);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__, "KIM_API_get_data", status);
    return status;
  }

  // get necessary data for constructing neighborlist
  KIM_API_getm_data(kimmdl, &status, 3*3,
      "numberOfParticles",    &Natoms,    1,
      "coordinates",          &coords,    1,
      "cutoff",               &cutoff,    1);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__, "KIM_API_getm_data", status);
    return status;
  }

  /* reset neigh object to refill */
  nbl_free_neigh_object(kimmdl);
  nl->Nneighbors = (int*) malloc((*Natoms)*sizeof(int));
  nl->beginIndex = (int*) malloc((*Natoms)*sizeof(int));


  R = *cutoff;
  R2 = R*R;

  for (i=0; i<*Natoms; i++){
    for (j=0; j<3; j++){
      if (max[j] < coords[3*i+j]) max[j] = coords[3*i+j];
      if (min[j] > coords[3*i+j]) min[j] = coords[3*i+j];
    }
  }

  /* make the cell box */
  size_total = 1;
  for (i=0; i<3; i++){
    size[i] = (int)((max[i]-min[i])/R);
    size[i] = size[i] <= 0 ? 1 : size[i];
    size_total *= size[i];
  }

  if (size_total > 1000000000) {
    sprintf(msg, "NBL: Cell size %i x %i x %i ?! You thinks me a bit ambitious.\n",
        size[0], size[1], size[2]);
    sprintf(msg, "NBL: Check if you have particles at 1e10.\n");
    KIM_API_report_error(__LINE__, __FILE__, msg, status);
    return KIM_STATUS_FAIL;
  }

  cvec **cells = (cvec**)malloc(sizeof(cvec*)*size_total);
  for (i=0; i<size_total; i++){
    cells[i] = (cvec*)malloc(sizeof(cvec));
    cvec_init(cells[i], 4);
  }

  for (i=0; i<*Natoms; i++){
    coords_to_index(&coords[3*i], size, index, max, min);
    cvec_insert_back(cells[index[0] + index[1]*size[0] + index[2]*size[0]*size[1]], i);
  }

  cvec *temp_neigh = (cvec*)malloc(sizeof(cvec));
  cvec_init(temp_neigh, 4);

  total = 0;
  /* create neighbors */
  for (i=0; i<*Natoms; i++){
    neighbors = 0;

    // padding atom needs no neighbors
    if (is_padding[i] && !padding_need_neigh) {
      nl->Nneighbors[i] = neighbors;
      nl->beginIndex[i] = total;
      total += neighbors;
      continue;
    }

    coords_to_index(&coords[DIM*i], size, index, max, min);

    for (ii=MAX(0, index[0]-1); ii<=MIN(index[0]+1, size[0]-1); ii++){
      for (jj=MAX(0, index[1]-1); jj<=MIN(index[1]+1, size[1]-1); jj++){
        for (kk=MAX(0, index[2]-1); kk<=MIN(index[2]+1, size[2]-1); kk++){
          ind = ii + jj*size[0] + kk*size[0]*size[1];

          cvec *cell = cells[ind];
          for (j=0; j<cell->elems; j++) {
            n = cvec_at(cell, j);
            if (i != n) {
              dist = 0.0;
              for (k=0; k<DIM; k++) {
                dx[k] = coords[DIM*n+k] - coords[DIM*i+k];
                dist += dx[k]*dx[k];
              }
              if (dist < R2) {
                cvec_insert_back(temp_neigh, n);
                neighbors++;
              }
            }
          }
        }
      }
    }

    nl->Nneighbors[i] = neighbors;
    nl->beginIndex[i] = total;
    total += neighbors;
  }


  nl->neighborList = (int*)malloc(sizeof(int)*total);
  memcpy(nl->neighborList, temp_neigh->array, sizeof(int)*total);

  /* cleanup all the memory usage */
  cvec_destroy(temp_neigh);
  for (i=0; i<size_total; i++) {
    cvec_destroy(cells[i]);
    safefree(cells[i]);
  }
  safefree(cells);
  safefree(temp_neigh);


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
//  idx = 0;
//  for(i = 0; i < request; i++) {
//    idx += nl->Nneighbors[i];
//  }
//  *nei1atom = nl->neighborList + idx;

  *nei1atom = nl->neighborList + nl->beginIndex[*atom];



  // set rij
  *rij = NULL;

  return KIM_STATUS_OK;
}


int nbl_clean(void* kimmdl) {
  int status = nbl_free_neigh_object(kimmdl);
  return status;
}


int nbl_free_neigh_object(void* kimmdl)
{
  int status;

  NeighList *nl = (NeighList*) KIM_API_get_data(kimmdl, "neighObject", &status);
  if (KIM_STATUS_OK != status) {
    KIM_API_report_error(__LINE__, __FILE__,"KIM_API_get_data", status);
    return status;
  }

  if (nl != NULL) {
    safefree(nl->Nneighbors);
    safefree(nl->neighborList);
    safefree(nl->beginIndex);
  }

  return KIM_STATUS_OK;
}


void safefree(void *ptr)
{
  if (ptr != NULL) {
    free(ptr);
  }
  ptr = NULL;
}

void coords_to_index(double *x, int *size, int *index, double *max, double *min){
  int i;
  for (i=0; i<3; i++)
    index[i] = (int)(((x[i]-min[i])/(max[i]-min[i]) - 1e-14) * size[i]);
}



