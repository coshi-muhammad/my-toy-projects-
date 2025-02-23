#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CAST(x) (x *)
#define CASTANDRETURN(x, y) (*((x *)(y)))
#define BYTE char

typedef struct {
  void *array;
  int element_size;
  int size;
  int capacity;
} DynamicArray;

DynamicArray *alocateArray(int e_s) {
  DynamicArray *array_ptr = (DynamicArray *)malloc(sizeof(DynamicArray));
  array_ptr->array = malloc(e_s);
  array_ptr->element_size = e_s;
  array_ptr->capacity = 1;
  array_ptr->size = 0;
  return array_ptr;
}

void growArray(DynamicArray *array_ptr) {
  array_ptr->capacity *= 2;
  array_ptr->array =
      realloc(array_ptr->array, array_ptr->capacity * array_ptr->element_size);
}

void shrinkArray(DynamicArray *array_ptr) {
  array_ptr->capacity = array_ptr->size;
  array_ptr->array =
      realloc(array_ptr->array, array_ptr->capacity * array_ptr->element_size);
}

void *getArrayElement(DynamicArray *array_ptr, int index) {
  if (index >= array_ptr->size) {
    return NULL;
  }
  return (BYTE *)array_ptr->array + (index * array_ptr->element_size);
}

void pushIntoArray(DynamicArray *array_ptr, void *element) {
  if (array_ptr->size == array_ptr->capacity) {
    growArray(array_ptr);
  }
  memcpy((BYTE *)array_ptr->array + (array_ptr->size * array_ptr->element_size),
         element, array_ptr->element_size);
  array_ptr->size++;
}
void addElementAtId(DynamicArray *array_ptr, void *element, int i) {
  if (array_ptr->size == array_ptr->capacity) {
    growArray(array_ptr);
  }
  memcpy((BYTE *)array_ptr->array + (array_ptr->element_size * (i + 1)),
         (BYTE *)array_ptr->array + (array_ptr->element_size * i),
         (array_ptr->size - i) * array_ptr->element_size);
  memcpy((BYTE *)array_ptr->array + (array_ptr->element_size * i), element,
         array_ptr->element_size);
  array_ptr->size++;
}
void removeElementAtId(DynamicArray *array_ptr, int i) {
  if (array_ptr->size == array_ptr->capacity / 2) {
    shrinkArray(array_ptr);
  }
  memcpy((BYTE *)array_ptr->array + (array_ptr->element_size * i),
         (BYTE *)array_ptr->array + (array_ptr->element_size * (i + 1)),
         (array_ptr->size - (i + 1)) * array_ptr->element_size);
  array_ptr->size--;
}
void popOutofArray(DynamicArray *array_ptr) {

  array_ptr->size--;
  if (array_ptr->size == array_ptr->capacity / 2) {
    shrinkArray(array_ptr);
  }
}

int searBYTEray(DynamicArray *array_ptr, void *element) {
  for (int i = 0; i < array_ptr->size; i++) {
    if (memcmp((BYTE *)array_ptr->array + (array_ptr->element_size * i),
               element, array_ptr->element_size) == 0) {
      return i;
    }
  }
  return -1;
}

void deleteArray(DynamicArray *array_ptr) {
  free(array_ptr->array);
  free(array_ptr);
}

bool isArrayEmpty(DynamicArray *array_ptr) {
  if (array_ptr->size == 0) {
    return true;
  }
  return false;
}
