servidorhttp: src/http.c src/main.c src/server.c src/mime.c include/http.h include/server.h include/mime.h
	gcc -Wall -Wextra -Wshadow -Iinclude src/server.c src/http.c src/mime.c src/main.c -o servidorhttp

clean:
	rm -f servidorhttp

.PHONY: clean