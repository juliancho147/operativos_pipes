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
#include <time.h>
/**variables***/
char nomActual[30] = "NULL";
int fdn = 0;
char *PipeSensores1;
char *PipeDirectorio2;
char *nombreSensor;
int valor;
int seg;
int Esrandom = 0;
int i;
int bytes = 0;
struct monitor mon;
/***************************/
void leerNombreActual()
{
    /*pipe que recibe el nombre para la comunicacion con el monitor */
    bytes = 0;
    fdn = open(PipeDirectorio2, O_RDONLY);
    do
    {
        if ((bytes = read(fdn, &mon, sizeof(mon))) > 0)
        {
            printf("leyendo nombre del pipe de comunicacion con el monitor \n");
        }
    } while (bytes > 0);
    close(fdn);
    strcpy(nomActual, mon.pipe);
    printf("nombre de comunicacion %s \n", nomActual);
}
typedef void (*sighandler_t)(int);
sighandler_t signalHandler(void)
{
    leerNombreActual();
}

int main(int argc, char **argv)
{
    signal(SIGUSR1, (sighandler_t)signalHandler);
    if (argc < 9)
    {
        printf("Error, numero valores incompletos");
        return -1;
    }

    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {

            PipeSensores1 = argv[i + 1];
            printf("Pipe de registro en el directorio: %s\n ", PipeSensores1);
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            PipeDirectorio2 = argv[i + 1];
            printf("Pipe para  lectura del directorio: %s\n ", PipeDirectorio2);
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            nombreSensor = argv[i + 1];
            printf("Nombre del senso: %s\n", nombreSensor);
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            valor = atoi(argv[i + 1]);
            printf("Valor del sensor: %d\n ", valor);
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
            Esrandom = 1;
            printf("Es random\n");
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            seg = atoi(argv[i + 1]);
            printf("Tiempo entre los valores genrados: %d\n ", seg);
        }
    }
    srand(time(NULL));
    int fd1;
    struct sensor sen;
    strcpy(sen.nombre, nombreSensor);
    sen.proceso = getpid();
    if (Esrandom)
    {
        sen.valor = rand() % (0 + 99);
    }
    /*pipe para mandar la indromacion de registro al directorio*/

    int entro = 0;
    do
    {
        fd1 = open(PipeSensores1, O_WRONLY);
        if (fd1 == -1)
        {
            printf("esperando pipe de conexion con directorio \n");
            sleep(5);
        }
        else
        {
            entro = 1;
        }

    } while (!entro);

    write(fd1, &sen, sizeof(struct sensor));
    close(fd1);
    printf("registo completado\n");

    /*se manda la informacion al monitor*/

    while (1)
    {
        
        fdn = 0;
        if (strcmp(nomActual, " ") != 0)
        {
            
                fdn = open(nomActual, O_WRONLY);
                do
                {
                    fdn = open(nomActual, O_WRONLY);
                } while (fdn == -1);

                write(fdn, &sen, sizeof(sen));

                close(fdn);
                sleep(seg);
                sen.valor = rand() % (0 + 99);
            
        }
    }
}