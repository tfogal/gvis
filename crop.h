#ifndef TJF_GVIS_CROP_H
#define TJF_GVIS_CROP_H

#include <cstdint>
#include <memory>
#include "filter.h"

namespace gv {

/// Simple cropping filter.  Takes a dimension (i.e. axis), and a
/// value/index to crop in comparison to. outputs the data set which
/// trims out the given dimension as appropriate.
class Crop: public Filter {
  public:
    Crop();
    virtual ~Crop();

    void set_axis(size_t axis);
    void set_index(uint64_t index);
    virtual void execute();

  private:
    struct cinfo;
    std::unique_ptr<struct cinfo> ci;
};

}
#endif
