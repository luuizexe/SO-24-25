#include "funciones_p3.h"

void cmd_getuid()
{
    // Credenciales reales
    uid_t real = getuid();
    struct passwd *pwdReal = getpwuid(real);
    printf("Credencial real: %d, (%s)\n", real, pwdReal != NULL ? pwdReal->pw_name : "unknown");

    // Credenciales efectivas
    uid_t efectiva = geteuid();
    struct passwd *pwdEfectivo = getpwuid(efectiva);
    printf("Credencial efectiva: %d, (%s)\n", efectiva, pwdEfectivo != NULL ? pwdEfectivo->pw_name : "unknown");
}

void cmd_setuid(char *input[])
{
    if (input[1] == NULL)
    {
        // Si input[1] es nulo, llamamos a nuestro cmd_getuid para mostrar informacion del usuario
        cmd_getuid();
    }
    else
    {
        uid_t uid;
        // Si se cunple el if, establecemos credencial por login
        if ((strcmp("-l", input[1]) == 0))
        {
            if (input[2] == NULL)
            {
                cmd_getuid();
                return;
            }
            struct passwd *pwd = getpwnam(input[2]);
            if (pwd == NULL)
            {
                printf("Usuario no existente %d\n", atoi(input[2]));
                return;
            }
            uid = pwd->pw_uid;
        }
        // Si no, sera por la id numerica
        else
        {
            uid = atoi(input[1]);
        }
        // Set del uid efectivo
        if (seteuid(uid) == -1)
        {
            perror("Imposible cambiar credencial");
        }
    }
}

int buscarVariable(char *var, char *e[]) /*busca una variable en el entorno que se le pasa como parÃ¡metro*/
{                                        /*devuelve la posicion de la variable en el entorno, -1 si no existe*/
    int pos = 0;
    char aux[MAX];
    strcpy(aux, var);
    strcat(aux, "=");
    while (e[pos] != NULL)
        if (!strncmp(e[pos], aux, strlen(aux)))
            return (pos);
        else
            pos++;
    errno = ENOENT; /*no hay tal variable*/
    return (-1);
}

void showvar(char *input[], char **environment)
{
    int i;
    if (input[1] == NULL)
    {
        // Si es nulo input[1], recorremos y mostramos environment
        for (i = 0; environment[i] != NULL; i++)
            printf("%p->main arg3[%d]=(%p) %s\n", &environment[i], i, environment[i], environment[i]);
    }
    else
    {
        // Si no, buscamos variables en environment y __environ para mostrar
        for (i = 1; input[i] != NULL; i++)
        {
            int n, m;
            if ((n = buscarVariable(input[i], environment)) != -1)
            {
                printf("Con arg3 main %s(%p) @(%p)\n", environment[n], environment[n], &environment[n]);
            }
            if ((m = buscarVariable(input[i], __environ)) != -1)
            {
                printf("  Con environ %s(%p) @(%p)\n", __environ[m], __environ[m], &__environ[m]);
                printf("    Con getenv %s(%p)\n", getenv(input[i]), getenv(input[i]));
            }
        }
    }
}

int cambiarVariable(char *var, char *var2, char *valor, char *e[], bool esChange) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
{                                                                                 /*lo hace directamente, no usa putenv*/
    int pos;
    char *aux;

    if ((pos = buscarVariable(var, e)) == -1)
        return (-1);

    if (esChange)
    {
        if ((aux = (char *)malloc(strlen(var) + strlen(valor) + 2)) == NULL)
            return -1;
        strcpy(aux, var);
    }
    else
    {
        if ((aux = (char *)malloc(strlen(var2) + strlen(valor) + 2)) == NULL)
            return -1;
        strcpy(aux, var2);
    }

    strcat(aux, "=");
    strcat(aux, valor);
    e[pos] = aux;
    return (pos);
}

