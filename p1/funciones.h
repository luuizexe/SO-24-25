#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include "cmdlist.h"
#include "filelist.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>    //open/close/dup
#include <sys/stat.h> // mkdir
#include <pwd.h>      //usuario
#include <grp.h>      //grupo
#include <errno.h>    // strerror
#include <dirent.h>   // directorios

char LetraTF(mode_t m);
char *ConvierteModo2(mode_t m);
bool procesarOpcionListado(char *input, bool *hayLink, bool *hayHid, bool *hayAcc, bool *hayLong, bool hayFile);
int trocearCadena(char *cadena, char *trozos[]);
void authors(char *input, int numtrozos);
void date(char *input, int numtrozos);
void pid();
void ppid();
void cwd();
void cd(char *input, int numtrozos);
void listOpenFiles(tListF lfich);
void Cmd_open(char *input[], tListF *lfich);
void Cmd_close(char *input[], tListF *lfich);
void Cmd_dup(char *input[], tListF *lfich);
void infosys();
void help(char *input, int numtrozos);
void makefile(char *input, int numtrozos);
void makedir(char *input, int numtrozos);
void manejarOpcionesEntrada(char *nombre, char *ruta, struct stat obj, bool hayLong, bool hayAcc, bool hayLink);
void listfile(char *input[], int numtrozos);
void listdir(char *input[]);
void erase(char *input[], int numtrozos);
void delrec(char *input[], int numtrozos);
void listarRec(char *input[], bool revlist);
#endif
