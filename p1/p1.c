#include "funciones.h"

void historic(char *input, int numtrozos, tListC *lcom, bool *terminar, tCmdPos *pos, tListF *lfich, int contrec);
void procesarCadena(char *input, bool *terminar, tListC *lcom, tCmdPos *pos, tListF *lfich, int contrec);

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
    tCmdPos pos = 0;       // Contador de comandos
    char input[MAX];       // String para la entrada de comandos
    bool terminar = false; // Booleano para determinar el fin del shell
    int contrec = 0;       // Contador para evitar recursividad infinita
    // Entramos en el bucle del shell
    while (!terminar)
    {
        printf("-> ");
        fgets(input, sizeof(input), stdin);
        procesarCadena(input, &terminar, &lcom, &pos, &lfich, contrec);
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

void historic(char *input, int numtrozos, tListC *lcom, bool *terminar, tCmdPos *pos, tListF *lfich, int contrec)
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

                if (i == NULL)
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
                    procesarCadena(d.cmd, terminar, lcom, pos, lfich, contrec + 1); // Llamamos a procesar cadena con el comando a ejecutar}
                }
                else
                    printf("Demasiada recursion en hist\n");
            }
            else
                printf("No hay elemento %d en el historico\n", atoi(input));
        }
    }
}
void procesarCadena(char *input, bool *terminar, tListC *lcom, tCmdPos *pos, tListF *lfich, int contrec)
{
    char *trozos[MAX]; // String para el input troceado
    char u[MAX];       // Declaramos otro string auxiliar para insertar el comando escrito sin trocear en el historial
    strcpy(u, input);
    int numtrozos = trocearCadena(input, trozos); // Lo troceamos
    if (numtrozos != 0)                           // Caso en el que se escribe algo
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
            authors(trozos[1], numtrozos);
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
            cd(trozos[1], numtrozos);
        }
        else if (strcmp(trozos[0], "date") == 0)
        {
            date(trozos[1], numtrozos);
        }
        else if (strcmp(trozos[0], "historic") == 0)
        {
            historic(trozos[1], numtrozos, lcom, terminar, pos, lfich, contrec);
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
            help(trozos[1], numtrozos);
        }
        else if (strcmp(trozos[0], "makefile") == 0)
        {
            makefile(trozos[1], numtrozos);
        }
        else if (strcmp(trozos[0], "cwd") == 0)
        {
            cwd();
        }
        else if (strcmp(trozos[0], "makedir") == 0)
        {
            makedir(trozos[1], numtrozos);
        }
        else if (strcmp(trozos[0], "listfile") == 0)
        {
            listfile(trozos, numtrozos);
        }
        else if (strcmp(trozos[0], "listdir") == 0)
        {
            listdir(trozos);
        }
        else if (strcmp(trozos[0], "erase") == 0)
        {
            erase(trozos, numtrozos);
        }
        else if (strcmp(trozos[0], "delrec") == 0)
        {
            delrec(trozos, numtrozos);
        }
        else if (strcmp(trozos[0], "reclist") == 0)
        {
            listarRec(trozos, false);
        }
        else if (strcmp(trozos[0], "revlist") == 0)
        {
            listarRec(trozos, true);
        }
        else
            printf("Comando no valido\n");
    }
}
