#ifndef TJF_GVIS_FILTER_H
#define TJF_GVIS_FILTER_H

#include <memory>
#include <vector>
#include "bstream.h"

namespace gv {

typedef struct _fake_stream {
  std::shared_ptr<void> data;
  bstream_t info;
} stream;

/// of course, const is not transitive, but we want a notion of a 'const stream'
/// to not be able to modify the data.
typedef struct _fake_cstream {
  std::shared_ptr<const void> data;
  bstream_t info;
} cstream;

struct finfo;

class Filter {
  public:
    Filter();
    virtual ~Filter();

    void set_inputs(std::vector<std::shared_ptr<void>> data,
                    std::vector<bstream_t> ddescriptor);
    /// convenience function so one doesn't have to build a bunch of data
    /// structures.
    void set_input(std::shared_ptr<void> data, bstream_t);
    void set_input(stream strm);

    stream output(size_t i);
    const cstream output(size_t i) const;

    virtual void execute() = 0;

  protected:
    /// @returns the 'i'th input stream.
    stream input(size_t i);
    const cstream input(size_t i) const;

    void set_output(size_t i, std::shared_ptr<void>, bstream_t);
    void set_output(size_t i, stream);

  private:
    std::unique_ptr<struct finfo> fi;
};

}
#endif
