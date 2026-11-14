#include "http.h"
#include <stdio.h>


int responder_error(const char *codigo, const char *mensaje, int nsd){

    char cabeceras[256];
    int cab = snprintf(cabeceras, sizeof(cabeceras),
    "HTTP/1.1 %s %s\r\n"
    "Allow: GET\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n"
    "\r\n",
    codigo, mensaje);

    return 0;
    
}