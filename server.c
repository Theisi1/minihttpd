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
    int nsd;
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
    }
}