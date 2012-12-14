#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include "cast.h"
#include "crop.h"
#include "minmax.h"
#include "nonstd.h"
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

  {
    gv::MinMax mm;
    std::array<double, 12> arr = {{ 9,11,16,42, 19,8,-2,4, -19,86,14,8 }};
    bstream_t arri = {
      FLOAT,
      sizeof(double),
      1,
      { 12, 1, 1, 1 }
    };
    mm.set_input(std::shared_ptr<void>(arr.data(), nonstd::null_deleter()),
                 arri);
    mm.execute();
    gv::stream mmax = mm.output(0);
    const double* dat = static_cast<const double*>(mmax.data.get());
    std::cout << "minmax: (" << dat[0] << ")--(" << dat[1] << ")\n";
  }
  return EXIT_SUCCESS;
}
