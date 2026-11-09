#include <string.h>   //memset
#include <stdio.h>  //perror
#include <stdlib.h> // exit
#include <unistd.h> //read, write y close
#include <fcntl.h>  // open O_RDONLY
#include <sys/types.h>  //getaddrinfo y freeaddrinfo
#include <sys/socket.h> // socket, bind, listen y accept
#include <netdb.h>

int ConnectionPassiveTCP(char *service, int qlen){
    int sd, s;
    int act = 1;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, service, &hints, &res);
    if(s != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return(-1);
    } 
    for ( p = res; p != NULL; p = p->ai_next){
        sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sd == -1){
            perror("No se pudo crear el socket ");
            continue;
        }     
        if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &act, sizeof(act)) == -1){
            perror("Error en setsockpot");
            close(sd);
            continue;
        }
        if(bind(sd, p->ai_addr, p->ai_addrlen) == -1){
            perror("Error en el bind");
            close(sd);
            continue;
        }
        break;
    }

    freeaddrinfo(res);
    if(p == NULL){      //resume los errores de los bucles del for
        fprintf(stderr, "No hay nodo\n");
        return(-1);
    }

    if(listen(sd, qlen) == -1){
        perror("No se escucha ninguna conexión");
        close(sd);
        return -1;
    }
    //fuera del if de listen para que al final de la llamada retorne el socket sd
    return sd;
}

int server(char *service){
    int sd = ConnectionPassiveTCP(service, 5);
    int nsd, nread;
    if(sd == -1){
        return -1;
    }

    struct sockaddr_storage cltAddr;   // (dirección) suficientemente grande para cualquier familia ya sea Inet o unspec(cualquiera)
    socklen_t l;        // (tamaño)

    while (1){
        l = sizeof(cltAddr);
        nsd = accept(sd, (struct sockaddr *)&cltAddr, &l);
        if(nsd == -1){
            perror("Error en el accept");
            continue;
        }
        //lectura
        char buf[1024];
        nread = read(nsd, buf, sizeof(buf) - 1);
        if (nread <= 0){
            close(nsd);
            continue;
        }
        buf[nread] = '\0';

        char metodo[64];
        char ruta[1024];
        char version[128];

        int scaner = sscanf(buf,"%63s %1023s %127s", metodo, ruta, version);
        if(scaner != 3){
            close(nsd);
            continue;
        }

        char comp_ruta[1028];
        int comparacion = strcmp(ruta, "/");
        if(comparacion == 0){
            snprintf(comp_ruta, sizeof(comp_ruta), "www%s", "/index.html");
        }else{
            snprintf(comp_ruta, sizeof(comp_ruta), "www%s", ruta);

        }

        int fd = open(comp_ruta, O_RDONLY);
        if (fd == -1){
            close(nsd);
            close(fd);
            continue;
        }

        close(nsd);
    }
}

int main (int argc, char *argv[]){
    if (argc < 2){  //si no se escribio el puerto 
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