#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

void ramdisk_read(void *, uint32_t, uint32_t);
size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  size_t len = get_ramdisk_size();
  ramdisk_read(DEFAULT_ENTRY, 0, len);
  return (uintptr_t)DEFAULT_ENTRY;

}
