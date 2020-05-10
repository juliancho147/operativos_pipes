#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "structsSen.h"
#include <unistd.h>
#include <signal.h>
int es = 1;

typedef void (*sighandler_t)(int);
sighandler_t signalHandler(void)
{
}
int main(int argc, char **argv)
{

    char *PipeDirectorio;
    char *PipeSensores;
    char *nombreMonitor;
    char *idc;
    int id;
    int i;
    signal(SIGUSR1, (sighandler_t)signalHandler);
    if (argc != 9)
    {
        printf("Error, numero de valores incompletos");
        return -1;
    }

    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {

            PipeDirectorio = argv[i + 1];
            printf("%s ", PipeDirectorio);
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            PipeSensores = argv[i + 1];
            printf("%s ", PipeSensores);
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            nombreMonitor = argv[i + 1];
            printf("%s ", nombreMonitor);
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            idc = argv[i + 1];
            id = atoi(idc);
            printf("%d ", id);
        }
    }
    /**pipe para mandar la informacion al directorio**/
    int fd;
    struct monitor mon;
    strcpy(mon.nombre, nombreMonitor);
    mon.proceso = getpid();
    mon.id = id;
    strcpy(mon.pipe, PipeSensores);
    do
    {
        fd = open(PipeDirectorio, O_WRONLY);
        if (fd == -1)
        {
            printf("Esperando que el pipe sea creado \n");
        }
    } while (fd == -1);
    write(fd, &mon, sizeof(struct monitor));

    close(fd);
    /*pipe para imprimir el valor de los sensores*/
    if( id != 0)
    while (es)
    {
        fd = 
        do
        {
            /* code */
        } while (/* condition */);
        
    }
    
}