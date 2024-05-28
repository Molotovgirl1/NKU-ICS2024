#include "common.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern int fs_open(const char *,int,int);
extern void fs_close(int);
extern size_t fs_filesz(int);
extern void fs_read(int ,void *,size_t);
extern void * new_page(void );

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  // ramdisk_read(DEFAULT_ENTRY, 0, RAMDISK_SIZE);
  int fd = fs_open(filename, 0, 0);
  Log("filename=%s,fd=%d",filename,fd);
  // fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  int size = fs_filesz(fd);
  int ppnum = size / PGSIZE;
  if(size % PGSIZE != 0) {
     ppnum++;
   }
  void *pa = NULL;
  void *va = DEFAULT_ENTRY;
  for(int i = 0; i < ppnum; i++) {
  pa = new_page();
  _map(as, va, pa);
  fs_read(fd, pa, PGSIZE);
  va += PGSIZE;
  }
 
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}