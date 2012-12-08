#include "bstream.h"

bool valid_stream(const bstream_t s) {
  return s.width <= 8 &&
         s.width != 3 && s.width != 5 && s.width != 6 && s.width != 7 &&
         s.dimensions[0] > 0 && s.dimensions[1] > 0 &&
         s.dimensions[2] > 0 && s.dimensions[2] > 0;
}
