#include <cassert>
#include <stdexcept>
#include "filter.h"

namespace gv {

typedef struct finfo {
  std::vector<std::shared_ptr<void>> inputs;
  std::vector<std::shared_ptr<void>> outputs;
  std::vector<bstream_t> itype;
  std::vector<bstream_t> otype;
  bool invariant() const;
} finfo_t;

Filter::Filter() : fi(new finfo_t) { }
bool finfo_t::invariant() const {
  return this->inputs.size() == this->itype.size() &&
         this->outputs.size() == this->otype.size();
}

Filter::~Filter() {
  assert(this->fi->invariant());
  this->fi->inputs.clear();
  this->fi->outputs.clear();
  this->fi->itype.clear();
  this->fi->otype.clear();
  assert(this->fi->invariant());
}

void Filter::set_inputs(std::vector<std::shared_ptr<void>> data,
                        std::vector<bstream_t> ddescriptor) {
  assert(data.size() == ddescriptor.size());
  assert(this->fi->invariant());
  if(data.size() != ddescriptor.size()) {
    throw std::runtime_error("sizes do not match.");
    return;
  }
  this->fi->inputs = data;
  this->fi->itype = ddescriptor;
  assert(this->fi->invariant());
}

void Filter::set_input(std::shared_ptr<void> data, bstream_t ddescriptor) {
  assert(this->fi->invariant());
  this->fi->inputs.clear();
  this->fi->itype.clear();
  this->fi->inputs.push_back(data);
  this->fi->itype.push_back(ddescriptor);
  assert(this->fi->invariant());
}
void Filter::set_input(stream strm) {
  assert(this->fi->invariant());
  this->fi->inputs.clear();
  this->fi->itype.clear();
  this->fi->inputs.push_back(strm.data);
  this->fi->itype.push_back(strm.info);
  assert(this->fi->invariant());
}
stream Filter::input(size_t i) {
  assert(this->fi->invariant());
  if(i > this->fi->inputs.size()) {
    /// @todo this should be range_error or something... whatever vector throws.
    throw std::runtime_error("index out of bounds");
  }
  stream rv;
  rv.data = this->fi->inputs[i];
  rv.info = this->fi->itype[i];
  return rv;
}
const cstream Filter::input(size_t i) const {
  assert(this->fi->invariant());
  if(i > this->fi->inputs.size()) {
    /// @todo this should be range_error or something... whatever vector throws.
    throw std::runtime_error("index out of bounds");
  }
  cstream rv;
  rv.data = this->fi->inputs[i];
  rv.info = this->fi->itype[i];
  return rv;
}

void Filter::set_output(size_t i, std::shared_ptr<void> d, bstream_t sinfo) {
  if(this->fi->outputs.size() < (i+1)) {
    this->fi->outputs.resize(i+1);
  }
  if(this->fi->otype.size() < (i+1)) {
    this->fi->otype.resize(i+1);
  }
  this->fi->outputs[i] = d;
  this->fi->otype[i] = sinfo;
}

void Filter::set_output(size_t i, stream strm) {
  this->set_output(i, strm.data, strm.info);
}

stream Filter::output(size_t i) {
  assert(this->fi->invariant());
  if(i > this->fi->outputs.size()) {
    /// @todo this should be range_error or something... whatever vector throws.
    throw std::runtime_error("index out of bounds");
  }
  stream rv;
  rv.data = this->fi->outputs[i];
  rv.info = this->fi->otype[i];
  return rv;
}
const cstream Filter::output(size_t i) const {
  assert(this->fi->invariant());
  if(i > this->fi->outputs.size()) {
    /// @todo this should be range_error or something... whatever vector throws.
    throw std::runtime_error("index out of bounds");
  }
  cstream rv;
  rv.data = this->fi->outputs[i];
  rv.info = this->fi->otype[i];
  return rv;
}

}
