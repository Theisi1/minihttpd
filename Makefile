servidorhttp: src/http.c src/main.c src/server.c include/http.h include/server.h
	gcc -Wall -Wextra -Wshadow -Iinclude src/server.c src/http.c src/main.c -o servidorhttp

clean:
	rm -f servidorhttp

.PHONY: clean