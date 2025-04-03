#include "funciones_p0.h"
#include "funciones_p1.h"
#include "funciones_p2.h"
#include "funciones_p3.h"

void historic(char *input, int numtrozos, tListC *lcom, bool *terminar, tCmdPos *pos, tListF *lfich, tListM *lmem, tListP *lproc, tListD *ldir, int contrec, char **environment);
void procesarCadena(char *input, bool *terminar, tListC *lcom, tCmdPos *pos, tListF *lfich, tListM *lmem, tListP *lproc, tListD *ldir, int contrec, char **environment);

int main(int argc, char *argv[], char **environment)
{
    // Creamos la lista de comandos
    tListC lcom;
    createEmptyListC(&lcom);
    // De ficheros abiertos
    tListF lfich;
    createEmptyListF(&lfich);
    // De bloques de memoria
    tListM lmem;
    createEmptyListM(&lmem);
    // De procesos ejecutados
    tListP lproc;
    createEmptyListP(&lproc);
    // De directorios de busqueda
    tListD ldir;
    createEmptyListD(&ldir);

    // Insertamos los archivos abiertos heredados a la lista de ficheros abiertos
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
    tCmdPos pos = 0;       // Contador de comandos
    char input[MAX];       // String para la entrada de comandos
    bool terminar = false; // Booleano para determinar el fin del shell
    int contrec = 0;       // Contador para evitar recursividad infinita

    printf("Iniciando la shell:\n");

    // Entramos en el bucle del shell
    while (!terminar)
    {
        printf("-> ");
        fgets(input, sizeof(input), stdin);
        procesarCadena(input, &terminar, &lcom, &pos, &lfich, &lmem, &lproc, &ldir, contrec, environment);
    }

    // Una vez terminado el bucle, vaciamos las listas
    vaciarListas(&lcom, &lfich, &lmem, &lproc, &ldir);
    return 0;
}

