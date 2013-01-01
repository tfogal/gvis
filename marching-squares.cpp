#include <array>
#include <cassert>
#include <stdexcept>
#include "lerp.h"
#include "marching-squares.h"
#include "nonstd.h"

namespace gv {

typedef struct MarchingSquares::minfo {
  uint64_t isou;
  int64_t isoi;
  double isof;
  enum datatype isotype;
} minfo_t;

MarchingSquares::MarchingSquares() : mi(new minfo_t) {}
MarchingSquares::~MarchingSquares() {}

void MarchingSquares::set_isovalue(uint64_t u) {
  this->mi->isou = u;
  this->mi->isotype = UNSIGNED_INTEGER;
}
void MarchingSquares::set_isovalue(int64_t i) {
  this->mi->isou = i;
  this->mi->isotype = INTEGER;
}
void MarchingSquares::set_isovalue(double d) {
  this->mi->isou = d;
  this->mi->isotype = FLOAT;
}

static uint8_t ms_case(const std::array<bool, 4>& v) {
  uint8_t value = 0;
  if(v[0]) { value |= (1 << 3); }
  if(v[1]) { value |= (1 << 2); }
  if(v[2]) { value |= (1 << 1); }
  if(v[3]) { value |= (1 << 0); }
  return value;
}

namespace {
  template<typename T> static double t(T v, T low,T high) {
    return static_cast<double>(v - low) / static_cast<double>(high-low);
  }
}
// given the cell indices, return the world-space location.
std::array<double,4> location(const std::array<uint64_t, 4>& node) {
  // .. current system doesn't really have any notion of world-space, so this
  // is just a placeholder.  For now, we consider the indices to be world
  // locations.
  return std::array<double,4>({{
    static_cast<double>(node[0]),
    static_cast<double>(node[1]),
    static_cast<double>(node[2]),
    static_cast<double>(node[3])
  }});
}

void MarchingSquares::execute() {
  cstream strm = const_cast<const MarchingSquares*>(this)->input(0);
  assert(valid_stream(strm.info));
  if(!valid_stream(strm.info)) {
    throw std::runtime_error("stream is nonsense");
  }

  const bstream_t info = strm.info;

  // ease of typing.
  std::array<uint64_t,4> dims = {{ info.dimensions[0], info.dimensions[1],
                                   info.dimensions[2], info.dimensions[3] }};

  std::vector<std::array<double,4>> geom;
  if(info.type == UNSIGNED_INTEGER && info.width == 1) {
    const uint8_t* din = static_cast<const uint8_t*>(strm.data.get());
    std::shared_ptr<uint8_t> dout(new uint8_t[dims[0]*dims[1]*dims[2]*dims[3]],
                                  nonstd::DeleteArray<uint8_t>());
    for(uint64_t t=0; t < dims[3]; ++t) {
      for(uint64_t z=0; z < dims[2]; ++z) {
        for(uint64_t y=1; y < dims[1]; ++y) {
          for(uint64_t x=1; x < dims[0]; ++x) {
            assert(this->mi->isotype == UNSIGNED_INTEGER);
            const uint64_t width = dims[0];
            uint64_t topleft = ((y-1)*width + (x-1));
            uint64_t topright = ((y-1)*width + x);
            uint64_t bottomleft = (y*width + (x-1));
            uint64_t bottomright = (y*width + x);
            topleft += t*dims[2]*dims[1]*dims[0] + z*dims[1]*dims[0];
            topright += t*dims[2]*dims[1]*dims[0] + z*dims[1]*dims[0];
            bottomleft += t*dims[2]*dims[1]*dims[0] + z*dims[1]*dims[0];
            bottomright += t*dims[2]*dims[1]*dims[0] + z*dims[1]*dims[0];
            std::array<bool, 4> threshold = {{
              din[topleft] < this->mi->isou ? false : true,
              din[topright] < this->mi->isou ? false : true,
              din[bottomleft] < this->mi->isou ? false : true,
              din[bottomright] < this->mi->isou ? false : true
            }};
            uint8_t mcase = ms_case(threshold);
            assert(mcase <= 15);
            // 4D coords of topleft, bottomright, etc.
            std::array<uint64_t, 4> tl = {{ x-1, y-1, z, t }};
            std::array<uint64_t, 4> tr = {{ x,   y-1, z, t }};
            std::array<uint64_t, 4> br = {{ x,   y,   z, t }};
            //std::array<uint64_t, 4> bl = {{ x-1, y,   z, t }};
              
            // different cases, depending on where the line(s) should
            // cross through the cell.  The potentially confusing
            // part is the interpolation.  first we identify the %
            // of the way through the cell (between endpoints) the
            // vertex should cross, based on the values.
            // then we use that percentage to interpolate between
            // the *location* (not values) of the grid points.

            // we need a double to make sure the operation is done in FP
            const double thresh = static_cast<double>(this->mi->isou);
            switch(mcase) {
              case 0: case 1: /* easy! */ break;
              case 2: // topright is above, rest are below.
                assert(din[topright] > din[topleft]); // bisects.
                double percent = (thresh - din[topleft]) /
                                 (din[topright] - din[topleft]);
                std::array<double,4> l1 = location(tl);
                std::array<double,4> l2 = location(tr);
                std::array<double,4> loc_top = {{
                  lerp<double>(percent, 0.0,1.0, l1[0],l2[0]),
                  lerp<double>(percent, 0.0,1.0, l1[1],l2[1]),
                  lerp<double>(percent, 0.0,1.0, l1[2],l2[2]),
                  lerp<double>(percent, 0.0,1.0, l1[3],l2[3])
                }};
                geom.push_back(loc_top);

                std::array<double,4> l3 = location(br);
                assert(din[topright] > din[bottomright]); // it bisects.
                percent = (thresh - din[bottomright]) /
                          (din[topright] - din[bottomright]);
                std::array<double,4> loc_side = {{
                  lerp<double>(percent, 0.0,1.0, l2[0],l3[0]),
                  lerp<double>(percent, 0.0,1.0, l2[1],l3[1]),
                  lerp<double>(percent, 0.0,1.0, l2[2],l3[2]),
                  lerp<double>(percent, 0.0,1.0, l2[3],l3[3])
                }};
                geom.push_back(loc_side);
                break;
            }
          }
        }
      }
    }
  } else {
    throw std::runtime_error("unhandled type");
  }
}

}
