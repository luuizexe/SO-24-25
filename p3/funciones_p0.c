#include "funciones_p0.h"

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
    time_t t;                 // Declaramos la variable de tipo time_t para guardar el tiempo
    struct tm *struct_t;      // Declaramos un struct de tipo tm para desglosar fecha y hora
    time(&t);                 // Obtenemos el tiempo actual
    struct_t = localtime(&t); // Insertamos los datos en el struct
    char fyh[20];             // Inicializamos el string en el que guardamos la hora y/o fecha

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

void cmd_open(char *input[], tListF *lfich)
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
            if (insertItemF(e, NULL, lfich))
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

void cmd_close(char *input[], tListF *lfich)
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
            tPosF p = findItemF(atoi(input[1]), *lfich);
            if (p != NULL)
                deleteAtPositionF(p, lfich);
        }
    }
}

void cmd_dup(char *input[], tListF *lfich)
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
            if (!insertItemF(e, NULL, lfich))
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
    uname(&sys);        // Rellenamos dicho structf
    printf("%s (%s), OS: %s-%s-%s\n", sys.nodename, sys.machine, sys.sysname, sys.release, sys.version);
}

void help(char *input, int numtrozos)
{
    if (numtrozos == 1) // Caso en el que no hay argumentos: imprimimos comandos disponibles
    {
        printf("Comandos disponibles:\nauthors\npid\nppid\ncd\ndate\nhistoric\nopen\nclose\ndup\ninfosys\nquit\nexit\nbye\n"
               "makefile\nmakedir\nlistfile\ncwd\nlistdir\nreclist\nrevlist\nerase\ndelrec\nallocate\ndeallocate\nmemfill\nmemdump\nmemory"
               "readfile\nwritefile\nread\nwrite\nrecurse\n");
    }
    else
    { // Segun el comando introducido, mostramos su descripcion
        if (strcmp(input, "authors") == 0)
            printf("authors [-l|-n]: Muestra nombre y correo de los autores\n[-l] Solo los correos\n[-n] Solo los nombres\n");
        else if (strcmp(input, "pid") == 0)
            printf("pid: Muestra el pid del proceso del shell\n");
        else if (strcmp(input, "ppid") == 0)
            printf("ppid: Muestra el pid del proceso padre del shell\n");
        else if (strcmp(input, "cd") == 0)
            printf("cd [dir]: Cambia el directorio de trabajo a [dir]\nSi no se escribe [dir], muestra el directorio actual\n");
        else if (strcmp(input, "date") == 0)
            printf("date [-d|-t]: Muestra la fecha actual en fomato DD/MM/AAAA y la hora actual en formato hh:mm:ss\n[-d] Solo la fecha\n[-t] Solo la hora\n");
        else if (strcmp(input, "historic") == 0)
            printf("historic [N|-N]: Muestra el historial ordenado de comandos ejecutadose\n[N] Repite el N comando\n[-N] Muestra los ultimos N comandos\n");
        else if (strcmp(input, "open") == 0)
            printf("open [file] [mode]: Abre el fichero y lo añade a la lista de ficheros abiertos\nSi no hay parametros, muestra la lista de archivos abiertos\n"
                   "\ncr: O_CREAT\nap: O_APPEND\nex: O_EXCL\nro: O_RDONLY\nrw: O_RDWR\nwo: O_WRONLY\ntr: O_TRUNC\n");
        else if (strcmp(input, "close") == 0)
            printf("close [df]: Cierra el descriptor del archivo df y elimina el elemento correspondiente de la lista\n"
                   "Si no hay parametros, muestra la lista de archivos abiertos\n");
        else if (strcmp(input, "dup") == 0)
            printf("dup [df]: Duplica el descriptor del archivo df\nSi no hay parametros, muestra la lista de archivos abiertos\n");
        else if (strcmp(input, "infosys") == 0)
            printf("infosys: Muetra información de la maquina ejecutando el shell\n");
        else if (strcmp(input, "help") == 0)
            printf("help [C]: Muestra una breve explicación del comando [C]\nSi no se escribe [C], muestra la lista de comandos disponibles\n");
        else if (strcmp(input, "quit") == 0)
            printf("quit: Finaliza el shell\n");
        else if (strcmp(input, "exit") == 0)
            printf("exit: Finaliza el shell\n");
        else if (strcmp(input, "bye") == 0)
            printf("bye: Finaliza el shell\n");
        else if (strcmp(input, "makefile") == 0)
            printf("makefile [name]: Crea un fichero de nombre [name]\n\tSi no hay parametros, muestra el directorio actual\n");
        else if (strcmp(input, "makedir") == 0)
            printf("makedir [name]: Crea un directorio de nombre [name]\n\tSi no hay parametros, muestra el directorio actual\n");
        else if (strcmp(input, "listfile") == 0)
            printf("listfile [-long][-link][-acc] name1 name2...: Lista ficheros:\n"
                   "\t[-long]: listado largo\n\t[-acc]: accestime\n[-link]: si es un enlace simbolico, el path contenido\n"
                   "\tSi no hay parametros, muestra el directorio actual\n");
        else if (strcmp(input, "cwd") == 0)
            printf("cwd: Muestra el directorio actul de la shell\n");
        else if (strcmp(input, "listdir") == 0)
            printf("listdir [-hid][-long][-link][-acc] n1 n2...: Lista contenidos de directorios:\n"
                   "[-hid]: incluye los ficheros ocultos\n[-long]: listado largo\n[-acc]: accestime\n[-link]: si es un enlace simbolico, el path contenido\n"
                   "Si no hay parametros, muestra el directorio actual\n");
        else if (strcmp(input, "reclist") == 0)
            printf("reclist [-hid][-long][-link][-acc] n1 n2...: Lista recursivamente contenidos de directorios (subdirs despues):\n"
                   "[-hid]: incluye los ficheros ocultos\n[-long]: listado largo\n[-acc]: accestime\n[-link]: si es un enlace simbolico, el path contenido\n"
                   "Si no hay parametros, muestra el directorio actual\n");
        else if (strcmp(input, "revlist") == 0)
            printf("revlist [-hid][-long][-link][-acc] n1 n2...: Lista recursivamente contenidos de directorios (subdirs antes):\n"
                   "[-hid]: incluye los ficheros ocultos\n[-long]: listado largo\n[-acc]: accestime\n[-link]: si es un enlace simbolico, el path contenido\n"
                   "Si no hay parametros, muestra el directorio actual\n");
        else if (strcmp(input, "erase") == 0)
            printf("erase [name 1 name2 ..]: Elimina ficheros o directorios vacios\nSi no hay parametros, muestra el directorio actual\n");
        else if (strcmp(input, "delrec") == 0)
            printf("delrec [name1 name2 ..]: Elimina ficheros o directorios no vacios recursivamente\nSi no hay parametros, muestra el directorio actual\n");
        else if (strcmp(input, "allocate") == 0)
            printf("allocate [-malloc|-shared|-createshared|-mmap]: Asigna un bloque de memoria:\n"
                   "\t[-malloc] [tam]: asigna un bloque malloc de tamano tam\n"
                   "\t[-createshared] [cl] [tam]: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n"
                   "\t[-shared] [cl]: asigna el bloque de memoria compartida (ya existente) de clave cl\n"
                   "\t[-mmap] [fich] [perm]: mapea el fichero fich con los permisos perm\n");
        else if (strcmp(input, "deallocate") == 0)
            printf("deallocate [-malloc|-shared|-delkey|-mmap|addr]: Desasigna un bloque de memoria\n"
                   "\t[-malloc] [tam]: desasigna el bloque malloc de tamano tam\n"
                   "\t[-shared] [cl]: desasigna (desmapea) el bloque de memoria compartida de clave cl\n"
                   "\t[-delkey] [cl]: elimina del sistema (sin desmapear) la clave de memoria cl\n"
                   "\t[-mmap] [fich]: desmapea el fichero mapeado fich\n"
                   "\t[addr]: desasigna el bloque de memoria en la direccion addr\n");
        else if (strcmp(input, "memfill") == 0)
            printf("memfill [addr] [cont] [byte]: Llena cont veces la memoria a partir de addr con byte\n");
        else if (strcmp(input, "memdump") == 0)
            printf("memdump [addr] [cont]: Vuelca en pantalla los contenidos (cont bytes) de la posicion de memoria addr\n");
        else if (strcmp(input, "memory") == 0)
            printf("memory [-blocks|-funcs|-vars|-all|-pmap]: Muestra detalles de la memoria del proceso\n"
                   "\t[-blocks]: los bloques de memoria asignados\n"
                   "\t[-funcs]: las direcciones de las funciones\n"
                   "\t[-vars]: las direcciones de las variables\n"
                   "\t[-all]: todo lo anterior\n"
                   "\t[-pmap]: muestra la salida del comando pmap\n");
        else if (strcmp(input, "readfile") == 0)
            printf("readfile [fiche] [addr] [cont]: Lee cont bytes desde fich a la direccion addr\n");
        else if (strcmp(input, "writefile") == 0)
            printf("writefile [-o] [fiche] [addr] [cont]: Escribe cont bytes desde la direccion addr a fich\n"
                   "\t[-o]: sobreescribe\n");
        else if (strcmp(input, "read") == 0)
            printf("read [df] [addr] [cont]: Transfiere cont bytes del fichero descrito por df a la direccion addr\n");
        else if (strcmp(input, "write") == 0)
            printf("write [df] [addr] [cont]: Transfiere cont bytes desde la direccion addr al fichero descrito por df\n");
        else if (strcmp(input, "recurse") == 0)
            printf("recurse [n]: Invoca a la funcion recursiva n veces\n");
        else if (strcmp(input, "getuid") == 0)
            printf("getuid: Muestra las credenciales del proceso que ejecuta el shell\n");
        else if (strcmp(input, "setuid") == 0)
            printf("setuid [-l] id: Cambia las credenciales del proceso que ejecuta el shell\n[-l]: establece la credencial a login id\n");
        else if (strcmp(input, "showvar") == 0)
            printf("showvar [v1|v1|...]: Muestra el valor y las direcciones de la variable de entorno var\n");
        else if (strcmp(input, "changevar") == 0)
            printf("changevar [-a|-e|-p] var valor: Cambia el valor de una variable de entorno\n"
                   "\t[-a]: accede por el tercer arg de main\n"
                   "\t[-e]: accede mediante environ\n"
                   "\t[-p]: accede mediante putenv\n");
        else if (strcmp(input, "subsvar") == 0)
            printf("subsvar [-a|-e] var1 var2 valor: Sustituye la variable de entorno var1 con var2=valor\n"
                   "\t[-a]: accede por el tercer arg de main\n"
                   "\t[-e]: accede mediante environ\n");
        else if (strcmp(input, "environ") == 0)
            printf("environ [-environ|-addr]: Muestra el entorno del proceso\n"
                   "\t[-environ]: accede usando environ (en lugar del tercer arg de main)\n"
                   "\t[-addr]: muestra el valor y donde se almacenan environ y el 3er arg main\n");
        else if (strcmp(input, "fork") == 0)
            printf("fork: El shell hace fork y queda en espera a que su hijo termine\n");
        else if (strcmp(input, "search") == 0)
            printf("search [-add|-del|-clear|-path]: Manipula o muestra la ruta de busqueda del shell (path)\n"
                   "\t[-add] dir: anhade dir a la ruta de busqueda(equiv +dir)\n"
                   "\t[-del] dir: elimina dir de la ruta de busqueda (equiv -dir)\n"
                   "\t[-clear]: vacia la ruta de busqueda\n"
                   "\t[-path]: importa el PATH en la ruta de busqueda\n");
        else if (strcmp(input, "exec") == 0)
            printf("exec VAR1 VAR2 ... prog args...: Ejecuta, sin crear proceso,prog"
                   " con argumentos args en un entorno que contiene solo las variables VAR1, VAR2...\n");
        else if (strcmp(input, "execpri") == 0)
            printf("execpri [prio] VAR1 VAR2 ... prog args...: Ejecuta con prioridad prio y sin crear proceso,prog"
                   " con argumentos args en un entorno que contiene solo las variables VAR1, VAR2...\n");
        else if (strcmp(input, "fg") == 0)
            printf("fg VAR1 VAR2 ... prog args...: Crea un proceso que ejecuta en primer plano prog"
                   " con argumentos args en un entorno que contiene solo las variables VAR1, VAR2...\n");
        else if (strcmp(input, "fgpri") == 0)
            printf("fgpri [prio] VAR1 VAR2 ... prog args...: Crea un proceso con prioridad prio que ejecuta en"
                   "primer plano prog con argumentos args en un entorno que contiene solo las variables VAR1, VAR2...\n");
        else if (strcmp(input, "back") == 0)
            printf("back VAR1 VAR2 ... prog args...: Crea un proceso que ejecuta en segundo plano prog"
                   " con argumentos args en un entorno que contiene solo las variables VAR1, VAR2...\n");
        else if (strcmp(input, "backpri") == 0)
            printf("backpri [prio] VAR1 VAR2 ... prog args...: Crea un proceso con prioridad prio que ejecuta en"
                   " segundo plano prog con argumentos args en un entorno que contiene solo las variables VAR1, VAR2...\n");
        else if (strcmp(input, "listjobs") == 0)
            printf("listjobs: Lista los procesos en segundo plano\n");
        else if (strcmp(input, "deljobs") == 0)
            printf("deljobs [-term|-sig]: Elimina los procesos de la lista procesos en segundo plano\n"
                   "\t[-term]: los terminados\n"
                   "\t[-sig]: los terminados por senal\n");
        else
            printf("Argumento no valido\n");
    }
}
