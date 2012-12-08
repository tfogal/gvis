#include <cstdlib>
#include "crop.h"
#include "gv-png.h"

int main() {
  {
  const char* depthfile = "/home/tfogal/data/scenedemo/D01293.png";
  uint8_t* img;
  uint32_t width, height;

  readpng(depthfile, &img, &width, &height);

  gv::Crop c;
  free(img);
  }
  return EXIT_SUCCESS;
}
