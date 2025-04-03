#ifndef FILELIST_H
#define FILELIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#define LNULL NULL
#define MAX 500

typedef char tFile[MAX];
typedef int tMode;

typedef int tDf;
typedef struct tItemF
{
    tFile file;
    tMode mode;
    tDf df;
} tItemF;

typedef struct tNodeF *tPosF;
typedef struct tNodeF
{
    tItemF data;
    tPosF next;
} tNodeF;
typedef tPosF tListF;

void createEmptyListF(tListF *L);
bool isEmptyListF(tListF L);
tPosF firstF(tListF L);
tPosF lastF(tListF L);
tPosF nextF(tPosF p);
tPosF previousF(tPosF p, tListF L);
bool insertItemF(tItemF d, tPosF p, tListF *L);
void deleteAtPositionF(tPosF p, tListF *L);
tItemF getItemF(tPosF p);
tPosF findItemF(int d, tListF L);
void listOpenFiles(tListF lfich);
#endif
