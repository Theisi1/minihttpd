#include "mime.h"
#include <string.h>

const char *extension(const char *rutarchivo){
    const char *punto = strrchr(rutarchivo, '.');
    if(punto == NULL){
        return "application/octet-stream";
    }
    if(strcmp(punto, ".html") == 0){
        return "text/html";
    }
    if(strcmp(punto, ".css") == 0){
        return "text/css";
    }
    if(strcmp(punto, ".js") == 0){
        return "application/javascript";
    }
    if(strcmp(punto, ".png") == 0){
        return "image/png";
    }
    if(strcmp(punto, ".jpg") == 0){
        return "image/jpeg";
    }
    return "application/octet-stream";
}