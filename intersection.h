#ifndef intersection_h
#define intersection_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

typedef struct Image {
  double width;
  double height;
  char* color;
}Image;

typedef struct {
  char *type;
  double color[3];
  union {
    struct {
      double width;
      double height;
    } camera;
    struct {
      double location[3];
      double radius;
    } sphere;
    struct {
      double location[3];
      double normal[3];
    }
    plane;
  };
} Object;

typedef struct obj_array{
  Object* arr1;
  int onum;
} obj_array;


void pix_coloring(double *color, int row, int col,Image *image);
void normalize(double* v);
double plane_intersection(double* Ro, double* Rd, double* location, double* normal);
double sphere_intersection(double* Ro, double* Rd, double* C, double r);
void p6Create(Image *img, FILE* input, int magicNumber);
#endif
