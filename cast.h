#ifndef TJF_GVIS_CAST_H
#define TJF_GVIS_CAST_H

#include <memory>
#include "filter.h"

namespace gv {

// Cast filter.  Just takes a buffer and casts it to whatever you need.
class Cast: public Filter {
  public:
    Cast();
    virtual ~Cast();

    void set_otype(bstream_t);
    virtual void execute();

  private:
    struct cinfo;
    std::unique_ptr<struct cinfo> ci;
};

}
#endif
