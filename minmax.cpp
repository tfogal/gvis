#include <cassert>
#include <cfloat>
#include <stdexcept>
#include <memory>
#include "minmax.h"
#include "nonstd.h"

namespace gv {

typedef struct MinMax::mminfo {
} mminfo_t;

MinMax::MinMax() : mi(new mminfo_t) { }
MinMax::~MinMax() {}

namespace {

template<typename T> std::shared_ptr<double>
work(const void* data, const uint64_t n) {
  std::shared_ptr<double> spmmax(new double[2], nonstd::DeleteArray<double>());
  double* mmax = spmmax.get(); // just for ease of typing.
  mmax[0] = FLT_MAX;
  mmax[1] = -FLT_MAX;
  const T* din = static_cast<const T*>(data);
  const T* end = din + n;
  while(din != end) {
    mmax[0] = std::min(static_cast<double>(*din), mmax[0]);
    mmax[1] = std::max(static_cast<double>(*din), mmax[1]);
    ++din;
  }
  return spmmax;
}

}

void MinMax::execute() {
  cstream strm = const_cast<const MinMax*>(this)->input(0);
  assert(valid_stream(strm.info));
  if(!valid_stream(strm.info)) {
    throw std::runtime_error("stream is nonsense.");
  }

  const bstream_t info = strm.info;
  const uint64_t n = info.dimensions[0] * info.dimensions[1] *
                     info.dimensions[2] * info.dimensions[3];
  stream rv;

  if(info.type == UNSIGNED_INTEGER && info.width == 1) {
    rv.data = work<uint8_t>(strm.data.get(), n);
  } else if(info.type == UNSIGNED_INTEGER && info.width == 2) {
    rv.data = work<uint16_t>(strm.data.get(), n);
  } else if(info.type == UNSIGNED_INTEGER && info.width == 4) {
    rv.data = work<uint32_t>(strm.data.get(), n);
  } else if(info.type == UNSIGNED_INTEGER && info.width == 8) {
    rv.data = work<uint64_t>(strm.data.get(), n);
  } else if(info.type == INTEGER && info.width == 1) {
    rv.data = work<int8_t>(strm.data.get(), n);
  } else if(info.type == INTEGER && info.width == 2) {
    rv.data = work<int16_t>(strm.data.get(), n);
  } else if(info.type == INTEGER && info.width == 4) {
    rv.data = work<int32_t>(strm.data.get(), n);
  } else if(info.type == INTEGER && info.width == 8) {
    rv.data = work<int64_t>(strm.data.get(), n);
  } else if(info.type == FLOAT && info.width == 4) {
    rv.data = work<float>(strm.data.get(), n);
  } else if(info.type == FLOAT && info.width == 8) {
    rv.data = work<double>(strm.data.get(), n);
  } else {
    throw std::runtime_error("unhandled type.");
  }
  rv.info.type = FLOAT;
  rv.info.width = sizeof(double);
  rv.info.channels = 2;
  rv.info.dimensions[0] = 2;
  rv.info.dimensions[1] = 1;
  rv.info.dimensions[2] = 1;
  rv.info.dimensions[3] = 1;
  this->set_output(0, rv);
}

}