void historic(char *input, int numtrozos, tListC *lcom, bool *terminar, tCmdPos *pos, tListF *lfich, tListM *lmem, tListP *lproc, tListD *ldir, int contrec, char **environment)
{
    if (numtrozos == 1) // Caso en el que no hay argumentos
    {
        // Imprimimos el historico de comandos
        for (tPosC i = firstC(*lcom); i != LNULL; i = nextC(i))
        {
            tItemC d = getItemC(i);
            printf("%d -> %s\n", d.pos, d.cmd);
        }
    }
    else
    {
        if (input[0] == '-' && isdigit(input[1])) // Caso [-N]: imprimimos los N ultimos comandos del historial
        {
            tCmdPos n = atoi(&input[1]); // convertimos el input a int
            if (n > 0)
            {
                tPosC i = lastC(*lcom);
                // Retrocedemos N veces desde el final
                for (int k = 1; k <= n && i != LNULL; k++)
                {
                    i = previousC(i, *lcom);
                }
                if (i == NULL) // Caso en el que se pasa de la lista: mostramos todos
                {
                    i = firstC(*lcom);
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
                if (contrec < 10) // Comprobamos que no haya recursion
                {
                    printf("Ejecutando: %d -> %s\n", d.pos, d.cmd);
                    procesarCadena(d.cmd, terminar, lcom, pos, lfich, lmem, lproc, ldir, contrec + 1, environment); // Llamamos a procesar cadena con el comando a ejecutar}
                }
                else
                    printf("Demasiada recursion en hist\n");
            }
            else
                printf("No hay elemento %d en el historico\n", atoi(input));
        }
    }
}
void procesarCadena(char *input, bool *terminar, tListC *lcom, tCmdPos *pos, tListF *lfich, tListM *lmem, tListP *lproc, tListD *ldir, int contrec, char **environment)
{
    char *trozos[MAX]; // String para el input troceado
    char u[MAX];       // Declaramos otro string auxiliar para insertar el comando escrito sin trocear en el historial
    strcpy(u, input);
    int numTrozos = trocearCadena(input, trozos); // Lo troceamos
    if (numTrozos != 0)                           // Caso en el que se escribe algo
    {
        if (contrec == 0) // Si es la primera vez que lee esa instruccion, la insertamos en la lista de comandos
        {
            int l = strlen(u) - 1; // Obtenemos el tamaño del string auxiliar
            u[l] = '\0';           // Insertamos '\0' para reemplazar el \n
            tItemC d;              // Rellenamos el item de la lista de comandos
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
            historic(trozos[1], numTrozos, lcom, terminar, pos, lfich, lmem, lproc, ldir, contrec, environment);
        }
        else if (strcmp(trozos[0], "open") == 0)
        {
            cmd_open(trozos, lfich);
        }
        else if (strcmp(trozos[0], "close") == 0)
        {
            cmd_close(trozos, lfich);
        }
        else if (strcmp(trozos[0], "dup") == 0)
        {
            cmd_dup(trozos, lfich);
        }
        else if (strcmp(trozos[0], "infosys") == 0)
        {
            infosys();
        }
        else if (strcmp(trozos[0], "help") == 0)
        {
            help(trozos[1], numTrozos);
        }
        else if (strcmp(trozos[0], "makefile") == 0)
        {
            makefile(trozos[1], numTrozos);
        }
        else if (strcmp(trozos[0], "cwd") == 0)
        {
            cwd();
        }
        else if (strcmp(trozos[0], "makedir") == 0)
        {
            makedir(trozos[1], numTrozos);
        }
        else if (strcmp(trozos[0], "listfile") == 0)
        {
            listfile(trozos, numTrozos);
        }
        else if (strcmp(trozos[0], "listdir") == 0)
        {
            listdir(trozos);
        }
        else if (strcmp(trozos[0], "erase") == 0)
        {
            erase(trozos, numTrozos);
        }
        else if (strcmp(trozos[0], "delrec") == 0)
        {
            delrec(trozos, numTrozos);
        }
        else if (strcmp(trozos[0], "reclist") == 0)
        {
            listarRec(trozos, false);
        }
        else if (strcmp(trozos[0], "revlist") == 0)
        {
            listarRec(trozos, true);
        }
        else if (strcmp(trozos[0], "allocate") == 0)
        {
            allocate(trozos, lmem, lfich);
        }
        else if (strcmp(trozos[0], "deallocate") == 0)
        {
            deallocate(trozos, lmem, lfich);
        }
        else if (strcmp(trozos[0], "memfill") == 0)
        {
            memfill(trozos);
        }
        else if (strcmp(trozos[0], "memdump") == 0)
        {
            memdump(trozos);
        }
        else if (strcmp(trozos[0], "memory") == 0)
        {
            memory(trozos, *lmem);
        }
        else if (strcmp(trozos[0], "readfile") == 0)
        {
            cmd_Read(trozos, true);
        }
        else if (strcmp(trozos[0], "writefile") == 0)
        {
            cmd_Write(trozos, true);
        }
        else if (strcmp(trozos[0], "read") == 0)
        {
            cmd_Read(trozos, false);
        }
        else if (strcmp(trozos[0], "write") == 0)
        {
            cmd_Write(trozos, false);
        }
        else if (strcmp(trozos[0], "recurse") == 0)
        {
            if (trozos[1] != NULL)
            {
                int n = atoi(trozos[1]);
                if (n > TAMANO)
                {
                    n = TAMANO;
                }
                recurse(n);
            }
        }
        else if (strcmp(trozos[0], "getuid") == 0)
        {
            cmd_getuid();
        }
        else if (strcmp(trozos[0], "setuid") == 0)
        {
            cmd_setuid(trozos);
        }
        else if (strcmp(trozos[0], "showvar") == 0)
        {
            showvar(trozos, environment);
        }
        else if (strcmp(trozos[0], "changevar") == 0)
        {
            changevar(trozos, environment);
        }
        else if (strcmp(trozos[0], "subsvar") == 0)
        {
            subsvar(trozos, environment);
        }
        else if (strcmp(trozos[0], "environ") == 0)
        {
            environ(trozos, environment);
        }
        else if (strcmp(trozos[0], "fork") == 0)
        {
            cmd_fork(trozos, lproc);
        }
        else if (strcmp(trozos[0], "search") == 0)
        {
            search(trozos, ldir);
        }
        else if (strcmp(trozos[0], "exec") == 0)
        {
            exec(false, trozos, ldir, numTrozos);
        }
        else if (strcmp(trozos[0], "execpri") == 0)
        {
            exec(true, trozos, ldir, numTrozos);
        }
        else if (strcmp(trozos[0], "fg") == 0)
        {
            fg(false, false, trozos, ldir, numTrozos, lcom, lfich, lmem, lproc);
        }
        else if (strcmp(trozos[0], "fgpri") == 0)
        {
            fg(true, false, trozos, ldir, numTrozos, lcom, lfich, lmem, lproc);
        }
        else if (strcmp(trozos[0], "back") == 0)
        {
            back(false, trozos, ldir, numTrozos, lcom, lfich, lmem, lproc);
        }
        else if (strcmp(trozos[0], "backpri") == 0)
        {
            back(true, trozos, ldir, numTrozos, lcom, lfich, lmem, lproc);
        }
        else if (strcmp(trozos[0], "listjobs") == 0)
        {
            listjobs(lproc);
        }
        else if (strcmp(trozos[0], "deljobs") == 0)
        {
            deljobs(trozos, lproc);
        }
        else
        {
            char *nuevoTrozos[MAX];
            nuevoTrozos[0] = "fg";
            for (int i = 0; i < numTrozos; i++)
            {
                nuevoTrozos[i + 1] = trozos[i];
            }
            nuevoTrozos[numTrozos + 1] = NULL;

            fg(false, false, nuevoTrozos, ldir, numTrozos + 1, lcom, lfich, lmem, lproc);
        }
    }
}
