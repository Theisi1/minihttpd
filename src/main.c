#include "server.h"
#include <stdio.h>

int main (int argc, char *argv[]){
    if (argc < 2){  
        fprintf(stderr, "Falta el puerto:\n");
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        return 1;
    }
    int estado = server(argv[1]);
    if (estado == -1){
        return 1;
    }
    return 0;
}