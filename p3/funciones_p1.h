#ifndef FUNCIONES_P1_H
#define FUNCIONES_P1_H

#include "funciones_p0.h"
#include <sys/stat.h> // mkdir
#include <pwd.h>      //usuario
#include <grp.h>      //grupo
#include <errno.h>    // strerror
#include <dirent.h>   // directorios

#define MAX 500

void makefile(char *input, int numtrozos);
void makedir(char *input, int numtrozos);
void listfile(char *input[], int numtrozos);
void listdir(char *input[]);
void erase(char *input[], int numtrozos);
void delrec(char *input[], int numtrozos);
void listarRec(char *input[], bool revlist);

#endif
