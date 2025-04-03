#ifndef FUNCIONES_P2_H
#define FUNCIONES_P2_H

#include "cmdlist.h"
#include "filelist.h"
#include "memlist.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/wait.h> // pmap
#include "funciones_p1.h"

void allocate(char *input[], tListM *lmem, tListF *lfich);
void deallocate(char *input[], tListM *lmem, tListF *lfich);
void memfill(char *input[]);
void memdump(char *input[]);
void memory(char *input[], tListM lmem);
void Cmd_Read(char *input[], bool file);
void Cmd_Write(char *input[], bool file);
void recurse(int n);
#endif
