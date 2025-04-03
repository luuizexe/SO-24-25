#ifndef LISTCMD_H
#define LISTCMD_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define LNULL NULL
#define MAX_INPUT 200

typedef char tCmd[MAX_INPUT];

typedef int tCmdPos;
typedef struct tItemC
{
    tCmdPos pos;
    tCmd cmd;
} tItemC;

typedef struct tNodeC *tPosC;
typedef struct tNodeC
{
    tItemC data;
    tPosC next;
} tNodeC;
typedef tPosC tListC;

void createEmptyListC(tListC *L);
bool isEmptyListC(tListC L);
tPosC firstC(tListC L);
tPosC lastC(tListC L);
tPosC nextC(tPosC p);
tPosC previousC(tPosC p, tListC L);
bool insertItemC(tItemC d, tPosC p, tListC *L);
tItemC getItemC(tPosC p);
tPosC findItemC(int d, tListC L);
void deleteAtPositionC(tPosC p, tListC *L);
#endif
