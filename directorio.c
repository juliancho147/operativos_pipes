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
/*datos */
char *PipeDirectorio1;
char *PipeSensores1;
char *PipeDirectorio2;
int i;
struct sensor listaSen[100];
int tams = 0;
struct monitor listaMonit[100];
int tamm = 0;
int pos = 0;
mode_t fifo_mode = S_IRUSR | S_IWUSR;

/****************************************/
enum estados
{
    espera,
    aSensor,
    aMonitor,
    lsensores,
    lmonitores,
    exchange,
    salir
};
/*****************************/
/*****************************/

/*****************************/
void agregarMonitor()
{
    struct monitor mon;
    int fd;
    int byetes;
    printf("iniando lectura del monitor...\n");
    fd = open(PipeDirectorio1, O_RDONLY);
    do
    {
        if ((byetes = read(fd, &mon, sizeof(struct monitor))) > 0)
        {
            printf("leyendo monitor\n");
        }
    } while (byetes > 0);
    listaMonit[tamm].id, mon.id;
    strcpy(listaMonit[tamm].nombre, mon.nombre);
    listaMonit[tamm].proceso = mon.proceso;
    tamm++;
    close(fd);
    printf("El monitor %s fue agregado \n", listaMonit[tamm - 1].nombre);
}
void agregarSensor()
{

    struct sensor sen;
    int fd1;
    int bytes;
    printf("iniciando lectura del  sensor \n");
    fd1 = open(PipeSensores1, O_RDONLY);
    do
    {
        if ((bytes = read(fd1, &sen, sizeof(struct sensor))) > 0)
        {
            printf("leyendo sensor  \n");
        }
    } while (bytes > 0);
    strcpy(listaSen[tams].nombre, sen.nombre);
    listaSen[tams].valor = sen.valor;
    listaSen[tams].proceso = sen.proceso;
    tams++;
    close(fd1);
    printf("el sensor %s fue agregado \n", listaSen[tams - 1].nombre);
}
/*se manda el nombre del pipe para que el sensor se comunique con el monitor*/
void mandarPipeMonitor()
{
    int fd;
    char nombrePmoni[20];
    strcpy(nombrePmoni, listaMonit[pos].pipe);
    do
    {
        fd = open(PipeDirectorio2, O_WRONLY);
        if (fd == -1)
        {
            perror("Pipe  de escritura con sensor no se enceuntra");
        }
    } while (fd == -1);
    /*se le manda el nombre del pipe para que se comuniquen*/
    write(fd, &nombrePmoni, sizeof(nombrePmoni));
    close(fd);
}

int main(int argc, char **argv)
{

    if (argc != 7)
    {
        printf("Error, numero valores incompletos");
        return -1;
    }

    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-m") == 0)
        {

            PipeDirectorio1 = argv[i + 1];
            printf("Pipe que usan los monitores para mandar infomracion: %s\n ", PipeDirectorio1);
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            PipeSensores1 = argv[i + 1];
            printf("Pipe de para que los sensres se registren : %s\n ", PipeSensores1);
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            PipeDirectorio2 = argv[i + 1];
            printf("Pipe que manda el nombre del monitor a los sensores : %s \n", PipeDirectorio2);
        }
    }
    /* signal(SIGUSR1,(sighandler_t) signalHandlerSensor);*/

    mkfifo(PipeDirectorio1, fifo_mode);
    mkfifo(PipeDirectorio2, fifo_mode);
    mkfifo(PipeSensores1, fifo_mode);
    enum estados est = espera;
    char letra;
    int termina = 0;
    while (!termina)
    {
        switch (est)
        {
        case espera:
            printf("manual: \nPara agregar una monitor escribre la letra o");
            printf("\nPara agregar un sensor oprime la letra e");
            printf("\nPara listar los sensosres oprime la s");
            printf("\nPara listar los monitores oprome m");
            printf("\nPara listar hacer cambio de monitor oprime x");
            printf("\nPara terminar la ejecicon del programa oprime q \n");
            scanf("%c", &letra);
            if (letra == 'o')
            {
                est = aMonitor;
            }
            else if (letra == 'e')
            {
                est = aSensor;
            }
            else if (letra == 's')
            {
                est = lsensores;
            }
            else if (letra == 'm')
            {
                est = lmonitores;
            }
            else if (letra == 'x')
            {
                est = exchange;
            }
            else if (letra == 'q')
            {
                termina = 1;
                est = salir;
            }

            break;
        case aSensor:
            agregarSensor();
            if (tamm != 0)
            {
                mandarPipeMonitor();
            }
            est = espera;
            break;
        case aMonitor:
            agregarMonitor();
            if (tamm == 1)
            {
                for (i = 0; i < tams ; i++)
                {
                    if (kill(listaSen[i].proceso, SIGUSR1) == -1)
                    {
                        perror("NO SE PUDO HACER EL KILL");
                    }
                    mandarPipeMonitor();
                }
            }
            est = espera;
            break;
        case lsensores:
            for (i = 0; i < tams; i++)
            {
                printf("Nombre:\t %s proceso:\t %d \n ", listaSen[i].nombre, listaSen[i].proceso);
            }

            break;
        case lmonitores:
            for (i = pos; i < tamm; i++)
            {
                printf("id:\n %d Nombre:\t %s proceso:\t %d\n", listaMonit[i].id, listaMonit[i].nombre, listaMonit[i].proceso);
            }

            break;
        case exchange:
            pos++;
            if (kill(listaMonit[pos].proceso, SIGUSR1) == -1)
            {
                perror("Error en el kill del monitor");
            }
            for (i = 0; i < tams ; i++)
            {
                if (kill(listaSen[i].proceso, SIGUSR1) == -1)
                {
                    perror("NO SE PUDO HACER EL KILL PARA LOS SENSORES");
                }
                mandarPipeMonitor();
            }

            break;
        case salir:
            break;
        }
    }
    unlink(PipeSensores1);
    unlink(PipeDirectorio1);
    unlink(PipeDirectorio2);
}