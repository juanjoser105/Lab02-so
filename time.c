#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    struct timeval tiempoActual;
    struct timeval tiempoFinal;

    gettimeofday(&tiempoActual, NULL);

    //Iniciamos el fork
    int child = fork();
    if (child == 0) {
        char *c_args[argc];

        for (int i=0; i<argc-1; i++){
            c_args[i] = argv[i+1];
        }    

        c_args[argc-1] = NULL;
        execvp(c_args[0], c_args);
    }
    //Si es menor a 0
    else if (child < 0) {
        fprintf(stderr, "An error has occurred\n");
        exit(1);
    } 

    //default
    else {
        int s;
        int mins;
        
        wait(NULL);
        gettimeofday(&tiempoFinal, NULL);
        
        s = tiempoFinal.tv_sec - tiempoActual.tv_sec;
        mins= tiempoFinal.tv_usec - tiempoActual.tv_usec;

        printf(" Segundos: %d Microsegundos : %d \n ", s,mins);
    }
    return 0;
}