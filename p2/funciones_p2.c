#include "funciones_p2.h"

// Variables globales para la funcion memory -vars
int globalA;
int globalB;
int globalC;
int iniGlobalA = 4;
int iniGlobalB = 5;
int iniGlobalC = 6;

void listMem(tListM lmem, tType param)
{
	// Funcion para mostrar por pantalla la lista de bloques de memoria
	char paramAux[15];
	if (param == MALLOC)
	{
		strcpy(paramAux, " malloc");
	}
	else if (param == CRSHARED)
	{
		strcpy(paramAux, " createshared");
		param = SHARED;
	}
	else if (param == SHARED)
	{
		strcpy(paramAux, " shared");
	}
	else if (param == MMAP)
	{
		strcpy(paramAux, " mmap");
	}
	else if (param == OTRO)
	{
		strcpy(paramAux, "");
	}

	printf("***Lista de bloques asignados%s para el proceso %d\n", paramAux, getpid());
	if (!isEmptyListM(lmem))
	{
		for (tPosM i = firstM(lmem); i != LNULL; i = nextM(i))
		{
			tType t = i->data.type;
			if (param == OTRO || param == t)
			{
				// Imprimimos si no hay parametros
				// O si el tipo introducido es el que corresponde
				char fyh[20];
				strftime(fyh, sizeof(fyh), "%b %d  %H:%M", i->data.talloc);
				printf("%20p %5ld %s", i->data.address, i->data.tam, fyh);
				if (t == MMAP)
				{
					printf(" %s (descriptor %d)", i->data.name, i->data.df);
				}
				else if (t == MALLOC)
				{
					printf(" malloc");
				}
				else if (t == SHARED)
				{
					printf(" shared (key %d)", i->data.key);
				}
				printf("\n");
			}
		}
	}
}

void *ObtenerMemoriaShmget(key_t clave, size_t tam, tListM *lmem)
{
	void *p;
	int aux, id, flags = 0777;
	struct shmid_ds s;

	if (tam)								  /*tam distito de 0 indica crear */
		flags = flags | IPC_CREAT | IPC_EXCL; /*cuando no es crear pasamos de tamano 0*/
	if (clave == IPC_PRIVATE)				  /*no nos vale*/
	{
		errno = EINVAL;
		return NULL;
	}
	if ((id = shmget(clave, tam, flags)) == -1)
		return (NULL);
	if ((p = shmat(id, NULL, 0)) == (void *)-1)
	{
		aux = errno;
		if (tam)
			shmctl(id, IPC_RMID, NULL);
		errno = aux;
		return (NULL);
	}
	shmctl(id, IPC_STAT, &s); /* si no es crear, necesitamos el tamano, que es s.shm_segsz*/

	// Inicializamos el item y lo insertamos en su lista
	tItemM d;
	d.address = p;
	d.tam = s.shm_segsz;
	d.type = SHARED;
	time_t t;
	time(&t);
	d.talloc = localtime(&t);
	d.key = clave;
	d.name[0] = '\0';
	d.df = -1;
	if (!insertItemM(d, NULL, lmem))
	{
		printf("Error al insertar\n");
	}
	return (p);
}

void do_AllocateCreateshared(char *input[], tListM *lmem)
{
	key_t cl;
	size_t tam;
	void *p;

	if (input[2] == NULL || input[3] == NULL)
	{
		// Mostramos la lista de bloques de memoria createshared
		listMem(*lmem, CRSHARED);
		return;
	}

	// Pasamos de string a tipo Key y a tipo Size
	cl = (key_t)strtoul(input[2], NULL, 10);
	tam = (size_t)strtoul(input[3], NULL, 10);
	if (tam == 0)
	{
		printf("No se asignan bloques de 0 bytes\n");
		return;
	}
	if ((p = ObtenerMemoriaShmget(cl, tam, lmem)) != NULL)
		printf("Asignados %lu bytes en %p\n", (unsigned long)tam, p);
	else
		printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long)cl, strerror(errno));
}