void changevar(char *input[], char **environment)
{
    // changevar debe tener los mismos argumentos
    if (input[1] != NULL && input[2] != NULL && input[3] != NULL)
    {
        // Cambio de variable utilizando environment
        if (strcmp(input[1], "-a") == 0)
        {
            if (cambiarVariable(input[2], NULL, input[3], environment, true) == -1)
            {
                perror("Imposible cambiar variable");
            }
        }
        // Cambio de variable utilizando __environ
        else if (strcmp(input[1], "-e") == 0)
        {
            if (cambiarVariable(input[2], NULL, input[3], __environ, true) == -1)
            {
                perror("Imposible cambiar variable");
            }
        }
        // Cambio de variable utilizando putenv
        else if (strcmp(input[1], "-p") == 0)
        {
            char *aux;
            if ((aux = (char *)malloc(strlen(input[2]) + strlen(input[3]) + 2)) == NULL)
            {
                perror("Imposible crear variable");
                return;
            }
            strcpy(aux, input[2]);
            strcat(aux, "=");
            strcat(aux, input[3]);
            if (putenv(aux) != 0)
            {
                perror("Imposible crear variable");
                free(aux);
            }
        }
        else
        {
            // Mostramos como debe ser el comando
            printf("Uso: changevar [-a|-e|-p] var valor\n");
        }
    }
    else
    {
        // Mostramos como debe ser el comando
        printf("Uso: changevar [-a|-e|-p] var valor\n");
    }
}

void subsvar(char *input[], char **environment)
{
    // subsvar debe pasar siempre los mismos argumentos, ninguno puede ser nulo
    if (input[1] != NULL && input[2] != NULL && input[3] != NULL && input[4] != NULL)
    {
        // Cambio de variable utilizando environment
        if (strcmp(input[1], "-a") == 0)
        {
            if (cambiarVariable(input[2], input[3], input[4], environment, false) == -1)
            {
                fprintf(stderr, "Imposible sustituir variable %s por %s\n", input[2], input[3]);
            }
        }
        // Cambio de variable utilizando __environ
        else if (strcmp(input[1], "-e") == 0)
        {
            if (cambiarVariable(input[2], input[3], input[4], __environ, false) == -1)
            {
                fprintf(stderr, "Imposible sustituir variable %s por %s\n", input[2], input[3]);
            }
        }
        else
        {
            // Mostramos como debe ser el comando
            printf("Uso: subsvar [-a|-e] var1 var2 valor\n");
        }
    }
    else
    {
        // Mostramos como debe ser el comando
        printf("Uso: subsvar [-a|-e] var1 var2 valor\n");
    }
}

void environ(char *input[], char **environment)
{
    int i;
    if (input[1] == NULL)
    {
        // Recorremos y mostramos environment
        for (i = 0; environment[i] != NULL; i++)
            printf("%p->main arg3[%d]=(%p) %s\n", &environment[i], i, environment[i], environment[i]);
    }
    else if (strcmp(input[1], "-environ") == 0)
    {
        // Recorremos y mostramos __environ
        for (i = 0; __environ[i] != NULL; i++)
            printf("%p->environ[%d]=(%p) %s\n", &__environ[i], i, __environ[i], __environ[i]);
    }
    else if (strcmp(input[1], "-addr") == 0)
    {
        // Mostramos las direcciones asociadas a __environ y environment
        printf("environ: %p (almacenado en %p)\n"
               "main arg3 : %p (almacenado en %p)\n",
               __environ, &__environ, environment, &environment);
    }
    else
    {
        // Mostramos como usar el comando si lo llaman mal
        printf("Uso: environ [-environ|-addr]");
    }
}

void cmd_fork(char *input[], tListP *lproc)
{
    pid_t pid;
    // Realiza el fork
    if ((pid = fork()) == 0)
    {
        // Limpia la lista del proceso hijo
        while (!isEmptyListP(*lproc))
        {
            deleteAtPositionP(firstP(*lproc), lproc);
        }
        printf("Ejecutando proceso %d\n", getpid());
    }

    // Si el fork tiene exito, se debe esperar a que termine el proceso hijo
    else if (pid != -1)
        waitpid(pid, NULL, 0);
}

