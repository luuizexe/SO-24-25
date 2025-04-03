#ifndef FUNCIONES_P0_H
#define FUNCIONES_P0_H

#include <stdio.h>
#include <stdbool.h>
#include "cmdlist.h"
#include "filelist.h"
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>

bool procesarOpcionListado(char *input, bool *hayLink, bool *hayHid, bool *hayAcc, bool *hayLong, bool hayFile);
int trocearCadena(char *cadena, char *trozos[]);
void authors(char *input, int numtrozos);
void date(char *input, int numtrozos);
void pid();
void ppid();
void cwd();
void cd(char *input, int numtrozos);
void cmd_open(char *input[], tListF *lfich);
void cmd_close(char *input[], tListF *lfich);
void cmd_dup(char *input[], tListF *lfich);
void infosys();
void help(char *input, int numtrozos);

#endif
