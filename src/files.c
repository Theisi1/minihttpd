#include "files.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int abrir_archivo(const char *ruta, char *comp_ruta, size_t tam){
    int comparacion = strcmp(ruta, "/");
    if(comparacion == 0){
        snprintf(comp_ruta, tam, "www%s", "/index.html");
    }else{
        snprintf(comp_ruta, tam, "www%s", ruta);
    }
    int fd = open(comp_ruta, O_RDONLY);
    return fd;
}
