//========================================================================\
// ridefinizione funzioni libreria C                                      |
// void* accmalloc(ULONG cbBytes)                                         |
// void accfree(PVOID pvoid)                                              |
// char* accstrcpy(char* target, char *source)                            |
// unsigned long accstrlen(const char* s)
//
// *** 2016 note by author
// it would be better to replace these with vage c++ subsystem library
// functions modifying the code as needed
//========================================================================/

#include <os2.h>
#include "clib.h"

void* accmalloc(ULONG cbBytes) {
   PVOID pvoid;
   if (DosAllocMem(&pvoid, cbBytes, PAG_READ | PAG_WRITE | PAG_COMMIT))
      return NULL;
   return pvoid;
}

void accfree(PVOID pvoid) {
   DosFreeMem(pvoid);
}

//==========================================================================\
// copia source su target, ritorna un puntatore alla fine di target         |
//==========================================================================/

char* accstrcpy(char* target, char *source) {
   while (*target++ = *source++);
   return --target;
}


//==========================================================================\
// ritorna la lunghezza della stringa s                                     |
//==========================================================================/

unsigned long accstrlen(const char* s) {
   const char* o = s;
   while (*s) ++s;
   return s - o;
}