void do_AllocateShared(char *input[], tListM *lmem)
{
	key_t cl;
	void *p;

	if (input[2] == NULL)
	{
		// Mostramos la lista de bloques de memoria shared
		listMem(*lmem, SHARED);
		return;
	}

	// Pasamos de string a tipo Key
	cl = (key_t)strtoul(input[2], NULL, 10);

	if ((p = ObtenerMemoriaShmget(cl, 0, lmem)) != NULL)
	{
		printf("Asignada memoria compartida de clave %lu en %p\n", (unsigned long)cl, p);
	}
	else
		printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long)cl, strerror(errno));
}

void *MapearFichero(char *fichero, int protection, tListM *lmem, tListF *lfich)
{
	int df, map = MAP_PRIVATE, modo = O_RDONLY;
	struct stat s;
	void *p;

	if (protection & PROT_WRITE)
		modo = O_RDWR;
	if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1)
		return NULL;
	if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED)
	{
		close(df);
		return NULL;
	}

	// Inicializamos el item y lo insertamos en su lista
	tItemM d;
	d.address = p;
	d.tam = s.st_size;
	d.type = MMAP;
	strcpy(d.name, fichero);
	d.df = df;
	time_t t;
	time(&t);
	d.talloc = localtime(&t);
	d.key = 0;
	if (insertItemM(d, NULL, lmem))
	{
		// Tambien lo insertamos en la lista de ficheros abiertos
		tItemF e;
		e.mode = modo;
		e.df = df;
		snprintf(e.file, MAX, "mapeo de %.150s", fichero);
		if (!insertItemF(e, NULL, lfich))
		{
			printf("Error al insertar\n");
		}
	}
	else
	{
		printf("Error al insertar\n");
	}
	return p;
}

void do_AllocateMmap(char *input[], tListM *lmem, tListF *lfich)
{
	char *perm;
	void *p;
	int protection = 0;

	if (input[2] == NULL)
	{
		// Mostramos la lista de bloques de memoria mmap
		listMem(*lmem, MMAP);
		return;
	}
	if ((perm = input[3]) != NULL && strlen(perm) < 4)
	{
		if (strchr(perm, 'r') != NULL)
			protection |= PROT_READ;
		if (strchr(perm, 'w') != NULL)
			protection |= PROT_WRITE;
		if (strchr(perm, 'x') != NULL)
			protection |= PROT_EXEC;
	}
	if ((p = MapearFichero(input[2], protection, lmem, lfich)) == NULL)
		perror("Imposible mapear fichero");
	else
	{
		printf("Fichero %s mapeado en %p\n", input[2], p);
	}
}

void do_AllocateMalloc(char *input[], tListM *lmem)
{
	if (input[2] == NULL)
	{
		// Mostramos la lista de bloques de memoria malloc
		listMem(*lmem, MALLOC);
	}
	else
	{
		size_t tam = atoi(input[2]);
		if (tam <= 0)
		{
			printf("No se asignan bloques de 0 bytes\n");
			return;
		}
		// Reservamos el espacio tam
		void *p = malloc(tam);

		if (p == NULL)
		{
			// Significa que no hay memoria disponible
			fprintf(stderr, "Imposible asignar memoria\n");
			return;
		}

		// Inicializamos el item y lo insertamos en su lista
		tItemM d;
		d.address = p;
		d.tam = tam;
		d.type = MALLOC;
		time_t t;
		time(&t);
		d.talloc = localtime(&t);
		d.key = 0;
		d.name[0] = '\0';
		d.df = -1;
		if (insertItemM(d, NULL, lmem))
		{
			printf("Asignados %ld bytes en %p\n", tam, p);
		}
		else
		{
			printf("Error al insertar\n");
		}
	}
}

void allocate(char *input[], tListM *lmem, tListF *lfich)
{
	if (input[1] != NULL)
	{
		// Segun el parametro introducido se hace un allocate diferente
		if (strcmp(input[1], "-malloc") == 0)
		{
			do_AllocateMalloc(input, lmem);
		}
		else if (strcmp(input[1], "-shared") == 0)
		{
			do_AllocateShared(input, lmem);
		}
		else if (strcmp(input[1], "-createshared") == 0)
		{
			do_AllocateCreateshared(input, lmem);
		}
		else if (strcmp(input[1], "-mmap") == 0)
		{
			do_AllocateMmap(input, lmem, lfich);
		}
		else
		{
			printf("uso: allocate [-malloc|-shared|-createshared|-mmap] ....\n");
		}
	}
	else
	{
		// Mostramos toda la lista de bloques de memoria
		listMem(*lmem, OTRO);
	}
}

