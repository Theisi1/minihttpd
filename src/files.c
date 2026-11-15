#include "files.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>

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
    char real[PATH_MAX];
    if(realpath(comp_ruta, real) == NULL){
        return -1;
    }
    if(strncmp(real, raiz, strlen(raiz)) != 0 || (real[strlen(raiz)] != '/' && real[strlen(raiz)] != '\0' )){
        errno = EACCES;
        return -1;
    }
    int fd = open(real, O_RDONLY);
    return fd;
}
