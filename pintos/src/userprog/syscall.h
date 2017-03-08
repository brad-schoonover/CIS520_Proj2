#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include #include "filesys/file.h"

void syscall_init (void);

struct open_filedescriptor
{
  int fd;
  struct file *file_pointer;

  struct open_filedescriptor *next;
}

#endif /* userprog/syscall.h */
