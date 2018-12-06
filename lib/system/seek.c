#include <kernel.h>

uint32_t
seek(uint32_t fd, uint32_t offset, uint8_t what) {
  // what:  SEEK_SET, cursor at offset
  //        SEEK_CUR, cursor at cursor + offset
  //        SEEK_END, cursor at end of file
}
