#ifndef TJF_GVIS_MARCHING_SQUARES_H
#define TJF_GVIS_MARCHING_SQUARES_H

#include <memory>
#include "filter.h"

namespace gv {

/// Computes the set of isolines through a 2-dimensional data set.
class MarchingSquares: public Filter {
  public:
    MarchingSquares();
    virtual ~MarchingSquares();

    /// set the isovalue to use.
    void set_isovalue(uint64_t);
    void set_isovalue(int64_t);
    void set_isovalue(double);
    virtual void execute();

  private:
    struct minfo;
    std::unique_ptr<struct minfo> mi;
};

}
#endif
