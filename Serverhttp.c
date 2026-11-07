#include #include #include #include #include #include #include
int PassiveTCP(char *service, int qlen) {
int sd;
struct addrinfo hints, *res;

memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;

if (getaddrinfo(NULL, service, &hints, &res) != 0) {
perror("getaddrinfo");
exit(1);
}

sd = socket(res->ai_family, res->ai_socktype,
res->ai_protocol);

bind(sd, res->ai_addr, res->ai_addrlen);

listen(sd, qlen);

freeaddrinfo(res);

return sd;
}

int main() {
int sd, nsd, fd, nread;
char buf[1024];

struct sockaddr_storage cltAddr;
socklen_t l;

char fname[] = "index.html";

sd = PassiveTCP("8080", 5);

while (1) {
l = sizeof(cltAddr);

nsd = accept(sd, (struct sockaddr *)&cltAddr, &l);

if (nsd < 0)
continue;

/* Read HTTP request */
nread = read(nsd, buf, sizeof(buf) - 1);

if (nread <= 0) {
close(nsd);
continue;
}

buf[nread] = '\0';

/* Open HTML file */
fd = open(fname, O_RDONLY);

if (fd < 0) {
close(nsd);
continue;
}

/* Send HTTP response headers */
char header[] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n"
"\r\n";

write(nsd, header, sizeof(header) - 1);

/* Send HTML file */
while ((nread = read(fd, buf, sizeof(buf))) > 0) {
write(nsd, buf, nread);
}

close(fd);
close(nsd);
}

close(sd);

return 0;
}
