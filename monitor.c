/*Desarrollado por Julian Builes y Santiago Bremudez
Sistemas operativos 1-2020 Pryecto 2*/
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
/***********variables************/
int es = 1;
char *PipeDirectorio;
char PipeSensores[30];
char *nombreMonitor;
char *idc;
int id;
int i;
mode_t fifo_mode = S_IRUSR | S_IWUSR;
int bytes = 0;
struct sensor s;
int fd;
struct monitor mon;

/**********************/
void escribir()
{
    int fdc = open(PipeSensores, O_RDONLY);
    read(fdc, &s, sizeof(s));
    close(fdc);
    printf("%s-%d.\n", s.nombre, s.valor);
}

typedef void (*sighandler_t)(int);
sighandler_t signalHandler(void)
{
    es = 0;
    printf("señal\n");
}
sighandler_t signalHandler2(void)
{
    printf("señal de inicio\n");
}

int main(int argc, char **argv)
{

    signal(SIGUSR1, (sighandler_t)signalHandler);
    signal(SIGUSR2, (sighandler_t)signalHandler2);

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
        }
        else if (strcmp(argv[i], "-p") == 0)
        {

            strcpy(PipeSensores, argv[i + 1]);
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            nombreMonitor = argv[i + 1];
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            idc = argv[i + 1];
            id = atoi(idc);
        }
    }
    /**pipe para mandar la informacion al directorio**/
    unlink(PipeSensores);
    mkfifo(PipeSensores, fifo_mode);

    strcpy(mon.nombre, nombreMonitor);
    mon.proceso = getpid();
    mon.id = id;
    strcpy(mon.pipe, PipeSensores);
    /*************se guarda el pid del directorio***************/
    int pid = 0;
    int fl = open("pidMonitor.txt", O_RDONLY);
    if (fl == -1)
    {
        printf("Error no se encuentra el archivo\n");
    }
    int b = read(fl, &pid, sizeof(pid));
    printf("los bytes son %d\n", b);
    printf("el pid directorio es %d\n", pid);
    close(fl);
    /************se manda una señal para el registro****************/
    if (kill(pid, SIGUSR1) == -1)
    {
        perror("NO SE PUDO HACER EL KILL PARA EL REGISTRO");
    }
    printf("nombre pipe :%s\n", mon.pipe);
    do
    {
        fd = open(PipeDirectorio, O_WRONLY);
        if (fd == -1)
        {
            printf("Esperando que el pipe sea creado \n");
            sleep(1);
        }
    } while (fd == -1);
    printf("nombre pipe :%s\n", mon.pipe);
    write(fd, &mon, sizeof(mon));

    close(fd);
    printf("registro en el directorio completo\n");
    /*pipe para imprimir el valor de los sensores*/
    if (id != 0)
    {
        pause();
    }
    printf("sale de la espera\n");

    fd = 0;
    while (es)
    {
        escribir();
    }
    if (unlink(PipeSensores) == -1)
    {
        perror("No se puede eliminar PipeSensores");
    }
}