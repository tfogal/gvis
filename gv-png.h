#ifndef GV_RW_PNG_H
#define GV_RW_PNG_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool writepng(const char* filename, const uint16_t* buf,
              uint32_t width, uint32_t height);
bool readpng(const char* filename, uint8_t** buf,
             uint32_t* width, uint32_t* height);

#ifdef __cplusplus
}
#endif
#endif