void search(char *input[], tListD *ldir)
{
    if (input[1] != NULL)
    {
        // Introduce dir en la lista
        if (strcmp(input[1], "-add") == 0)
        {
            if (input[2] == NULL)
            {
                errno = EFAULT;
                perror("Imposible realizar operacion -add");
            }
            else
            {
                insertItemD(input[2], NULL, ldir);
            }
        }
        // Elimina dir en la lista
        else if (strcmp(input[1], "-del") == 0)
        {
            tPosD p = findItemD(input[2], *ldir);
            if (p != NULL)
            {
                deleteAtPositionD(p, ldir);
            }
        }
        // Vacia la lista por completo
        else if (strcmp(input[1], "-clear") == 0)
        {
            while (!isEmptyListD(*ldir))
            {
                deleteAtPositionD(firstD(*ldir), ldir);
            }
        }
        // Importa el PATH a la lista
        else if (strcmp(input[1], "-path") == 0)
        {
            char *path = getenv("PATH");
            if (path != NULL)
            {
                // Importante para no cambiar el path original
                char *path_aux = strdup(path);
                if (path_aux == NULL)
                {
                    perror("Error al duplicar PATH");
                    return;
                }
                char *dir = strtok(path_aux, ":");
                int cont = 0;
                while (dir != NULL)
                {
                    insertItemD(dir, NULL, ldir);
                    cont++;
                    dir = strtok(NULL, ":");
                }
                printf("Importados %d directorios en la ruta de busqueda\n", cont);
                free(path_aux);
            }
            else
            {
                perror("No existe PATH");
            }
        }
    }
    else
    {
        // Listamos elementos
        if (!isEmptyListD(*ldir))
            for (tPosD p = firstD(*ldir); p != NULL; p = nextD(p))
            {
                printf("%s\n", p->dir);
            }
    }
}

char *ejecutable(char *s, tListD ldir)
{
    static char path[MAX];
    struct stat st;
    tDir p;
    tPosD q;

    if (isEmptyListD(ldir))
        return s;
    else
        p = getItemD((firstD(ldir)));

    if (s == NULL)
        return s;
    if (s[0] == '/' || !strncmp(s, "./", 2) || !strncmp(s, "../", 3))
        return s; /*s es un path absoluto*/

    strncpy(path, p, MAX - 1);
    strncat(path, "/", MAX - 1);
    strncat(path, s, MAX - 1);
    if (lstat(path, &st) != -1)
        return path;
    for (q = nextD(firstD(ldir)); q != NULL; q = nextD(q))
    {
        p = getItemD(q);
        strncpy(path, p, MAX - 1);
        strncat(path, "/", MAX - 1);
        strncat(path, s, MAX - 1);
        if (lstat(path, &st) != -1)
            return path;
    }
    return s;
}

int execpve(char *tr[], char **NewEnv, int *pprio, tListD ldir)
{
    char *p; /*NewEnv contiene la dir del nuevo environment*/
             /*pprio dir de la nueva prioridad*/
             /*NULL indica que no hay cambio en la prioridad/environment*/
    if (tr[0] == NULL || (p = ejecutable(tr[0], ldir)) == NULL)
    {
        errno = EFAULT;
        return -1;
    }
    if (pprio != NULL && setpriority(PRIO_PROCESS, getpid(), *pprio) == -1 && errno)
    {
        printf("Imposible cambiar prioridad: %s\n", strerror(errno));
        return -1;
    }
    if (NewEnv == NULL)
        return execv(p, tr);
    else
        return execve(p, tr, NewEnv);
}

void exec(bool hayPri, char *input[], tListD *ldir, int numTrozos)
{
    char *newInput[numTrozos], *vars[numTrozos];
    bool isVar = true;
    int lastVar = 0, pos = 0, inicio = 1, v, pri;

    // Si se ha pasado la prioridad por parametro la pasamos a entero
    if (hayPri)
    {
        ++inicio;
        if (input[1] != NULL)
        {
            pri = atoi(input[1]);
        }
    }
    for (int i = inicio; input[i] != NULL; i++)
    {
        // Suponemos que son variables hasta que haya una que no
        if (isVar && (v = buscarVariable(input[i], __environ)) != -1)
        {
            vars[lastVar] = __environ[v];
            lastVar++;
        }
        else
        {
            // Aqui se guarda el ejecutable con sus argumentos
            newInput[pos] = input[i];
            pos++;
            isVar = false;
        }
    }
    // Es importante poner a NULL la ultima posicion de estos dos strings
    newInput[pos] = NULL;
    vars[lastVar] = NULL;

    // Si el usuario ha pasado environ, se ponen sus vars, si no, se mantiene el actual
    if (execpve(newInput, vars[0] != NULL ? vars : NULL, hayPri ? &pri : NULL, *ldir) == -1)
    {
        perror("Imposible ejecutar");
    }
}