void do_DeallocateDelKey(char *input[])
{
	key_t clave;
	int id;
	char *key = input[2];

	if (key == NULL || (clave = (key_t)strtoul(key, NULL, 10)) == IPC_PRIVATE)
	{
		printf("delkey necesita clave valida\n");
		return;
	}
	if ((id = shmget(clave, 0, 0666)) == -1)
	{
		perror("shmget: imposible obtener memoria compartida");
		return;
	}
	if (shmctl(id, IPC_RMID, NULL) == -1)
		perror("shmctl: imposible eliminar memoria compartida\n");
}

void do_DeallocateMalloc(char *input[], tListM *lmem)
{
	if (input[2] == NULL)
	{
		// Mostramos la lista de bloques de memoria malloc
		listMem(*lmem, MALLOC);
	}
	else
	{
		size_t tam = atoi(input[2]);
		if (tam <= 0)
		{
			printf("No se asignan bloques de 0 bytes\n");
			return;
		}
		// Buscamos el item de tamaño tam en la lista
		tPosM p = findItemByBytes(tam, *lmem);
		if (p != NULL)
		{
			// Si lo encontramos,liberamos memoria y lo quitamos de la lista
			free(p->data.address);
			deleteAtPositionM(p, lmem);
		}
		else
		{
			printf("No hay bloque de ese tamaño asignado con malloc\n");
		}
	}
}

void do_DeallocateShared(char *input[], tListM *lmem)
{
	key_t clave;
	int id;
	if (input[2] == NULL)
	{
		// Mostramos la lista de bloques de memoria shared
		listMem(*lmem, SHARED);
	}
	else if ((clave = (key_t)strtoul(input[2], NULL, 10)) != IPC_PRIVATE)
	{
		// Buscamos el item con la clave key en la lista
		tPosM p = findItemByKey(clave, *lmem);
		if (p != NULL)
		{
			if (shmdt(p->data.address) == -1)
			{
				perror("shmdt: imposible desacoplar memoria compartida\n");
				return;
			}

			// Si solo queda un bloque con esa clave, la eliminamos
			if (countBlocksWithKey(clave, *lmem) == 1)
			{
				if ((id = shmget(clave, 0, 0666)) == -1)
				{
					perror("shmget: imposible obtener memoria compartida\n");
					return;
				}
				if (shmctl(id, IPC_RMID, NULL) == -1)
				{
					perror("shmctl: imposible eliminar memoria compartida\n");
					return;
				}
			}
			// Y la quitamos de la lista de memoria
			deleteAtPositionM(p, lmem);
		}
		else
		{
			printf("No hay bloque de esa clave mapeado en el proceso\n");
		}
	}
}

void do_DeallocateMmap(char *input[], tListM *lmem, tListF *lfich)
{
	if (input[2] == NULL)
	{
		// Mostramos la lista de bloques de memoria mmap
		listMem(*lmem, MMAP);
	}
	else
	{
		// Buscamos el item con el nombre introducido
		tPosM p = findItemByName(input[2], *lmem);
		if (p != NULL)
		{
			if (munmap(p->data.address, p->data.tam) == -1)
			{
				perror("Error al desmapear la memoria\n");
				return;
			}
			if (close(p->data.df) == -1)
				perror("Imposible cerrar descriptor");
			else
			{
				// Lo quitamos de la lista de ficheros abiertos
				deleteAtPositionF(findItemF((p->data.df), *lfich), lfich);
			}
			// Y luego de la de memoria
			deleteAtPositionM(p, lmem);
		}
		else
		{
			printf("Fichero %s no mapeado\n", input[2]);
		}
	}
}

