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
}