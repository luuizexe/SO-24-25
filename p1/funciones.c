#include "funciones.h"

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

void authors(char *input, int numtrozos)
{
	if (numtrozos == 1) // Caso en el que no recibe argumentos
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

void date(char *input, int numtrozos)
{
	time_t t;				  // Declaramos la variable de tipo time_t para guardar el tiempo
	struct tm *struct_t;	  // Declaramos un struct de tipo tm para desglosar fecha y hora
	time(&t);				  // Obtenemos el tiempo actual
	struct_t = localtime(&t); // Insertamos los datos en el struct
	char fyh[20];			  // Inicializamos el string en el que guardamos la hora y/o fecha

	if (numtrozos == 1) // Caso en el que no hay argumentos
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

void cwd()
{
	char cwd[MAX];
	getcwd(cwd, sizeof(cwd)); // Obtenemos el directorio actual
	printf("%s\n", cwd);
}

void cd(char *input, int numtrozos)
{
	if (numtrozos == 1) // Caso en el que no recibe argumentos
	{
		cwd();
	}
	else
	{
		if (chdir(input) != 0) // chdir devuelve 0 cuando encontro el directorio
		{
			perror("No existe directorio");
		}
	}
}

void listOpenFiles(tListF lfich)
{ // Funcion que recibe una lista de ficheros y la muestra por pantalla
	for (tPosF p = firstF(lfich); p != LNULL; p = nextF(p))
	{
		// Obtenemos los modos:
		char u[MAX] = "";
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
			strcat(u, " O_trUNC");
		printf("Descriptor: %d -> %s%s\n", d.df, d.file, u);
	}
}

void Cmd_open(char *input[], tListF *lfich)
{
	int i, df, mode = 0;
	if (input[1] == NULL || (df = atoi(input[1])) < 0) // Caso en el que no hay argumentos o el df es menor que 0
	{
		listOpenFiles(*lfich);
	}
	else
	{
		// Asignamos al fichero sus modos
		for (i = 2; input[i] != NULL; i++)
			if (!strcmp(input[i], "cr"))
				mode |= O_CREAT;
			else if (!strcmp(input[i], "ex"))
				mode |= O_EXCL;
			else if (!strcmp(input[i], "ro"))
				mode |= O_RDONLY;
			else if (!strcmp(input[i], "wo"))
				mode |= O_WRONLY;
			else if (!strcmp(input[i], "rw"))
				mode |= O_RDWR;
			else if (!strcmp(input[i], "ap"))
				mode |= O_APPEND;
			else if (!strcmp(input[i], "tr"))
				mode |= O_TRUNC;
			else
				break;

		// Ahora abrimos el fichero en cuestion
		if ((df = open(input[1], mode, 0777)) == -1)
			perror("Imposible abrir fichero");
		else
		{
			// Insertamos los datos del fichero en la lista
			tItemF e;
			e.mode = mode;
			e.df = df;
			strcpy(e.file, input[1]);
			if (insertItemF(e, LNULL, lfich))
			{
				printf("Entrada %d añadida a la tabla de ficheros abiertos\n", e.df);
			}
			else
			{
				printf("Error de insercion\n");
			}
		}
	}
}

void Cmd_close(char *input[], tListF *lfich)
{
	int df;
	if (input[1] == NULL || (df = atoi(input[1])) < 0) // Caso en el que no hay argumentos o el df es menor que 0
	{
		listOpenFiles(*lfich);
	}
	else
	{
		// Ahora cerramos el fichero en cuestion
		if (close(df) == -1)
			perror("Imposible cerrar descriptor");
		else
		{
			printf("Entrada %d eliminada de la lista de ficheros abiertos\n", df);
			deleteAtPositionF(findItemF(atoi(input[1]), *lfich), lfich);
		}
	}
}

void Cmd_dup(char *input[], tListF *lfich)
{
	int df, duplicado;

	if (input[1] == NULL || (df = atoi(input[1])) < 0) // Caso en el que no hay argumentos o es menor a 0
	{
		listOpenFiles(*lfich);
	}
	else
	{
		tPosF p = findItemF(df, *lfich); // Encontramos el elemento a duplicar
		if ((duplicado = dup(df)) == -1)
		{
			perror("Imposible duplicar descriptor");
		}
		else
		{
			// Ahora copiamos la informacion del fichero a su duplicado
			tItemF d = getItemF(p);
			tItemF e;
			e.mode = fcntl(duplicado, F_GETFL);
			e.df = duplicado;
			snprintf(e.file, MAX, "dup de %d (%.150s)", df, d.file); // Ademas del nombre, insertamos un identificador "duplicado"
			if (!insertItemF(e, LNULL, lfich))
			{
				printf("Error de insercion\n");
			}
			else
			{
				printf("Entrada %d duplicada y entrada %d añadida a la tabla de ficheros abiertos\n", d.df, e.df);
			}
		}
	}
}

void infosys()
{
	struct utsname sys; // Declaramos un struct de tipo utsname para desglosar la informacion de la maquina
	uname(&sys);		// Rellenamos dicho structf
	printf("%s (%s), OS: %s-%s-%s\n", sys.nodename, sys.machine, sys.sysname, sys.release, sys.version);
}

void help(char *input, int numtrozos)
{
	if (numtrozos == 1) // Caso en el que no hay argumentos: imprimimos comandos disponibles
	{
		printf("Comandos disponibles:\nauthors\npid\nppid\ncd\ndate\nhistoric\nopen\nclose\ndup\ninfosys\nquit\nexit\nbye\n"
			   "makefile\nmakedir\nlistfile\ncwd\nlistdir\nreclist\nrevlist\nerase\ndelrec\n");
	}
	else
	{ // Segun el comando introducido, mostramos su descripcion
		if (strcmp(input, "authors") == 0)
			printf("authors [-l|-n] Muestra nombre y correo de los autores\n[-l] Solo los correos\n[-n] Solo los nombres\n");
		else if (strcmp(input, "pid") == 0)
			printf("pid Muestra el pid del proceso del shell\n");
		else if (strcmp(input, "ppid") == 0)
			printf("ppid Muestra el pid del proceso padre del shell\n");
		else if (strcmp(input, "cd") == 0)
			printf("cd [dir] Cambia el directorio de trabajo a [dir]\nSi no se escribe [dir], muestra el directorio actual\n");
		else if (strcmp(input, "date") == 0)
			printf("date [-d|-t] Muestra la fecha actual en fomato DD/MM/AAAA y la hora actual en formato hh:mm:ss\n[-d] Solo la fecha\n[-t] Solo la hora\n");
		else if (strcmp(input, "historic") == 0)
			printf("historic [N|-N] Muestra el historial ordenado de comandos ejecutadose\n[N] Repite el N comando\n[-N] Muestra los ultimos N comandos\n");
		else if (strcmp(input, "open") == 0)
			printf("open [file] mode Abre el fichero y lo añade a la lista de ficheros abiertos\nSi no hay parametros, muestra la lista de archivos abiertos\n"
				   "\ncr: O_CREAT\nap: O_APPEND\nex: O_EXCL\nro: O_RDONLY\nrw: O_RDWR\nwo: O_WRONLY\ntr: O_TRUNC\n");
		else if (strcmp(input, "close") == 0)
			printf("close [df] Cierra el descriptor del archivo df y elimina el elemento correspondiente de la lista\n"
				   "Si no hay parametros, muestra la lista de archivos abiertos\n");
		else if (strcmp(input, "dup") == 0)
			printf("dup [df] Duplica el descriptor del archivo df\nSi no hay parametros, muestra la lista de archivos abiertos\n");
		else if (strcmp(input, "infosys") == 0)
			printf("infosys Muetra información de la maquina ejecutando el shell\n");
		else if (strcmp(input, "help") == 0)
			printf("help [C] Muestra una breve explicación del comando [C]\nSi no se escribe [C], muestra la lista de comandos disponibles\n");
		else if (strcmp(input, "quit") == 0)
			printf("quit Finaliza el shell\n");
		else if (strcmp(input, "exit") == 0)
			printf("exit Finaliza el shell\n");
		else if (strcmp(input, "bye") == 0)
			printf("bye Finaliza el shell\n");
		else if (strcmp(input, "makefile") == 0)
			printf("makefile [name]	Crea un fichero de nombre [name]\nSi no hay parametros, muestra el directorio actual\n");
		else if (strcmp(input, "makedir") == 0)
			printf("makedir [name] Crea un directorio de nombre [name]\nSi no hay parametros, muestra el directorio actual\n");
		else if (strcmp(input, "listfile") == 0)
			printf("listfile [-long][-link][-acc] name1 name2 .. Lista ficheros:\n"
				   "-long: listado largo\n-acc: accestime\n-linl: si es un enlace simbolico, el path contenido\n"
				   "Si no hay parametros, muestra el directorio actual\n");
		else if (strcmp(input, "cwd") == 0)
			printf("Muestra el directorio actul de la shell\n");
		else if (strcmp(input, "listdir") == 0)
			printf("listdir [-hid][-long][-link][-acc] n1 n2 .. Lista contenidos de directorios:\n"
				   "-hid: incluye los ficheros ocultos\n-long: listado largo\n-acc: accestime\n-linl: si es un enlace simbolico, el path contenido\n"
				   "Si no hay parametros, muestra el directorio actual\n");
		else if (strcmp(input, "reclist") == 0)
			printf("reclist [-hid][-long][-link][-acc] n1 n2 .. Lista recursivamente contenidos de directorios (subdirs despues):\n"
				   "-hid: incluye los ficheros ocultos\n-long: listado largo\n-acc: accestime\n-linl: si es un enlace simbolico, el path contenido\n"
				   "Si no hay parametros, muestra el directorio actual\n");
		else if (strcmp(input, "revlist") == 0)
			printf("revlist [-hid][-long][-link][-acc] n1 n2 .. Lista recursivamente contenidos de directorios (subdirs antes):\n"
				   "-hid: incluye los ficheros ocultos\n-long: listado largo\n-acc: accestime\n-linl: si es un enlace simbolico, el path contenido\n"
				   "Si no hay parametros, muestra el directorio actual\n");
		else if (strcmp(input, "erase") == 0)
			printf("erase [name 1 name2 ..] Elimina ficheros o directorios vacios\nSi no hay parametros, muestra el directorio actual\n");
		else if (strcmp(input, "delrec") == 0)
			printf("delrec [name1 name2 ..]	Elimina ficheros o directorios no vacios recursivamente\nSi no hay parametros, muestra el directorio actual\n");
		else
			printf("Argumento no valido\n");
	}
}

char LetraTF(mode_t m)
{
	switch (m & S_IFMT)
	{ /*and bit a bit con los bits de formato,0170000 */
	case S_IFSOCK:
		return 's'; /*socket */
	case S_IFLNK:
		return 'l'; /*symbolic link*/
	case S_IFREG:
		return '-'; /* fichero normal*/
	case S_IFBLK:
		return 'b'; /*block device*/
	case S_IFDIR:
		return 'd'; /*directorio */
	case S_IFCHR:
		return 'c'; /*char device*/
	case S_IFIFO:
		return 'p'; /*pipe*/
	default:
		return '?'; /*desconocido, no deberia aparecer*/
	}
}

char *ConvierteModo2(mode_t m)
{
	static char permisos[12];
	strcpy(permisos, "---------- ");

	permisos[0] = LetraTF(m);
	if (m & S_IRUSR)
		permisos[1] = 'r'; /*propietario*/
	if (m & S_IWUSR)
		permisos[2] = 'w';
	if (m & S_IXUSR)
		permisos[3] = 'x';
	if (m & S_IRGRP)
		permisos[4] = 'r'; /*grupo*/
	if (m & S_IWGRP)
		permisos[5] = 'w';
	if (m & S_IXGRP)
		permisos[6] = 'x';
	if (m & S_IROTH)
		permisos[7] = 'r'; /*resto*/
	if (m & S_IWOTH)
		permisos[8] = 'w';
	if (m & S_IXOTH)
		permisos[9] = 'x';
	if (m & S_ISUID)
		permisos[3] = 's'; /*setuid, setgid y stickybit*/
	if (m & S_ISGID)
		permisos[6] = 's';
	if (m & S_ISVTX)
		permisos[9] = 't';

	return permisos;
}

bool procesarOpcionListado(char *input, bool *hayLink, bool *hayHid, bool *hayAcc, bool *hayLong, bool hayFile)
{
	// Funcion que activa las diferentes banderas para mostrar datos

	if (hayFile) // Cuando hay al menos un archivo, no procesamos mas opciones ya
	{
		return false;
	}

	if (strcmp(input, "-link") == 0)
	{
		*hayLink = true;
	}
	else if (strcmp(input, "-hid") == 0 && hayHid != NULL) // En las funciones en las que no esta disponible esta etiqueta, pasamos NULL
	{
		*hayHid = true;
	}
	else if (strcmp(input, "-acc") == 0)
	{
		*hayAcc = true;
	}
	else if (strcmp(input, "-long") == 0)
	{
		*hayLong = true;
	}
	else // Si no activa ninguna bandera, devolvemos false
	{
		return false;
	}

	return true;
}

void manejarOpcionesEntrada(char *nombre, char *ruta, struct stat obj, bool hayLong, bool hayAcc, bool hayLink)
{
	// Funcion que segun los parametros introducidos, muestra por pantalla su correspondiente salida
	if (S_ISREG(obj.st_mode) || S_ISDIR(obj.st_mode) || S_ISLNK(obj.st_mode))
	{
		if (hayLong)
		{
			// Creamos los structs para meter los datos de usuario y grupo
			struct passwd *usuario;
			struct group *grupo;
			usuario = getpwuid(obj.st_uid);
			grupo = getgrgid(obj.st_uid);

			// Creamos un struct para introducir los datos del tiempo de modificacion
			struct tm *struct_t;
			struct_t = localtime(&obj.st_mtime);
			char mTime[20];
			strftime(mTime, 80, "%Y/%m/%d-%H:%M", struct_t);
			printf("%s%7ld (%7ld) %-10s%-10s %-10s",
				   mTime, obj.st_nlink, obj.st_ino, usuario->pw_name, grupo->gr_name,
				   ConvierteModo2(obj.st_mode));
		}
		else if (hayAcc)
		{
			// Creamos un struct para introducir los datos del tiempo de acceso
			struct tm *struct_t;
			struct_t = localtime(&obj.st_atime);
			char aTime[20];
			strftime(aTime, 80, "%Y/%m/%d-%H:%M", struct_t);
			printf("%s", aTime);
		}
		// Mostramos el tamaño en bytes y el nombre
		printf("%10ld %s", obj.st_size, nombre);
		if (hayLink)
		{
			char link_target[MAX];
			// Leemos el valor del enlace simbolico
			// Esta funcion necesita la ruta completa si no esta en la misma carpeta,
			// si estamos en ella, basta con el nombre
			int len = readlink(ruta != NULL ? ruta : nombre, link_target, sizeof(link_target) - 1);
			if (len != -1) // Si len es -1, no es un enlace simbolico
			{
				link_target[len] = '\0'; // Insertamos '\0' para indicar el final del string
				printf(" -> %s", link_target);
			}
		}
		printf("\n");
	}
}

void crearNuevoInput(char *input[], char *ruta, bool hayLink, bool hayHid, bool hayAcc, bool hayLong)
{
	// Funcion que segun las banderas activadas, escribe en un string troceado con la nueva direccion
	int i = 0;
	input[i++] = "";
	if (hayLink)
		input[i++] = "-link";
	if (hayHid)
		input[i++] = "-hid";
	if (hayAcc)
		input[i++] = "-acc";
	if (hayLong)
		input[i++] = "-long";
	input[i++] = ruta;
	input[i] = NULL;
}

void makefile(char *input, int numtrozos)
{
	if (numtrozos == 1) // Caso en el que no hay argumentos
	{
		cwd();
	}
	else if (numtrozos == 2) // Si hay argumentos, creamos el fichero [input]
	{
		if (open(input, O_CREAT, 0777) == -1)
			perror("Imposible abrir fichero");
	}
}

void makedir(char *input, int numtrozos)
{
	if (numtrozos == 1) // Caso en el que no hay argumentos
	{
		cwd();
	}
	else if (numtrozos == 2) // Si hay argumentos, creamos el directorio [input]
	{
		if (mkdir(input, 0777) == -1)
			perror("Imposible abrir fichero");
	}
	else
	{
		printf("Argumento no valido\n");
	}
}

void listfile(char *input[], int numtrozos)
{
	bool hayLink = false, hayAcc = false, hayLong = false, hayFile = false;
	int pos;
	// Creamos el struct para obtener los datos de la entrada
	struct stat obj;
	for (pos = 1; input[pos] != NULL; pos++)
	{
		// Si devuelve false, no hay mas etiquetas que leer
		if (!procesarOpcionListado(input[pos], &hayLink, NULL, &hayAcc, &hayLong, hayFile))
		{
			hayFile = true;
			// Rellenamos el struct con los datos de la entrada
			if (lstat(input[pos], &obj) == 0)
			{
				manejarOpcionesEntrada(input[pos], NULL, obj, hayLong, hayAcc, hayLink);
			}
			else // Si lstat no devuelve 0, significa que no existe el fichero/directorio
			{
				printf("Error al acceder a %s: %s\n", input[pos], strerror(errno));
			}
		}
	}
	// Si no se ha detectado ningun objeto, devolvemos cwd
	if (!hayFile)
	{
		cwd();
	}
}

void listdir(char *input[])
{
	bool hayLink = false, hayAcc = false, hayLong = false, hayHid = false, hayFile = false;
	int pos;
	// Creamos el struct para obtener los datos de la entrada
	struct stat obj;
	for (pos = 1; input[pos] != NULL; pos++)
	{
		// Si devuelve false, no hay mas etiquetas que leer
		if (!procesarOpcionListado(input[pos], &hayLink, &hayHid, &hayAcc, &hayLong, hayFile))
		{
			hayFile = true;
			// Rellenamos el struct con los datos de la entrada
			if (lstat(input[pos], &obj) == 0)
			{
				// Si es un fichero o un enlace, simplemente mostramos por pantalla
				if (S_ISREG(obj.st_mode) || S_ISLNK(obj.st_mode))
				{
					manejarOpcionesEntrada(input[pos], NULL, obj, hayLong, hayAcc, hayLink);
				}
				// Si es un directorio
				else if (S_ISDIR(obj.st_mode))
				{
					// Creamos el struct para meter los datos del directorio
					struct dirent *entradaDir;

					// Abrimos el directorio
					DIR *dir;
					dir = opendir(input[pos]);
					if (dir)
					{ // Mostramos el directorio actual y leemos su contenido
						printf("**********%s\n", input[pos]);
						while ((entradaDir = readdir(dir)) != NULL)
						{
							// Si -hid no está presente y el archivo es oculto (comienza con .), lo ignoramos
							if (hayHid || (!hayHid && entradaDir->d_name[0] != '.'))
							{
								// Obtenemos la ruta completa del archivo
								char ruta[MAX];
								snprintf(ruta, sizeof(ruta), "%s/%s", input[pos], entradaDir->d_name);
								if (lstat(ruta, &obj) == 0)
								{
									manejarOpcionesEntrada(entradaDir->d_name, ruta, obj, hayLong, hayAcc, hayLink);
								}
								else
								{
									printf("Error obteniendo información del archivo %s: %s\n", ruta, strerror(errno));
								}
							}
						}
						closedir(dir);
					}
					else
						printf("Error al abrir el directorio %s: %s\n", input[pos], strerror(errno));
				}
			}
			else // Si lstat no devuelve 0, significa que no existe el fichero/directorio
			{
				printf("Error al acceder a %s: %s\n", input[pos], strerror(errno));
			}
		}
	}
	// Si no se ha detectado ningun objeto, devolvemos cwd
	if (!hayFile)
	{
		cwd();
	}
}

void listarRec(char *input[], bool revlist)
{
	bool hayLink = false, hayAcc = false, hayLong = false, hayHid = false, hayFile = false;
	int pos;
	// Creamos el struct para obtener los datos de la entrada
	struct stat obj;

	// Creamos un string troceado que mas adelante llenaremos con la direccion de una subcarpeta
	// o cuando ingresamos varios directorios
	char *input2[MAX];

	for (pos = 1; input[pos] != NULL; pos++)
	{
		// Si devuelve false, no hay mas etiquetas que leer
		if (!procesarOpcionListado(input[pos], &hayLink, &hayHid, &hayAcc, &hayLong, hayFile))
		{
			hayFile = true;
			// Rellenamos el struct con los datos de la entrada
			if (lstat(input[pos], &obj) == 0)
			{
				// En caso de que sea reclist (opuesto a revlist), llamamos a listdir para mostrar
				// el contenido del directorio antes de mostrar las subcarpetas
				if (!revlist)
				{
					crearNuevoInput(input2, input[pos], hayLink, hayHid, hayAcc, hayLong);
					listdir(input2);
				}

				// Si es un directorio, lo abrimos y leemos su contenido
				if (S_ISDIR(obj.st_mode))
				{
					struct dirent *entradaDir;

					DIR *dir;
					dir = opendir(input[pos]);
					if (dir)
					{
						while ((entradaDir = readdir(dir)) != NULL)
						{
							// Si -hid no está presente y el archivo es oculto (comienza con .), lo ignoramos
							if (entradaDir->d_name[0] != '.')
							{
								// Obtenemos la ruta completa del archivo
								char ruta[MAX];
								snprintf(ruta, sizeof(ruta), "%s/%s", input[pos], entradaDir->d_name);
								if (lstat(ruta, &obj) == 0)
								{
									// Si es un directorio, creamos un nuevo input de la subcarpeta y llamamos de nuevo a reclist
									if (S_ISDIR(obj.st_mode))
									{
										char *input3[MAX];
										crearNuevoInput(input3, ruta, hayLink, hayHid, hayAcc, hayLong);
										listarRec(input3, revlist);
									}
								}
								else
								{
									printf("Error obteniendo información del archivo %s: %s\n", ruta, strerror(errno));
								}
							}
						}
						closedir(dir);
					}
					else
					{
						printf("Error al acceder a %s: %s\n", input[pos], strerror(errno));
					}
				}
				// En caso de que sea revlist (opuesto a reclist), llamamos a listdir para mostrar
				// el contenido del directorio despues de mostrar las subcarpetas
				if (revlist)
				{
					crearNuevoInput(input2, input[pos], hayLink, hayHid, hayAcc, hayLong);
					listdir(input2);
				}
			}
			else
			{
				printf("Error al accseder a %s: %s\n", input[pos], strerror(errno));
			}
		}
	}
	// Si no se ha detectado ningun objeto, devolvemos cwd
	if (!hayFile)
	{
		cwd();
	}
}

void erase(char *input[], int numtrozos)
{
	int pos;
	// Creamos el struct para obtener los datos de la entrada
	struct stat obj;
	if (numtrozos == 1) // Si se ha escrito erase solamente, llamamos a cwd
	{
		cwd();
	}
	else
	{
		for (pos = 1; input[pos] != NULL; pos++)
		{
			if (lstat(input[pos], &obj) == 0)
			{
				if (S_ISREG(obj.st_mode) || S_ISLNK(obj.st_mode))
				{
					if (remove(input[pos]) != 0)
					{
						printf("Imposible borrar %s: %s\n", input[pos], strerror(errno));
					}
				}
				else if (S_ISDIR(obj.st_mode))
				{
					if (rmdir(input[pos]) != 0)
					{
						printf("Imposible borrar %s: %s\n", input[pos], strerror(errno));
					}
				}
			}
			else
			{
				printf("Error al acceder a %s: %s\n", input[pos], strerror(errno));
			}
		}
	}
}

void delrec(char *input[], int numtrozos)
{
	int pos;
	// Creamos el struct para obtener los datos de la entrada
	struct stat obj;
	if (numtrozos == 1) // Si se ha escrito delrec solamente, llamamos a cwd
	{
		cwd();
	}
	else
	{
		for (pos = 1; input[pos] != NULL; pos++)
		{
			if (lstat(input[pos], &obj) == 0)
			{
				if (S_ISREG(obj.st_mode) || S_ISLNK(obj.st_mode))
				{
					if (remove(input[pos]) != 0)
					{
						printf("Imposible borrar %s: %s\n", input[pos], strerror(errno));
					}
				}
				else if (S_ISDIR(obj.st_mode))
				{
					if (rmdir(input[pos]) != 0)
					{
						struct dirent *entradaDir;

						// Abrimos el directorio
						DIR *dir;
						dir = opendir(input[pos]);

						// Leemos el contenido del directorio
						while ((entradaDir = readdir(dir)) != NULL)
						{
							if (entradaDir->d_name[0] != '.')
							{
								char ruta[MAX];
								snprintf(ruta, sizeof(ruta), "%s/%s", input[pos], entradaDir->d_name);
								char *input2[] = {"delrec", ruta, NULL};
								delrec(input2, 2);
							}
						}
						closedir(dir);
						if (rmdir(input[pos]) != 0)
						{
							printf("Imposible borrar el directorio %s: %s\n", input[pos], strerror(errno));
						}
					}
				}
			}
			else
				printf("Imposible borrar %s: %s\n", input[pos], strerror(errno));
		}
	}
}