void do_DeallocateAddr(char *input[], tListM *lmem, tListF *lfich)
{
	if (input[1] == NULL)
	{
		// Mostramos todos los bloques de memoria
		listMem(*lmem, OTRO);
	}
	else
	{
		void *address = (void *)strtoull(input[1], NULL, 16);
		tPosM p = findItemByAddress(address, *lmem);
		if (p != NULL)
		{
			// Segun el type, se hace un deallocate diferente
			char *aux[4] = {NULL, NULL, NULL, NULL};
			if (p->data.type == MALLOC)
			{
				free(p->data.address);
				deleteAtPositionM(p, lmem);
			}
			else if (p->data.type == SHARED)
			{
				aux[1] = "-shared";
				aux[2] = malloc(20 * sizeof(char));
				snprintf(aux[2], 20, "%d", p->data.key);
				do_DeallocateShared(aux, lmem);
				free(aux[2]);
			}
			else if (p->data.type == MMAP)
			{
				aux[1] = "-mmap";
				aux[2] = malloc(MAX * sizeof(char));
				strcpy(aux[2], p->data.name);
				do_DeallocateMmap(aux, lmem, lfich);
				free(aux[2]);
			}
		}
		else
		{
			printf("Direccion %p no asignada con malloc, shared o mmap\n", address);
		}
	}
}

void deallocate(char *input[], tListM *lmem, tListF *lfich)
{
	if (input[1] != NULL)
	{
		// Segun el parametro introducido se hace un deallocate diferente
		if (strcmp(input[1], "-malloc") == 0)
		{
			do_DeallocateMalloc(input, lmem);
		}
		else if (strcmp(input[1], "-shared") == 0)
		{
			do_DeallocateShared(input, lmem);
		}
		else if (strcmp(input[1], "-delkey") == 0)
		{
			do_DeallocateDelKey(input);
		}
		else if (strcmp(input[1], "-mmap") == 0)
		{
			do_DeallocateMmap(input, lmem, lfich);
		}
		else
		{
			do_DeallocateAddr(input, lmem, lfich);
		}
	}
	else
	{
		// Mostramos todos los bloques de memoria
		listMem(*lmem, OTRO);
	}
}

void LlenarMemoria(void *p, size_t cont, unsigned char byte)
{
	unsigned char *arr = (unsigned char *)p;
	size_t i;

	for (i = 0; i < cont; i++)
		arr[i] = byte;
}

void memfill(char *input[])
{
	if (input[1] != NULL)
	{
		void *address = (void *)strtoull(input[1], NULL, 16);
		size_t cont = (input[2] != NULL) ? strtoull(input[2], NULL, 10) : 128;
		unsigned char byte = (input[3] != NULL) ? input[3][0] : 'A';
		printf("Llenando %ld bytes de memoria con el byte %c(%02X) a partir de la direccion %p\n", cont, byte, byte, address);
		LlenarMemoria(address, cont, byte);
	}
}

void memdump(char *input[])
{
	if (input[1] != NULL)
	{
		void *address = (void *)strtoull(input[1], NULL, 16);
		size_t cont = (input[2] != NULL) ? strtoull(input[2], NULL, 10) : 25;
		unsigned char *byte = (unsigned char *)address;
		printf("Volcando %ld bytes desde la direccion %p\n", cont, address);
		const size_t bytesPorLinea = 25;
		size_t i = 0;

		for (size_t inicio = 0; inicio < cont; inicio += bytesPorLinea)
		{
			for (i = inicio; i < (inicio + bytesPorLinea) && i < cont; i++)
			{
				// El "\n", "\t" y "\r" debemos escaparlo para que se impriman bien
				if (byte[i] == '\n')
				{
					printf(" \\n ");
				}
				else if (byte[i] == '\t')
				{
					printf(" \\t ");
				}
				else if (byte[i] == '\r')
				{
					printf(" \\r ");
				}
				else if (isprint(byte[i])) // Si es un caracter printeable, lo mostramos
				{
					printf(" %c  ", byte[i]);
				}
				else // Si no, dejamos espacio en blanco
				{
					printf("    ");
				}
			}
			printf("\n");

			for (i = inicio; i < (inicio + bytesPorLinea) && i < cont; i++)
			{
				printf(" %02X ", byte[i]);
			}
			printf("\n");
		}
	}
}

