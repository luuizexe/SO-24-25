#ifndef DIRLIST_H
#define DIRLIST_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define LNULL NULL

typedef char *tDir;

typedef struct tNodeD *tPosD;
typedef struct tNodeD
{
    tDir dir;
    tPosD next;
} tNodeD;
typedef tPosD tListD;

void createEmptyListD(tListD *L);
bool isEmptyListD(tListD L);
tPosD firstD(tListD L);
tPosD lastD(tListD L);
tPosD nextD(tPosD p);
tPosD previousD(tPosD p, tListD L);
bool insertItemD(tDir d, tPosD p, tListD *L);
tDir getItemD(tPosD p);
tPosD findItemD(tDir d, tListD L);
void deleteAtPositionD(tPosD p, tListD *L);
#endif
