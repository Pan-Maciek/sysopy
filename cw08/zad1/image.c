#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "macros.h"

#define SHADES 256

typedef unsigned char shade;
typedef struct image {
  int width, height;
  shade **data; // [x][y]
} image;

image* load_image(char* path) {
  FILE* file = fopen(path, "r");
  assert(file != NULL);
  image* img = malloc(sizeof *img);
  int shades;
  int read = fscanf(file, "P2\n%d %d\n%d\n", &img->width, &img->height, &shades);
  assert(shades <= SHADES);

  assert(read == 3);
  img->data = malloc(sizeof *img->data * img->width);
  repeat(img->width) img->data[i] = malloc(sizeof **img->data * img->height);

  for (int y = 0; y < img->height; y++)
    for (int x = 0; x < img->width; x++)
      fscanf(file, "%hhu", &img->data[x][y]);

  fclose(file);
  return img;
}
