#ifndef _CVEC_H_
#define _CVEC_H_

#include <string.h>
#include <stdlib.h>

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

typedef struct {
  int *array;
  int arr_size, elems;
} cvec;

void cvec_init(cvec* c, int s);
void cvec_destroy(cvec *c);

int  cvec_at(cvec *c, int i);
void cvec_insert_back(cvec *c, int ins);
void cvec_reallocate(cvec *c, int size_new);

typedef struct {
  double *array;
  int arr_size, elems;
} dvec;

void   dvec_init(dvec* c, int s);
void   dvec_destroy(dvec *c);

double dvec_at(dvec *c, int i);
void   dvec_insert_back(dvec *c, double ins);
void   dvec_reallocate(dvec *c, int size_new);

#endif
