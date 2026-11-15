#include "files.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>

int abrir_archivo(const char *ruta, char *comp_ruta, size_t tam){
    int comparacion = strcmp(ruta, "/");
    if(comparacion == 0){
        snprintf(comp_ruta, tam, "www%s", "/index.html");
    }else{
        snprintf(comp_ruta, tam, "www%s", ruta);
    }
    char raiz[PATH_MAX];
    if(realpath("www", raiz ) == NULL){
        return -1;
    }
    int fd = open(comp_ruta, O_RDONLY);
    return fd;
}
