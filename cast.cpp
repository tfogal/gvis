#include <cassert>
#include <stdexcept>
#include "cast.h"
#include "nonstd.h"

namespace gv {

typedef struct Cast::cinfo {
  bstream_t otype;
} cinfo_t;

Cast::Cast() : ci(new cinfo_t) { }
Cast::~Cast() { }

void Cast::set_otype(bstream_t t) { this->ci->otype = t; }

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

namespace { template<typename T, typename U> stream
work(const void* in, const uint64_t dims[4], size_t channels) {
  stream rv;
  const T* din = static_cast<const T*>(in);
  std::shared_ptr<U> dout(new U[dims[0]*dims[1]*dims[2]*dims[3]],
                          nonstd::DeleteArray<U>());
  for(size_t t=0; t < dims[3]; ++t) {
    for(size_t z=0; z < dims[2]; ++z) {
      for(size_t y=0; y < dims[1]; ++y) {
        for(size_t x=0; x < dims[0]; ++x) {
          size_t idx = t * dims[2] * dims[1] * dims[0] +
                       z * dims[1] * dims[0] +
                       y * dims[0] + x;
          dout.get()[idx] = static_cast<U>(din[idx]);
        }
      }
    }
  }
  rv.data = dout;
  rv.info.type = type(din);
  rv.info.width = sizeof(U);
  rv.info.channels = channels;
  rv.info.dimensions[0] = dims[0];
  rv.info.dimensions[1] = dims[1];
  rv.info.dimensions[2] = dims[2];
  rv.info.dimensions[3] = dims[3];
  return rv;
}}

void Cast::execute() {
  cstream strm = const_cast<const Cast*>(this)->input(0);
  assert(valid_stream(strm.info));
  if(!valid_stream(strm.info)) {
    throw std::runtime_error("stream is nonsense.");
  }

  const bstream_t info = strm.info;
  stream outs;
  const bstream_t oi = this->ci->otype;

  if(info.type == UNSIGNED_INTEGER && info.width == 1 &&
     oi.type == UNSIGNED_INTEGER && oi.width == 1) {
    this->set_output(0,
      work<uint8_t, uint8_t>(strm.data.get(), strm.info.dimensions,
                             strm.info.channels)
    );
  } else if(info.type == UNSIGNED_INTEGER && info.width == 1 &&
            oi.type == UNSIGNED_INTEGER && oi.width == 2) {
    this->set_output(0,
      work<uint8_t, uint16_t>(strm.data.get(), strm.info.dimensions,
                              strm.info.channels)
    );
  } else if(info.type == UNSIGNED_INTEGER && info.width == 1 &&
            oi.type == UNSIGNED_INTEGER && oi.width == 4) {
    this->set_output(0,
      work<uint8_t, uint32_t>(strm.data.get(), strm.info.dimensions,
                              strm.info.channels)
    );
  } else if(info.type == UNSIGNED_INTEGER && info.width == 1 &&
            oi.type == UNSIGNED_INTEGER && oi.width == 8) {
    this->set_output(0,
      work<uint8_t, uint64_t>(strm.data.get(), strm.info.dimensions,
                              strm.info.channels)
    );
  } else {
    throw std::runtime_error("unknown type");
  }
}

}
