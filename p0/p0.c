#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "listcmd.h"
#include "listfile.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <ctype.h>
#include <fcntl.h>

int trocearCadena(char *cadena, char *trozos[])
{
	// Funcion que se encarga de trocear la entrada y devolver el numero de palabras
	int i = 1;
	if ((trozos[0] = strtok(cadena, " \n\t")) == NULL)
		return 0;
	while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
		i++;
	return i;
}
void procesarCadena(char *input, bool *terminar, tListC *lcom, tCmdPos *pos, tListF *lfich, int contRec);
void authors(char *input, int numTrozos)
{
	if (numTrozos == 1) // Caso en el que no recibe argumentos
		printf("Luis Alfonso Rodriguez Rodriguez: luis.rodriguezr@udc.es\nAlberto Paz Perez: a.pazp@udc.es\n");
	else
	{
		if (strcmp(input, "-l") == 0) // Caso [-l]: imprime logins
			printf("luis.rodriguezr@udc.es\na.pazp@udc.es\n");
		else if (strcmp(input, "-n") == 0) // Caso [-n]: Imprime nombres
			printf("Luis Alfonso Rodriguez Rodriguez\nAlberto Paz Perez\n");
		else
			printf("Argumento no valido\n"); // Caso en el que no se reconoce el argumento
	}
}
void date(char *input, int numTrozos)
{
	time_t t;				  // Declaramos la variable de tipo time_t para guardar el tiempo
	struct tm *struct_t;	  // Declaramos un struct de tipo tm para desglosar fecha y hora
	time(&t);				  // Obtenemos el tiempo actual
	struct_t = localtime(&t); // Insertamos los datos en el struct
	char fyh[20];			  // Inicializamos el string en el que guardamos la hora y/o fecha

	if (numTrozos == 1) // Caso en el que no hay argumentos
	{
		// Insertamos los datos de la fecha en el string para imprimirlo con el formato específico
		strftime(fyh, sizeof(fyh), "%d/%m/%Y", struct_t);
		printf("%s\n", fyh);
		// Insertamos los datos de la hora en el string para imprimirlo con el formato específico
		strftime(fyh, sizeof(fyh), "%X", struct_t);
		printf("%s\n", fyh);
	}
	else
	{
		if (strcmp(input, "-d") == 0) // Caso [-d]: solo la fecha
		{
			strftime(fyh, sizeof(fyh), "%d/%m/%Y", struct_t);
			printf("%s\n", fyh);
		}
		else if (strcmp(input, "-t") == 0) // Caso [-t]: solo la hora
		{
			strftime(fyh, sizeof(fyh), "%X", struct_t);
			printf("%s\n", fyh);
		}
		else
		{
			printf("Argumento no valido\n");
		}
	}
}
void pid()
{
	printf("El pid es %d\n", getpid()); // Imprimimos PID del proceso actual llamando a getpid()
}
void ppid()
{
	printf("El pid padre es %d\n", getppid()); // Imprimimos PID del proceso padre actual llamando a getppid()
}
void cd(char *input, int numTrozos)
{
	if (numTrozos == 1) // Caso en el que no recibe argumentos
	{
		char cwd[MAX_INPUT];
		getcwd(cwd, sizeof(cwd)); // Obtenemos el directorio actual
		printf("%s\n", cwd);
	}
	else
	{
		if (chdir(input) != 0) // chdir devuelve 0 cuando encontro el directorio
		{
			perror("No existe directorio");
		}
		else
			printf("%s\n", input);
	}
}
void historic(char *input, int numTrozos, tListC *lcom, bool *terminar, tCmdPos *pos, tListF *lfich, int contRec)
{
	if (numTrozos == 1) // Caso en el que no hay argumentos
	{
		// Imprimimos el historico de comandos
		for (tPosC i = firstC(*lcom); i != LNULL; i = nextC(i))
		{
			tItemC d;
			d = getItemC(i);
			printf("%d -> %s\n", d.pos, d.cmd);
		}
	}
	else
	{
		if (input[0] == '-' && isdigit(input[1])) // Caso [-N]: imprimimos los N ultimos comandos del historial
		{
			tCmdPos N = atoi(&input[1]); // convertimos el input a int
			if (N > 0)
			{
				tPosC i = lastC(*lcom);
				int cont;
				// Contador para retroceder N veces desde el final
				for (cont = 1; cont <= N && i != LNULL; cont++)
				{
					i = previousC(i, *lcom);
				}

				// Y a partir de ahi mostramos en orden
				while (nextC(i) != LNULL)
				{
					tItemC d = getItemC(i);
					printf("%d -> %s\n", d.pos, d.cmd);
					i = nextC(i);
				}
			}
		}
		else if (isdigit(input[0])) // Caso [N]: ejecutamos el comando N del historial de comandos
		{
			tPosC i = findItemC(atoi(input), *lcom);
			if (i != LNULL) // Verificamos que existe el comando que introducimos
			{
				tItemC d = getItemC(i);
				if (contRec < 10) // Comprobamos que no haya recursion
				{
					printf("Ejecutando: %d -> %s\n", d.pos, d.cmd);
					procesarCadena(d.cmd, terminar, lcom, pos, lfich, contRec + 1); // Llamamos a procesar cadena con el comando a ejecutar}
				}
				else
					printf("Cancelamos para evitar recursividad\n");
			}
			else
				printf("No existe tal comando\n");
		}
	}
}
void listOpenFiles(tListF lfich)
{ // Funcion que recibe una lista de ficheros y la muestra por pantalla
	for (tPosF p = firstF(lfich); p != LNULL; p = nextF(p))
	{
		char u[MAX_INPUT] = "";
		tItemF d = getItemF(p);
		if (d.mode & O_CREAT)
			strcat(u, " O_CREAT");
		if (d.mode & O_EXCL)
			strcat(u, " O_EXCL");
		if (d.mode & O_RDONLY)
			strcat(u, " O_RDONLY");
		if (d.mode & O_WRONLY)
			strcat(u, " O_WRONLY");
		if (d.mode & O_RDWR)
			strcat(u, " O_RDWR");
		if (d.mode & O_APPEND)
			strcat(u, " O_APPEND");
		if (d.mode & O_TRUNC)
			strcat(u, " O_TRUNC");
		printf("Descriptor: %d -> %s%s\n", d.df, d.file, u);
	}
}
void Cmd_open(char *tr[], tListF *lfich)
{
	int i, df, mode = 0;
	if (tr[1] == NULL || (df = atoi(tr[1])) < 0) // Caso en el que no hay argumentos o es menor a 0
	{
		listOpenFiles(*lfich);
		return;
	}
	// Asignamos al fichero sus modos
	for (i = 2; tr[i] != NULL; i++)
		if (!strcmp(tr[i], "cr"))
			mode |= O_CREAT;
		else if (!strcmp(tr[i], "ex"))
			mode |= O_EXCL;
		else if (!strcmp(tr[i], "ro"))
			mode |= O_RDONLY;
		else if (!strcmp(tr[i], "wo"))
			mode |= O_WRONLY;
		else if (!strcmp(tr[i], "rw"))
			mode |= O_RDWR;
		else if (!strcmp(tr[i], "ap"))
			mode |= O_APPEND;
		else if (!strcmp(tr[i], "tr"))
			mode |= O_TRUNC;
		else
			break;

	// Ahora abrimos el fichero en cuestion
	if ((df = open(tr[1], mode, 0777)) == -1)
		perror("Imposible abrir fichero");
	else
	{
		// Insertamos los datos del fichero en la lista
		tItemF e;
		e.mode = mode;
		e.df = df;
		strcpy(e.file, tr[1]);
		if (!insertItemF(e, nextF(lastF(*lfich)), lfich))
		{
			printf("Error de insercion\n");
		}
		else
		{
			printf("Anadida entrada %d a la tabla de ficheros abiertos\n", e.df);
		}
	}
}
void Cmd_close(char *tr[], tListF *lfich)
{
	int df;

	if (tr[1] == NULL || (df = atoi(tr[1])) < 0) // Caso en el que no hay argumentos o es menor a 0
	{
		listOpenFiles(*lfich);
		return;
	}

	// Ahora cerramos el fichero en cuestion
	if (close(df) == -1)
		perror("Imposible cerrar descriptor");
	else
	{
		printf("Eliminada entrada %d de la lista de ficheros abiertos\n", df);
		deleteAtPositionF(findItemF(atoi(tr[1]), *lfich), lfich);
	}
}
void Cmd_dup(char *tr[], tListF *lfich)
{
	int df, duplicado;

	if (tr[1] == NULL || (df = atoi(tr[1])) < 0) // Caso en el que no hay argumentos o es menor a 0
	{
		listOpenFiles(*lfich);
		return;
	}
	tPosF p = findItemF(df, *lfich); // Encontramos el elemento a duplicar
	if (p != LNULL)
	{
		duplicado = dup(df); // Nuevo df del duplicado

		// Ahora copiamos la informacion del fichero a su duplicado
		tItemF d = getItemF(p);
		tItemF e;
		e.mode = fcntl(duplicado, F_GETFL);
		e.df = duplicado;
		snprintf(e.file, MAX_INPUT, "dup %d (%.150s)", df, d.file); // Ademas del nombre, insertamos un identificador "duplicado"
		if (!insertItemF(e, nextF(lastF(*lfich)), lfich))
		{
			printf("Error de insercion\n");
		}
		else
		{
			printf("Duplicada entrada %d y añadida entrada %d a la tabla de ficheros abiertos\n", d.df, e.df);
		}
	}
	else
		printf("Argumento no valido\n");
}
void infosys()
{
	struct utsname sys; // Declaramos un struct de tipo utsname para desglosar la informacion de la maquina
	uname(&sys);		// Rellenamos dicho struct
	printf("%s (%s), OS: %s-%s-%s\n", sys.nodename, sys.machine, sys.sysname, sys.release, sys.version);
}
void help(char *input, int numTrozos)
{
	if (numTrozos == 1) // Caso en el que no hay argumentos: imprimimos comandos disponibles
	{
		printf("Comandos disponibles:\nauthors\npid\nppid\ncd\ndate\nhistoric\nopen\nclose\ndup\ninfosys\nquit\nexit\nbye\n");
	}
	else
	{ // Segun el comando introducido, mostramos su descripcion
		if (strcmp(input, "authors") == 0)
			printf("authors [-l|-n] Muestra nombre y correo de los autores\n[-l] Solo los correos\n[-n] Solo los nombres\n");
		if (strcmp(input, "pid") == 0)
			printf("pid Muestra el pid del proceso del shell\n");
		if (strcmp(input, "ppid") == 0)
			printf("ppid Muestra el pid del proceso padre del shell\n");
		if (strcmp(input, "cd") == 0)
			printf("cd [dir] Cambia el directorio de trabajo a [dir]\nSi no se escribe [dir], muestra el directorio actual\n");
		if (strcmp(input, "date") == 0)
			printf("date [-d|-t] Muestra la fecha actual en fomato DD/MM/AAAA y la hora actual en formato hh:mm:ss\n[-d] Solo la fecha\n[-t] Solo la hora\n");
		if (strcmp(input, "historic") == 0)
			printf("historic [N|-N] Muestra el historial ordenado de comandos ejecutadose\n[N] Repite el N comando\n[-N] Muestra los ultimos N comandos\n");
		if (strcmp(input, "open") == 0)
			printf("open [file] mode Muestra la lista de archivos abiertos\ncr O_CREAT\nap O_APPEND\nex O_EXCL\nro O_RDONLY\nrw O_RDWR\nwo O_WRONLY\ntr O_TRUNC\n");
		if (strcmp(input, "close") == 0)
			printf("close [df] Cierra el descriptor del archivo df y elimina el elemento correspondiente de la lista\n");
		if (strcmp(input, "dup") == 0)
			printf("dup [df] Duplica el descriptor del archivo df\n");
		if (strcmp(input, "infosys") == 0)
			printf("infosys Muetra información de la maquina ejecutando el shell\n");
		if (strcmp(input, "help") == 0)
			printf("help [C] Muestra una breve explicación del comando [C]\nSi no se escribe [C], muestra la lista de comandos disponibles\n");
		if (strcmp(input, "quit") == 0)
			printf("quit Finaliza el shell\n");
		if (strcmp(input, "exit") == 0)
			printf("exit Finaliza el shell\n");
		if (strcmp(input, "bye") == 0)
			printf("bye Finaliza el shell\n");
		else
			printf("Argumento no valido\n");
	}
}
void procesarCadena(char *input, bool *terminar, tListC *lcom, tCmdPos *pos, tListF *lfich, int contRec)
{
	char *trozos[MAX_INPUT]; // String para meter el input troceado
	char u[MAX_INPUT];		 // Declaramos otro string auxiliar para insertar el comando escrito sin trocear en el historial
	strcpy(u, input);
	int numTrozos = trocearCadena(input, trozos); // Lo troceamos
	if (numTrozos != 0)							  // Caso en el que se escribe algo
	{
		if (contRec == 0)
		{
			int l = strlen(u) - 1; // Obtenemos el tamaño del string auxiliar
			u[l] = '\0';		   // Insertamos '\0' para reemplazar el \n
			tItemC d;			   // Rellenamos el item de la lista de comandos
			strcpy(d.cmd, u);
			d.pos = *pos;

			if (isEmptyListC(*lcom)) // Si esta vacia, lo insertamos en la primera posicion
			{
				if (!insertItemC(d, *lcom, lcom))
				{
					printf("Error de insercion\n");
				}
			}
			else // Si no, al final
			{
				if (!insertItemC(d, nextC(lastC(*lcom)), lcom))
				{
					printf("Error de insercion\n");
				}
			}
			*pos += 1; // Aumentamos el contador de comandos
		}
		if (strcmp(trozos[0], "authors") == 0)
			authors(trozos[1], numTrozos);
		else if ((strcmp(trozos[0], "quit") == 0) || (strcmp(trozos[0], "bye") == 0) || (strcmp(trozos[0], "exit") == 0))
		{
			*terminar = true;
		}
		else if (strcmp(trozos[0], "pid") == 0)
		{
			pid();
		}
		else if (strcmp(trozos[0], "ppid") == 0)
		{
			ppid();
		}
		else if (strcmp(trozos[0], "cd") == 0)
		{
			cd(trozos[1], numTrozos);
		}
		else if (strcmp(trozos[0], "date") == 0)
		{
			date(trozos[1], numTrozos);
		}
		else if (strcmp(trozos[0], "historic") == 0)
		{
			historic(trozos[1], numTrozos, lcom, terminar, pos, lfich, contRec);
		}
		else if (strcmp(trozos[0], "open") == 0)
		{
			Cmd_open(trozos, lfich);
		}
		else if (strcmp(trozos[0], "close") == 0)
		{
			Cmd_close(trozos, lfich);
		}
		else if (strcmp(trozos[0], "dup") == 0)
		{
			Cmd_dup(trozos, lfich);
		}
		else if (strcmp(trozos[0], "infosys") == 0)
		{
			infosys();
		}
		else if (strcmp(trozos[0], "help") == 0)
		{
			help(trozos[1], numTrozos);
		}
		else
			printf("Comando no valido\n");
	}
}
int main()
{
	// Creamos la lista de comandos y de ficheros abiertos
	tListC lcom;
	createEmptyListC(&lcom);
	tListF lfich;
	createEmptyListF(&lfich);

	// Insertamos los archivos abiertos heredados a la lista
	tItemF e;
	for (int i = 0; i <= 2; i++)
	{
		e.df = i;
		if (i == 0)
		{
			strcpy(e.file, ("entrada estandar"));
		}
		else if (i == 1)
		{
			strcpy(e.file, ("salida estandar"));
		}
		else
			strcpy(e.file, ("error estandar"));
		e.mode = fcntl(i, F_GETFL);

		if (isEmptyListF(lfich)) // Si esta vacia, lo insertamos en la primera posicion
		{
			if (!insertItemF(e, lfich, &lfich))
			{
				printf("Error de insercion\n");
			}
		}
		else // Si no, al final
		{
			if (!insertItemF(e, nextF(lastF(lfich)), &lfich))
			{
				printf("Error de insercion\n");
			}
		}
	}
	tCmdPos pos = 0;	   // Contador de comandos
	char input[MAX_INPUT]; // String para la entrada de comandos
	bool terminar = false; // Booleano para determinar el fin del shell
	int contRec = 0;	   // Contador para evitar recursividad infinita
	// Entramos en el bucle del shell
	while (!terminar)
	{
		printf("-> ");
		fgets(input, sizeof(input), stdin);
		procesarCadena(input, &terminar, &lcom, &pos, &lfich, contRec);
	}
	// Una vez terminado el bucle, vaciamos ambas listas
	for (tPosC i = firstC(lcom); i != LNULL; i = firstC(lcom))
	{
		deleteAtPositionC(i, &lcom);
	}
	for (tPosF j = firstF(lfich); j != LNULL; j = firstF(lfich))
	{
		deleteAtPositionF(j, &lfich);
	}
}
