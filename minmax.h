#ifndef TJF_GVIS_MINMAX_H
#define TJF_GVIS_MINMAX_H

#include <memory>
#include "filter.h"

namespace gv {

// Calculates the min && max of a stream.  The output is just a simple
// array of two elements.
class MinMax: public Filter {
  public:
    MinMax();
    virtual ~MinMax();

    virtual void execute();

  private:
    struct mminfo;
    std::unique_ptr<struct mminfo> mi;
};

}
#endif
