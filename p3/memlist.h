#ifndef MEMLIST_H
#define MEMLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>

#define LNULL NULL
#define TAMANO 2048
#define MAX 500
typedef enum
{
    MALLOC,
    SHARED,
    CRSHARED,
    MMAP,
    OTRO
} tType;

typedef struct tItemM
{
    void *address;
    size_t tam;
    struct tm *talloc;
    tType type;
    key_t key;
    char name[MAX];
    int df;
} tItemM;

typedef struct tNodeM *tPosM;
typedef struct tNodeM
{
    tItemM data;
    tPosM next;
} tNodeM;
typedef tPosM tListM;

void createEmptyListM(tListM *L);
bool isEmptyListM(tListM L);
tPosM firstM(tListM L);
tPosM lastM(tListM L);
tPosM nextM(tPosM p);
tPosM previousM(tPosM p, tListM L);
bool insertItemM(tItemM d, tPosM p, tListM *L);
tItemM getItemM(tPosM p);
tPosM findItemByName(char d[], tListM L);
tPosM findItemByBytes(size_t n, tListM L);
tPosM findItemByKey(key_t n, tListM L);
tPosM findItemByAddress(void *d, tListM L);
int countBlocksWithKey(key_t clave, tListM L);
void deleteAtPositionM(tPosM p, tListM *L);
#endif
