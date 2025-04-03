#ifndef FUNCIONES_P3_H
#define FUNCIONES_P3_H
#include "funciones_p1.h"
#include "funciones_p2.h"
#include "proclist.h"
#include "dirlist.h"
#include <sys/resource.h>
#include <signal.h>

void cmd_getuid();
void cmd_setuid(char *input[]);
void showvar(char *input[], char **environment);
void changevar(char *input[], char **environment);
void subsvar(char *input[], char **environment);
void environ(char *input[], char **environment);
void cmd_fork(char *input[], tListP *lproc);
void search(char *input[], tListD *ldir);
void exec(bool hayPri, char *input[], tListD *ldir, int numTrozos);
void fg(bool hayPri, bool esDefault, char *input[], tListD *ldir, int numTrozos, tListC *lcom, tListF *lfich, tListM *lmem, tListP *lproc);
void back(bool hayPri, char *input[], tListD *ldir, int numTrozos, tListC *lcom, tListF *lfich, tListM *lmem, tListP *lproc);
void listjobs(tListP *lproc);
void deljobs(char *input[], tListP *lproc);
void vaciarListas(tListC *lcom, tListF *lfich, tListM *lmem, tListP *lproc, tListD *ldir);
#endif
