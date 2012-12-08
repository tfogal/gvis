#ifndef TJF_GVIS_BSTREAM_H
#define TJF_GVIS_BSTREAM_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum datatype {
  INTEGER,
  UNSIGNED_INTEGER,
  FLOAT
};

/** Describes a binary stream. */
typedef struct _tjf_bstream {
  enum datatype type;
  uint8_t width; /**< byte width, i.e. 2 bytes for uint16, etc. */
  uint16_t channels;
  uint64_t dimensions[4]; /**< x=0, z=2, time=3 */
} bstream_t;

/* streams can be invalid for a variety of reasons (3-byte widths? etc.).  This
 * checks a streams validity. */
bool valid_stream(const bstream_t);

#ifdef __cplusplus
}
#endif
#endif
