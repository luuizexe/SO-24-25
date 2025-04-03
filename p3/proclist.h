#ifndef EXECLIST_H
#define EXECLIST_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#define LNULL NULL
typedef enum
{
    FINISHED,
    STOPPED,
    SIGNALED,
    ACTIVE,
    UNKNOWN
} tStatus;

typedef struct tItemP
{
    pid_t pid;
    struct tm *tlaunch;
    tStatus status;
    char *cmd;
    int priority;
    int signal;
} tItemP;

typedef struct tNodeP *tPosP;
typedef struct tNodeP
{
    tItemP data;
    tPosP next;
} tNodeP;
typedef tPosP tListP;

void createEmptyListP(tListP *L);
bool isEmptyListP(tListP L);
tPosP firstP(tListP L);
tPosP lastP(tListP L);
tPosP nextP(tPosP p);
tPosP previousP(tPosP p, tListP L);
bool insertItemP(tItemP d, tPosP p, tListP *L);
tItemP getItemP(tPosP p);
tPosP findItemP(pid_t pid, tListP L);
void deleteAtPositionP(tPosP p, tListP *L);
#endif
