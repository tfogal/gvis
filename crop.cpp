#include <cassert>
#include <stdexcept>
#include "crop.h"
#include "nonstd.h"

namespace gv {

typedef struct Crop::cinfo {
  size_t axis;
  uint64_t index;
} cinfo_t;

Crop::Crop() : ci(new cinfo_t) {
  this->ci->axis = 0;
  this->ci->index = 0;
}
Crop::~Crop() { }

void Crop::set_axis(size_t axis) { this->ci->axis = axis; }
void Crop::set_index(uint64_t index) { this->ci->index = index; }

namespace {
// pull the 'enum datatype' from an array of the given type.
template<typename T> enum datatype type(const T* buf);
template<> enum datatype type<uint8_t>(const uint8_t*) {
  return UNSIGNED_INTEGER;
}
template<> enum datatype type<uint16_t>(const uint16_t*) {
  return UNSIGNED_INTEGER;
}
template<> enum datatype type<uint32_t>(const uint32_t*) {
  return UNSIGNED_INTEGER;
}
template<> enum datatype type<uint64_t>(const uint64_t*) {
  return UNSIGNED_INTEGER;
}
template<> enum datatype type<int8_t>(const int8_t*) { return INTEGER; }
template<> enum datatype type<int16_t>(const int16_t*) { return INTEGER; }
template<> enum datatype type<int32_t>(const int32_t*) { return INTEGER; }
template<> enum datatype type<int64_t>(const int64_t*) { return INTEGER; }
template<> enum datatype type<float>(const float*) { return FLOAT; }
template<> enum datatype type<double>(const double*) { return FLOAT; }
}

namespace { template<typename T> stream
work(const T* din, const size_t idims[4], const size_t dims[4],
     size_t channels) {
  if(channels != 1) {
    throw std::runtime_error("bug: we don't handle multi-channel data.");
  }
  stream rv;

  std::shared_ptr<T> dout(new T[dims[0]*dims[1]*dims[2]*dims[3] * sizeof(T)],
                          nonstd::DeleteArray<T>());
  for(size_t t=0; t < dims[3]; ++t) {
    for(size_t z=0; z < dims[2]; ++z) {
      for(size_t y=0; y < dims[1]; ++y) {
        for(size_t x=0; x < dims[0]; ++x) {
          size_t oidx = t * dims[2] * dims[1] * dims[0] +
                        z * dims[1] * dims[0] +
                        y * dims[0] + x;
          size_t iidx = t * idims[2] * idims[1] * idims[0] +
                        z * idims[1] * idims[0] +
                        y * idims[0] + x;
          dout.get()[oidx] = din[iidx];
        }
      }
    }
  }
  rv.data = dout;
  rv.info.type = type(din);
  rv.info.width = sizeof(T);
  rv.info.channels = channels;
  rv.info.dimensions[0] = dims[0];
  rv.info.dimensions[1] = dims[1];
  rv.info.dimensions[2] = dims[2];
  rv.info.dimensions[3] = dims[3];
  return rv;
}}

void Crop::execute() {
  assert(this->ci->index > 0 && "forgot to set index?");
  // if they want a dimension to have 0 values, then the whole data set will be
  // cropped.
  if(this->ci->index == 0) {
    throw std::runtime_error("did not set index");
  }

  cstream strm = const_cast<const Crop*>(this)->input(0);
  assert(valid_stream(strm.info));
  if(!valid_stream(strm.info)) {
    throw std::runtime_error("stream is nonsense.");
  }

  // just some aliases to save typing
  const bstream_t info = strm.info;
  const size_t idims[4] = { info.dimensions[0], info.dimensions[1],
                            info.dimensions[2], info.dimensions[3] };
  /* output dimensions are the same as our input dims... */
  size_t dims[4] = { idims[0], idims[1], idims[2], idims[3] };
  /* ... except one of our axes is now shorter. */
  dims[this->ci->axis] = this->ci->index;

  if(info.channels == 1) {
    throw std::runtime_error("bug: it should, but right now crop doesn't "
                             "handle multi-channel data.");
  }

  stream outs;
  if(info.type == UNSIGNED_INTEGER && info.width == 1) {
    outs = work<uint8_t>(static_cast<const uint8_t*>(strm.data.get()), idims,
                         dims, info.channels);
  } else if(info.type == UNSIGNED_INTEGER && info.width == 2) {
    outs = work<uint16_t>(static_cast<const uint16_t*>(strm.data.get()), idims,
                          dims, info.channels);
  } else if(info.type == UNSIGNED_INTEGER && info.width == 4) {
    outs = work<uint32_t>(static_cast<const uint32_t*>(strm.data.get()), idims,
                          dims, info.channels);
  } else if(info.type == UNSIGNED_INTEGER && info.width == 8) {
    outs = work<uint64_t>(static_cast<const uint64_t*>(strm.data.get()), idims,
                          dims, info.channels);
  } else if(info.type == INTEGER && info.width == 1) {
    outs = work<int8_t>(static_cast<const int8_t*>(strm.data.get()), idims,
                        dims, info.channels);
  } else if(info.type == INTEGER && info.width == 2) {
    outs = work<int16_t>(static_cast<const int16_t*>(strm.data.get()), idims,
                         dims, info.channels);
  } else if(info.type == INTEGER && info.width == 4) {
    outs = work<int32_t>(static_cast<const int32_t*>(strm.data.get()), idims,
                         dims, info.channels);
  } else if(info.type == INTEGER && info.width == 8) {
    outs = work<int64_t>(static_cast<const int64_t*>(strm.data.get()), idims,
                         dims, info.channels);
  } else if(info.type == FLOAT && info.width == 4) {
    outs = work<float>(static_cast<const float*>(strm.data.get()), idims,
                       dims, info.channels);
  } else if(info.type == FLOAT && info.width == 8) {
    outs = work<double>(static_cast<const double*>(strm.data.get()), idims,
                        dims, info.channels);
  } else {
    throw std::runtime_error("unknown type.");
  }
}

}
