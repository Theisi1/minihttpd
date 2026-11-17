# MiniHTTPd
Mini Servidor HTTP/1.1, en Linux con sockets escritos en C.

## Qué hace
- El servidor acepta el método GET, en el protocolo de HTTP/1.1 y los archivos salen de la carpeta www/.
- Si un usuario pide / se sirve el index.html.
- Si un usuario pide ../ o alguna carpeta el error se responde con 403 (realpath / S_ISDIR)
- Se maneja 5 códigos de error los que son: 405 "Method Not Allowed" : sale en caso de que el método sea diferente de GET, 400 "Bad Request": El servidor no pudo procesar la petición pero es fallo del cliente, 404 "Not Found": El archivo solicitado no se encuentra o no existe, 403 "Forbidden": No se permite el acceso solicitado y 500 "Internal Server Error": Error por parte del servidor.
- La concurrencia se aplica con "epoll" quien nos permite controlar el como se atiende a varios clientes con un solo hilo. 

## Compilar 
```
make
```

## Ejecutar 
```
./servidorhttp 8080
```
- Por ejemplo el puerto 8080

## Probar 
```
curl -v http://localhost:8080/
```




