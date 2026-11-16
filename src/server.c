#include <string.h>   
#include <stdio.h>  
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/socket.h> 
#include <sys/stat.h>  
#include <netdb.h>
#include <errno.h>
#include <sys/epoll.h>
#include "http.h"
#include "mime.h"
#include "server.h"
#include "files.h"

static int ConnectionPassiveTCP(char *service, int qlen){
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

static int poner_no_bloqueante(int fd){
    int flags =  fcntl(fd, F_GETFL); 
    if(flags == -1 ){
        perror("Falló fcntl F_GETFL ");
        return -1;
    }
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK)== -1){
        perror("Fallo fcntl F_SETFL");
        return -1;
    }
    return 0;
}

int server(char *service){
    int sd = ConnectionPassiveTCP(service, 5);
    int nsd;
    ssize_t nread;
    if(sd == -1){
        return -1;
    }
    int epfd = epoll_create1(0);     
    if(epfd == -1){
        perror("Fallo epoll");
        close(sd);
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
            responder_error("400", "Bad Request", nsd);
            close(nsd);
            continue;
        }

        char comp_ruta[1028];

        if( strcmp(metodo, "GET") != 0 ){
            responder_error("405", "Method Not Allowed", nsd);
            close(nsd);
            continue;
        }

        int fd = abrir_archivo(ruta, comp_ruta, sizeof(comp_ruta));
        if (fd == -1){
            if(errno == ENOENT){
                responder_error("404", "Not Found", nsd);
                close(nsd);
                continue;
            }
            if(errno == EACCES  ){
                responder_error("403", "Forbidden", nsd);
                close(nsd);
                continue;
            }
            perror("open");
            responder_error("500","Internal Server Error", nsd);
            close(nsd);
            continue;
        }
        struct stat info;
        int st = fstat(fd, &info); 
        if (st == -1){
            perror("Error en el fstat");
            responder_error("500","Internal Server Error", nsd);
            close(fd);
            close(nsd);
            continue;
        }

        if(S_ISDIR(info.st_mode)){
            responder_error("403", "Forbidden", nsd);
            close(fd);
            close(nsd);
            continue;
        }
    
        char header[256]; 
        int head = snprintf(header, sizeof(header), 
        "HTTP/1.1 200 OK\r\n"
        "Content-type: %s\r\n"
        "Content-Length: %lld\r\n"
        "Connection: close\r\n" 
        "\r\n", 
        extension(comp_ruta), (long long) info.st_size);

        ssize_t wr = write(nsd, header, head);    
        if(wr == -1){
            perror("Error en write de cabecera");
            close(fd);
            close(nsd);
            continue;
        }

        char cuerpo[4096]; 
        while ((nread = read(fd, cuerpo, sizeof(cuerpo))) > 0){
            ssize_t sends = 0;
            while(sends < nread){
                ssize_t writ = write(nsd, cuerpo + sends, nread - sends);
                if(writ == -1){
                    perror("No se envio todo el archivo");
                    break;
                }
                sends += writ;        
            }
            if(sends < nread){
                break;
            }    
        }

        if( nread == -1){
            perror("Error en leer el archivo");
        }
        

        close(fd);
        close(nsd);
    }
}