void do_MemoryFuncs()
{
	printf("Funciones programa\t\t%p,\t%p,\t%p", (void *)makedir, (void *)erase, (void *)listarRec);
	printf("\nFunciones libreria\t\t%p,\t%p,\t%p\n", (void *)printf, (void *)scanf, (void *)malloc);
}

void do_MemoryVars()
{
	// Variables locales
	int localA = 10;
	char localB = 'x';
	double localC = 20;
	// Variables estaticas
	static int staticA;
	static int staticB;
	static int staticC;
	// Variables estaticas inicializadas
	static int iniStaticA = 1;
	static int iniStaticB = 2;
	static int iniStaticC = 3;
	printf("Variables locales\t\t%p,\t%p,\t%p\n", (void *)&localA, (void *)&localB, (void *)&localC);
	printf("Variables globales\t\t%p,\t%p,\t%p\n", (void *)&globalA, (void *)&globalB, (void *)&globalC);
	printf("Var (N.I.) globales\t\t%p,\t%p,\t%p\n", (void *)&iniGlobalA, (void *)&iniGlobalB, (void *)&iniGlobalC);
	printf("Variables estaticas\t\t%p,\t%p,\t%p\n", (void *)&staticA, (void *)&staticB, (void *)&staticC);
	printf("Variables (N.I.) estaticas\t%p,\t%p,\t%p\n", (void *)&iniStaticA, (void *)&iniStaticB, (void *)&iniStaticC);
}

void do_MemoryPmap()
{
	pid_t pid; /*Hace el pmap (o equivalente) del proceso actual*/
	char elpid[32];
	char *argv[4] = {"pmap", elpid, NULL};

	sprintf(elpid, "%d", (int)getpid());
	if ((pid = fork()) == -1)
	{
		perror("Imposible crear proceso");
		return;
	}
	if (pid == 0)
	{
		if (execvp(argv[0], argv) == -1)
			perror("cannot execute pmap (linux, solaris)");

		argv[0] = "procstat";
		argv[1] = "vm";
		argv[2] = elpid;
		argv[3] = NULL;
		if (execvp(argv[0], argv) == -1) /*No hay pmap, probamos procstat FreeBSD */
			perror("cannot execute procstat (FreeBSD)");

		argv[0] = "procmap", argv[1] = elpid;
		argv[2] = NULL;
		if (execvp(argv[0], argv) == -1) /*probamos procmap OpenBSD*/
			perror("cannot execute procmap (OpenBSD)");

		argv[0] = "vmmap";
		argv[1] = "-interleave";
		argv[2] = elpid;
		argv[3] = NULL;
		if (execvp(argv[0], argv) == -1) /*probamos vmmap Mac-OS*/
			perror("cannot execute vmmap (Mac-OS)");
		exit(1);
	}
	waitpid(pid, NULL, 0);
}

void memory(char *input[], tListM lmem)
{
	if (input[1] != NULL)
	{
		if (strcmp(input[1], "-blocks") == 0)
		{
			// Mostramos todos los bloques de memoria
			listMem(lmem, OTRO);
		}
		else if (strcmp(input[1], "-funcs") == 0)
		{
			do_MemoryFuncs();
		}
		else if (strcmp(input[1], "-vars") == 0)
		{
			do_MemoryVars();
		}
		else if (strcmp(input[1], "-all") == 0)
		{
			do_MemoryVars();
			do_MemoryFuncs();
			listMem(lmem, OTRO);
		}
		else if (strcmp(input[1], "-pmap") == 0)
		{
			do_MemoryPmap();
		}
		else
		{
			printf("Opcion %s no contemplada\n", input[1]);
		}
	}
	else
	{
		// Si no se especifica, se hace lo mismo que "-all"
		do_MemoryVars();
		do_MemoryFuncs();
		// Mostramos todos los bloques de memoria
		listMem(lmem, OTRO);
	}
}

