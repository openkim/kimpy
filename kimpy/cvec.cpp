#include <string.h>
#include <stdlib.h>
#include "cvec.h"

/* ===========================================
    cvec
  ============================================*/
void cvec_init(cvec* c, int s){
    c->array = NULL;
    c->elems = 0;
    c->arr_size = s;
    cvec_reallocate(c, c->arr_size);
}

void cvec_destroy(cvec *c){
    free(c->array);
    c->array = NULL;
}

int cvec_at(cvec *c, int i){
    return c->array[i];
}

void cvec_insert_back(cvec *c, int ins){
    if (c->elems >= c->arr_size) {
      c->arr_size = c->arr_size << 1;
      cvec_reallocate(c, c->arr_size);
    }
    memcpy(c->array + c->elems, &ins, sizeof(int));
    c->elems++;
}

void cvec_reallocate(cvec *c, int size_new){
    c->array = (int*)realloc(c->array, size_new * sizeof(int));
}

/* ===========================================
    dvec
  ============================================*/
void dvec_init(dvec* c, int s){
    c->array = NULL;
    c->elems = 0;
    c->arr_size = s;
    dvec_reallocate(c, c->arr_size);
}

void dvec_destroy(dvec *c){
    free(c->array);
    c->array = NULL;
}

double dvec_at(dvec *c, int i){
    return c->array[i];
}

void dvec_insert_back(dvec *c, double ins){
    if (c->elems >= c->arr_size) {
      c->arr_size = c->arr_size << 1;
      dvec_reallocate(c, c->arr_size);
    }
    memcpy(c->array + c->elems, &ins, sizeof(double));
    c->elems++;
}

void dvec_reallocate(dvec *c, int size_new){
    c->array = (double*)realloc(c->array, size_new * sizeof(double));
}


