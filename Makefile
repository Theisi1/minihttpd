servidorhttp: src/http.c src/main.c src/server.c src/mime.c src/files.c include/http.h include/server.h include/mime.h include/files.h
	gcc -Wall -Wextra -Wshadow -Iinclude src/server.c src/http.c src/mime.c src/files.c src/main.c -o servidorhttp

clean:
	rm -f servidorhttp

.PHONY: clean