ssize_t LeerFichero(char *f, void *p, size_t cont, bool file)
{
	struct stat s;
	ssize_t n;
	int df, aux;

	if (file)
	{
		// Pasamos un archivo por parametro
		if (stat(f, &s) == -1 || (df = open(f, O_RDONLY)) == -1)
			return -1;

		// Si el contador es mayor que st_size lo leemos entero
		if (s.st_size < cont)
			cont = s.st_size;
	}
	else
	{
		// Si file es false, significa que se ha pasado df por parametro
		df = atoi(f);
		if (df < 0)
		{
			return -1;
		}
	}

	if (file && cont == -1) /* si pasamos -1 como bytes a leer lo leemos entero*/
		cont = s.st_size;

	if ((n = read(df, p, cont)) == -1)
	{
		aux = errno;
		if (file)
			close(df);

		errno = aux;
		return -1;
	}
	if (file)
		close(df);
	return n;
}

ssize_t EscribirFichero(char *f, void *p, size_t cont, bool file, bool overwrite)
{
	ssize_t n;
	int df, aux, perms = O_CREAT | O_EXCL | O_WRONLY;

	if (file)
	{
		// Si es overwrite cambiamos los permisos
		if (overwrite)
			perms = O_CREAT | O_WRONLY | O_TRUNC;

		if ((df = open(f, perms, 0777)) == -1)
			return -1;
	}
	else
	{
		// Si file es false, significa que se ha pasado df por parametro
		df = atoi(f);
		if (df < 0)
		{
			return -1;
		}
	}

	// Importante que si no tiene las flags para escribir fallará
	if ((n = write(df, p, cont)) == -1)
	{
		aux = errno;

		if (file)
			close(df);

		errno = aux;
		return -1;
	}
	if (file)
		close(df);
	return n;
}

void Cmd_Read(char *input[], bool file)
{
	void *p;
	size_t cont = -1;
	/* si no pasamos tamano se lee entero */
	ssize_t n;

	if (input[1] == NULL || input[2] == NULL)
	{
		printf("faltan parametros\n");
		return;
	}

	p = (void *)strtoull(input[2], NULL, 16); /* convertimos de cadena a puntero */
	if (input[3] != NULL)
		cont = (size_t)atoll(input[3]);

	if ((n = LeerFichero(input[1], p, cont, file)) == -1)
		perror("Imposible leer fichero");
	else
	{
		if (file)
		{
			// Comando readfile
			printf("Leidos %lld bytes de %s en %p\n", (long long)n, input[1], p);
		}
		else
		{
			// Comando read
			printf("Leidos %lld bytes del descriptor %s en %p\n", (long long)n, input[1], p);
		}
	}
}

void Cmd_Write(char *input[], bool file)
{
	void *p;
	size_t cont = -1; /*si no pasamos tamano se lee entero */
	ssize_t n;
	int i = 0;
	bool overwrite = false;

	if (input[1] == NULL)
	{
		printf("Faltan parametros\n");
		return;
	}

	// Si se escribe "-o", significa que es overwrite
	if (strcmp(input[1], "-o") == 0)
	{
		overwrite = true;
		i = 1;
	}
	if (input[i + 1] == NULL || input[i + 2] == NULL)
	{
		printf("Faltan parametros\n");
		return;
	}
	p = (void *)strtoull(input[i + 2], NULL, 16); /*convertimos de cadena a puntero*/
	if (input[i + 3] != NULL)
	{
		// bytes a contar:
		cont = (size_t)atoll(input[i + 3]);

		if (cont < 1)
		{
			printf("cont debe ser mayor que 0\n");
			return;
		}
	}
	else
	{
		printf("Faltan parametros\n");
		return;
	}
	if ((n = EscribirFichero(input[i + 1], p, cont, file, overwrite)) == -1)
		perror("Imposible escribir fichero");
	else
	{
		if (file)
		{
			// Comando writefile
			printf("Escritos %lld bytes de %p en %s\n", (long long)n, p, input[i + 1]);
		}
		else
		{
			// Comando write
			printf("Escritos %lld bytes de %p en el descriptor %s\n", (long long)n, p, input[i + 1]);
		}
	}
}

// Ejecuta la funcion recursiva n veces
void recurse(int n)
{
	char automatico[TAMANO];
	static char estatico[TAMANO];

	printf("Parametro:%3d(%p) Array %p, Arr Estatico %p\n", n, &n, automatico, estatico);

	if (n > 0)
		recurse(n - 1);
}
