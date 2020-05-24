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
    strcpy(listaMonit[tamm].pipe, mon.pipe);
    tamm++;
    close(fd);
    printf("El monitor %s fue agregado \n", listaMonit[tamm - 1].nombre);
    printf("con nombre de  %s fue agregado \n", listaMonit[tamm - 1].pipe);
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
    printf("mando nombre para la comunicacion Sensore-monitor: %s....\n", listaMonit[pos].pipe);
    do
    {
        fd = open(PipeDirectorio2, O_WRONLY);
        if (fd == -1)
        {
            perror("Pipe  de escritura con sensor no se enceuntra");
        }
    } while (fd == -1);
    /*se le manda el nombre del pipe para que se comuniquen*/
    write(fd, &listaMonit[pos], sizeof(listaMonit[i]));
    printf("pipe de comunicacion Sensor-Monitor mandado\n");
    close(fd);
}
typedef void (*sighandler_t)(int);
void iniciarS1();
void iniciarS2();

sighandler_t signalHandler(void)
{
    agregarMonitor();
            if (tamm == 1)
            {
                for (i = 0; i < tams; i++)
                {
                    if (kill(listaSen[i].proceso, SIGUSR1) == -1)
                    {
                        perror("NO SE PUDO HACER EL KILL");
                    }
                    else
                        mandarPipeMonitor();
                }
            }
     iniciarS1();
}
sighandler_t signalHandler2(void)
{
    agregarSensor();
    if (tamm != 0)
            {
                if (kill(listaSen[tams - 1].proceso, SIGUSR1) == -1)
                {
                    perror("NO SE PUDO HACER EL KILL");
                }
                else
                    mandarPipeMonitor();
            }
    iniciarS2();
}
void iniciarS1()
{
    signal(SIGUSR1, (sighandler_t)signalHandler);
    
}
void iniciarS2()
{
   signal(SIGUSR2, (sighandler_t)signalHandler2);
    
}
int main(int argc, char **argv)
{
    /*signal(SIGUSR1, (sighandler_t)signalHandler);
    signal(SIGUSR2, (sighandler_t)signalHandler2);*/
    iniciarS1();
    iniciarS2();

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

    mkfifo(PipeDirectorio1, fifo_mode);
    mkfifo(PipeDirectorio2, fifo_mode);
    mkfifo(PipeSensores1, fifo_mode);
    enum estados est = espera;
    char letra;
    int termina = 0;
    /**************se guarda el pid del monitor en un fichero*****************/
    int pid = getpid();
    printf("pid es: %d\n", pid);
    int  fl = open("pidMonitor.txt",O_WRONLY);
    if(fl == -1)
    {
        printf("Error no se encuentra el archivo\n");
    }
    write(fl, &pid,sizeof(pid));
    close(fl);

	/******************************************/
    while (!termina)
    {

        switch (est)
        {
        case espera:
           
            printf("\nPara listar los sensosres oprime la s");
            printf("\nPara listar los monitores oprome m");
            printf("\nPara listar hacer cambio de monitor oprime x");
            printf("\nPara terminar la ejecicon del programa oprime q \n");
            scanf("%c", &letra);

            
            if (letra == 's')
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
        case lsensores:
            for (i = 0; i < tams; i++)
            {
                printf("Nombre:\t %s proceso:\t %d \n ", listaSen[i].nombre, listaSen[i].proceso);
            }
            est = espera;

            break;
        case lmonitores:
            for (i = pos; i < tamm; i++)
            {
                printf("id:%d Nombre:\t%s proceso:\t%d\n", listaMonit[i].id, listaMonit[i].nombre, listaMonit[i].proceso);
            }
            est = espera;

            break;
        case exchange:
            if (pos + 1 >= tamm)
            {
                printf("no se puede hace el cambio no hay monitores disponibles");
            }
            else
            {
                pos++;
                /*Se manda la seña al monitor qu esta en uso, y que haora va a dejar de ser usado*/
                if (kill(listaMonit[pos - 1].proceso, SIGUSR1) == -1)
                {
                    perror("Error en el kill del monitor");
                }
                /*se manda al siguiente monitor que salga del pause*/
                if (kill(listaMonit[pos].proceso, SIGUSR2) == -1)
                {
                    perror("Kill de camibio mal ");
                }
                for (i = 0; i < tams; i++)
                {
                    printf("mandando señal a %d\n", listaSen[i].proceso);
                    sleep(1);
                    if (kill(listaSen[i].proceso, SIGUSR1) == -1)
                    {
                        perror("NO SE PUDO HACER EL KILL PARA LOS SENSORES");
                    }
                    sleep(1);
                    mandarPipeMonitor();
                }
            }
            est = espera;
            break;
        case salir:

            printf("xxxxx");

            termina = 1;
            break;
        }
    }
    for (i = 0; i < tams; i++)
    {
        sleep(1);
        printf("matando proceso %d\n", listaSen[i].proceso);
        if (kill(listaSen[i].proceso, SIGUSR2) == -1)
        {
            perror("NO SE PUDO HACER EL KILL PARA LOS SENSOR,EL PROCESO YA NO ESTA");
        }
    }
    sleep(1);
    for (i = pos; i < tamm; i++)
    {
        sleep(1);
        printf("matando monitor %d\n", listaMonit[i].proceso);
        if (kill(listaMonit[i].proceso, SIGUSR1) == -1)
        {
            perror("NO SE PUDO HACER EL KILL PARA EL MONITOR, EL PROCESO YA NO ESTA ");
        }
    }
    sleep(1);
    
    unlink(PipeSensores1);
    unlink(PipeDirectorio1);
    unlink(PipeDirectorio2);

}