void fg(bool hayPri, bool esDefault, char *input[], tListD *ldir, int numTrozos, tListC *lcom, tListF *lfich, tListM *lmem, tListP *lproc)
{
    char *newInput[numTrozos], *vars[numTrozos];
    bool isVar = true;
    int lastVar = 0, pos = 0, inicio, v, pri;
    // En caso de que procesarCadena no cumpla ningun if, entra aqui por defecto
    if (esDefault)
    {
        inicio = 0;
    }
    // Una llamada normal a fg o fgpri entra aqui
    else
    {
        inicio = 1;
    }

    // Si se ha pasado la prioridad por parametro la pasamos a entero
    if (hayPri)
    {
        ++inicio;
        if (input[1] != NULL)
        {
            pri = atoi(input[1]);
        }
    }

    for (int i = inicio; input[i] != NULL; i++)
    {
        // Suponemos que son variables hasta que hay una que no
        if (isVar && (v = buscarVariable(input[i], __environ)) != -1)
        {
            vars[lastVar] = __environ[v];
            lastVar++;
        }
        else
        {
            // Aqui se guarda el ejecutable con sus argumentos
            newInput[pos] = input[i];
            pos++;
            isVar = false;
        }
    }
    // Es importante poner a NULL la ultima posicion de estos dos strings
    newInput[pos] = NULL;
    vars[lastVar] = NULL;

    pid_t pidh;
    if ((pidh = fork()) == 0)
    {
        // Si el usuario ha pasado environ, se ponen sus vars, si no, se mantiene el actual
        if (execpve(newInput, vars[0] != NULL ? vars : NULL, hayPri ? &pri : NULL, *ldir) == -1)
        {
            // En caso de que falle, vaciamos las listas para evitar leaks
            perror("Imposible ejecutar");
            vaciarListas(lcom, lfich, lmem, lproc, ldir);
            exit(255);
        }
    }
    else if (pidh != -1)
        waitpid(pidh, NULL, 0);
}

void back(bool hayPri, char *input[], tListD *ldir, int numTrozos, tListC *lcom, tListF *lfich, tListM *lmem, tListP *lproc)
{
    char *newInput[numTrozos], *vars[numTrozos];
    bool isVar = true;
    int lastVar = 0, pos = 0, inicio = 1, v, pri;

    // Si se ha pasado la prioridad por parametro la pasamos a entero
    if (hayPri)
    {
        ++inicio;
        if (input[1] != NULL)
        {
            pri = atoi(input[1]);
        }
    }

    for (int i = inicio; input[i] != NULL; i++)
    {
        // Suponemos que son variables hasta que hay una que no
        if (isVar && (v = buscarVariable(input[i], __environ)) != -1)
        {
            vars[lastVar] = __environ[v];
            lastVar++;
        }
        else
        {
            // Aqui se guarda el ejecutable con sus argumentos
            newInput[pos] = input[i];
            pos++;
            isVar = false;
        }
    }
    // Es importante poner a NULL la ultima posicion de estos dos strings
    newInput[pos] = NULL;
    vars[lastVar] = NULL;

    pid_t pidh;
    if ((pidh = fork()) == 0)
    {
        // Si el usuario ha pasado environ, se ponen sus vars, si no, se mantiene el actual
        if (execpve(newInput, vars[0] != NULL ? vars : NULL, hayPri ? &pri : NULL, *ldir) == -1)
        {
            // En caso de que falle, vaciamos las listas para evitar leaks
            perror("Imposible ejecutar");
            vaciarListas(lcom, lfich, lmem, lproc, ldir);
            exit(255);
        }
    }
    else if (pidh != -1)
    {
        // Insertamos el proceso en la lista
        tItemP d;
        d.pid = pidh;
        d.priority = hayPri ? pri : 0;
        d.status = ACTIVE;
        d.signal = 0;
        d.cmd = 0;
        time_t t = time(NULL);
        d.tlaunch = malloc(sizeof(struct tm));
        if (d.tlaunch != NULL)
        {
            *d.tlaunch = *localtime(&t);
        }
        else
        {
            perror("Imposible asignar memoria para la hora del proceso");
            return;
        }

        char cmd[MAX];
        cmd[0] = '\0';
        for (int i = 0; newInput[i] != NULL; i++)
        {
            strcat(cmd, newInput[i]);
            if (newInput[i + 1] != NULL)
            {
                strcat(cmd, " ");
            }
        }
        d.cmd = strdup(cmd);
        insertItemP(d, NULL, lproc);
    }
}

