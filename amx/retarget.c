#include <stdio.h>
#include "amx_svc.h"

#if defined __GNUC__
int _write (int fd, char *ptr, int len)
{
  /* Write "len" of char from "ptr" to file id "fd"
   * Return number of char written.
   * Need implementing with UART here. */
  return Pawn_DebugTx(ptr, len);
}

int _read (int fd, char *ptr, int len)
{
  /* Read "len" of char to "ptr" from file id "fd"
   * Return number of char read. */
  return 0;
}

void _ttywrch(int ch) {
  /* Write one char "ch" to the default console. */
  char tempch = ch;  // temp char avoids endianness issue
  Pawn_DebugTx(&tempch, 1); // Pawn_DebugTx(&ch, 1) would not work everywhere
}

int puts(const char *ptr)
{
  const char* tmp = ptr;
  int len = 0;

  while(*tmp != '\0')
  {
    tmp++; len++;
  }
  return Pawn_DebugTx(ptr, len);
}

#else

int fputc(int ch, FILE *f)
{
  char tempch = ch;  // temp char avoids endianness issue
  Pawn_DebugTx(&tempch, 1); // Pawn_DebugTx(&ch, 1) would not work everywhere
  return ch;
}
int ferror(FILE *f)
{
  /* Your implementation of ferror(). */
  return 0;
}

/*
 * Call _init_alloc, and define __rt_heap_extend, if you are trying
 * to run a heap on the bare metal.
 */
size_t __rt_heap_extend(size_t size, void ** block)
{
  /* not implemented */
  return 0;
}

/*
 * Redefine this function to replace the entire signal handling mechanism for the library.
 * The default implementation calls __raise().
 */
void __rt_raise(int signal, int type)
{
  __BKPT(0);
}

#endif
