#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include "cast.h"
#include "crop.h"
#include "gv-png.h"

int main() {
  {
  const char* depthfile = "/home/tfogal/data/scenedemo/D01293.png";
  uint8_t* img;
  uint32_t width, height;

  readpng(depthfile, &img, &width, &height);

  std::shared_ptr<void> id(img, free);
  bstream_t info = {
    UNSIGNED_INTEGER,
    sizeof(uint8_t),
    1,
    { width, height, 1, 1 }
  };
  //std::array<uint64_t,4> dims = {{ width, height, 1, 1 }};

#if 0
  gv::Crop c;
  c.set_axis(0);
  c.set_index(320);
  c.set_input(id, info);
  c.execute();
#endif

  gv::Cast cast;
  cast.set_input(id, info);
  bstream_t convert = info;
  convert.width = 2;
  cast.set_otype(convert);
  cast.execute();
  gv::stream upcasted = cast.output(0);

  assert(((const uint16_t*)upcasted.data.get())[0] == 27);

  writepng("test.png", (const uint16_t*)upcasted.data.get(), width, height);
  }
  return EXIT_SUCCESS;
}