// Funcion que devuelve el status segun wstatus
tStatus getStatus(int wstatus, int *signal)
{
    // Terminado
    if (WIFEXITED(wstatus))
    {
        *signal = WEXITSTATUS(wstatus);
        return FINISHED;
    }
    // Terminado por senal
    else if (WIFSIGNALED(wstatus))
    {
        *signal = WTERMSIG(wstatus);
        return SIGNALED;
    }
    // Parado
    else if (WIFSTOPPED(wstatus))
    {
        *signal = WSTOPSIG(wstatus);
        return STOPPED;
    }
    // Activo
    else if (WIFCONTINUED(wstatus))
    {
        *signal = 0;
        return ACTIVE;
    }
    // Para evitar errores, si no coincide con ninguno establecemos desconocido
    else
    {
        return UNKNOWN;
    }
}

// Funcion para recorrer la lista de procesos
void listjobs(tListP *lproc)
{
    if (!isEmptyListP(*lproc))
    {
        for (tPosP p = firstP(*lproc); p != NULL; p = nextP(p))
        {
            pid_t pid;
            int wstatus, signal;
            // Debemos revisar si ha cambiado el estado del proceso para actualizarlo antes de la salida
            if ((pid = waitpid(p->data.pid, &wstatus, WNOHANG | WUNTRACED | WCONTINUED)) == 0)
                ;
            else if (p->data.pid == pid)
            {
                p->data.status = getStatus(wstatus, &signal);
                p->data.signal = signal;
                p->data.priority = getpriority(PRIO_PROCESS, p->data.pid);
            }
            // Obtenemos nombre del usuario real
            uid_t real = getuid();
            struct passwd *pwdReal = getpwuid(real);
            // Formateamos la fecha
            char fyh[20];
            strftime(fyh, sizeof(fyh), "%Y/%m/%d %X", p->data.tlaunch);
            // Convertimos el enum tStatus a string para el print
            char status[10];
            switch (p->data.status)
            {
            case FINISHED:
                strcpy(status, "FINISHED");
                break;
            case STOPPED:
                strcpy(status, "STOPPED");
                break;
            case SIGNALED:
                strcpy(status, "SIGNALED");
                break;
            case ACTIVE:
                strcpy(status, "ACTIVE");
                break;
            case UNKNOWN:
                strcpy(status, "UNKNOWN");
                break;
            }
            printf("  %d   %s p=%d %s %s (%03d) %s\n", p->data.pid,
                   pwdReal != NULL ? pwdReal->pw_name : "unknown",
                   p->data.priority, fyh, status, p->data.signal, p->data.cmd);
        }
    }
}

void deljobs(char *input[], tListP *lproc)
{
    if (input[1] != NULL)
    {
        if (strcmp(input[1], "-term") == 0)
        {
            if (!isEmptyListP(*lproc))
            {
                // Borramos los procesos FINISHED
                tPosP p = firstP(*lproc);
                while (p != NULL)
                {
                    tPosP r = p;
                    p = nextP(p);
                    if (r->data.status == FINISHED)
                        deleteAtPositionP(r, lproc);
                }
            }
        }
        else if (strcmp(input[1], "-sig") == 0)
        {
            if (!isEmptyListP(*lproc))
            {
                // Borramos los procesos SIGNALED
                tPosP p = firstP(*lproc);
                while (p != NULL)
                {
                    tPosP r = p;
                    p = nextP(p);
                    if (r->data.status == SIGNALED)
                        deleteAtPositionP(r, lproc);
                }
            }
        }
        else
        {
            // si input[1] no es ni "-term" ni "-sig" listamos procesos
            listjobs(lproc);
        }
    }
    else
    {
        // Si input[1] == NULL listamos procesos
        listjobs(lproc);
    }
}

// Funcion para limpiar todas las listas
void vaciarListas(tListC *lcom, tListF *lfich, tListM *lmem, tListP *lproc, tListD *ldir)
{
    while (!isEmptyListC(*lcom))
    {
        deleteAtPositionC(firstC(*lcom), lcom);
    }
    while (!isEmptyListM(*lmem))
    {
        char addressStr[32];
        snprintf(addressStr, sizeof(addressStr), "%p", firstM(*lmem)->data.address);
        char *newInput[] = {"deallocate", addressStr, NULL};
        deallocate(newInput, lmem, lfich);
    }
    while (!isEmptyListF(*lfich))
    {
        deleteAtPositionF(firstF(*lfich), lfich);
    }
    while (!isEmptyListD(*ldir))
    {
        deleteAtPositionD(firstD(*ldir), ldir);
    }
    while (!isEmptyListP(*lproc))
    {
        deleteAtPositionP(firstP(*lproc), lproc);
    }
}
