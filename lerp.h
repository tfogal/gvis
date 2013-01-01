#ifndef TJF_GVIS_LERP_H
#define TJF_GVIS_LERP_H
#include <cassert>

namespace gv {

template<typename T> double
lerp(T v, T imin,T imax, double omin,double omax) {
  assert(imin <= v && v <= imax);
  return omin + (v-imin) * ((omax-omin)/(imax-imin));
}

}
#endif
