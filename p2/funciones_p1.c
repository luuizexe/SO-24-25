#include "funciones_p1.h"

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

            const char *nombre_usuario = (usuario != NULL) ? usuario->pw_name : "unknown";
            const char *nombre_grupo = (grupo != NULL) ? grupo->gr_name : "unknown";

            // Creamos un struct para introducir los datos del tiempo de modificacion
            struct tm *struct_t;
            struct_t = localtime(&obj.st_mtime);
            char mTime[20];
            strftime(mTime, 80, "%Y/%m/%d-%H:%M", struct_t);
            printf("%s%7ld (%7ld) %-10s%-10s %-10s",
                   mTime, obj.st_nlink, obj.st_ino, nombre_usuario, nombre_grupo,
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
                            if ((hayHid && strcmp(entradaDir->d_name, ".") != 0 && strcmp(entradaDir->d_name, "..") != 0) || (!hayHid && entradaDir->d_name[0] != '.'))
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
