 /*
 *  Created on: 6/9/2017
 *      Author: utnso
 */
#include "sockets.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/sendfile.h>
#include "protocolo.h"

#define TAMBUFFER 1024
#define MAX_CLIENTES 10



int escuchar(int puerto) {
	int socketEscucha;
	struct sockaddr_in address;
	if ((socketEscucha = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		return -1;

	}
	int activador = 1;
	if (setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, (char *) &activador,
			sizeof(activador)) < 0) {
		return -1;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(puerto);

	if (bind(socketEscucha, (struct sockaddr *) &address, sizeof(address))
			< 0) {
		printf("Error al bindear");
		return -1;
	}
	if (listen(socketEscucha, MAX_CLIENTES) < 0) {
		printf("Error al escuchar\n");
		return -1;
	}
	return socketEscucha;
}

int aceptarConexion(int socketEscucha) {
	int nuevoSocket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	if ((nuevoSocket = accept(socketEscucha, (struct sockaddr *) &address,
			(socklen_t*) &addrlen)) < 0) {
		return 1;
	}
	return nuevoSocket;
}

int conectarseA(char *ip, int puerto) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);
	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor))
			!= 0) {
		perror("No se pudo conectar\n");
		return 0;
	}
	return cliente;
}


int enviarMensaje(int socketDestino, char* mensaje) {
	int totalEnviado=0, bytesRestantes, lenEnviado;

	int len = strlen(mensaje);
	bytesRestantes = len;
	enviarInt(socketDestino, len);
	while(totalEnviado < len) {
		lenEnviado = send(socketDestino, mensaje, len, 0);
		if(lenEnviado == -1){ perror("error al enviar\n"); return -1;}
		totalEnviado = totalEnviado + lenEnviado;
		bytesRestantes = bytesRestantes - lenEnviado;
	}
	return len;
}




char *recibirMensaje(int socketDestino) {
	//size es el tamaño que previamente me envio socketDestino, y mensaje tiene ese tamaño
	int largoLeido = -1, size, llegoTodo = 0;
	recibirInt(socketDestino, &size);
	char *mensaje = malloc(size+1);
	while(!llegoTodo){
		largoLeido = recv(socketDestino, mensaje, size, 0);
		if(largoLeido == 0){ //toda esta fumada es para cuando algun cliente se desconecta.
			printf("Socket dice: Cliente en socket N° %d se desconecto\n", socketDestino);
			mensaje = "-1";
			return (mensaje);
		}
		//si el largo leido es menor que el size, me quedan cosas por leer, sino llego el mensaje completo
		if(largoLeido < size) size= size - largoLeido;
		else llegoTodo = 1;
	}
	mensaje[largoLeido] = '\0';
	return mensaje;
}

int enviarInt(int socketDestino, int num){
	int status;
	void* bufferEnviarInt = malloc(sizeof(int));
	memcpy(bufferEnviarInt,&num,sizeof(int));
	status = send(socketDestino,bufferEnviarInt,sizeof(int),0);
	free(bufferEnviarInt);
	return status;
}

int recibirInt(int socketDestino, int* i) {
	int len_leida;
	len_leida = recv(socketDestino, i, sizeof(int), 0);
	return len_leida;
}

void selectRead(int descriptor, fd_set* bag, struct timeval* timeout) {
	select(descriptor, bag, NULL, NULL, timeout);
}

void selectWrite(int descriptor, fd_set* bag, struct timeval* timeout) {
	select(descriptor, NULL, bag, NULL, timeout);
}

void selectException(int descriptor, fd_set* bag, struct timeval* timeout) {
	select(descriptor, NULL, NULL, bag, timeout);
}

int esperarConexionesSocket(fd_set *master, int socketEscucha) {
	//dado un set y un socket de escucha, verifica mediante select, si hay alguna conexion nueva para aceptar
	int nuevoSocket = -1;
	fd_set readSet;
	FD_ZERO(&readSet);
	readSet = *(master);
	if (select(socketEscucha + 1, &readSet, NULL, NULL, NULL) == -1) {
		perror("select");
		exit(4);
	}
	if (FD_ISSET(socketEscucha, &readSet)) {
		// handle new connections
		nuevoSocket = aceptarConexion(socketEscucha);
	}
	return nuevoSocket;
}

char * esperarMensajeSocket(fd_set *socketsClientes, int socketMaximo) {
	int i = -1;
	fd_set readSet;
	FD_ZERO(&readSet);
	readSet = *(socketsClientes);
	if (select(socketMaximo + 1, &readSet, NULL, NULL, NULL) == -1) {
		perror("select");
		exit(4);
	}
	for (i = 0; i <= socketMaximo; i++) {
		if (FD_ISSET(i, &readSet)) {
			char *mensaje = recibirMensaje(i);
			return mensaje;
		}
	}
}

int recibirHandShake (int unSocket) {
	int ret = 0, len;
	len = recibirInt(unSocket, &ret);
	if(len == -1) perror("error recibiendo handshake\n");
	return ret;
}

int envioArchivo(int peer_socket, char * archivo){

	ssize_t len;
	struct sockaddr_in      peer_addr;
	int fd;
	int sent_bytes = 0;
	char file_size[1024];
	struct stat file_stat;
	int offset;
	int remain_data;

    fd = open(archivo, O_RDONLY);
    if (fd == -1)
    {
            fprintf(stderr, "Error abriendo archivo --> %s", strerror(errno));

            exit(EXIT_FAILURE);
    }

    if (fstat(fd, &file_stat) < 0)
    {
            fprintf(stderr, "Error fstat --> %s", strerror(errno));

            exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Tamaño: \n%d bytes\n", file_stat.st_size);

    /* fprintf(stdout, "Acepto nodo --> %s\n", inet_ntoa(peer_addr.sin_addr));*/

    enviarInt(peer_socket, ENVIAR_ARCHIVO_TEXTO);
    /* Envio tamaño archivo */

    //len = send(peer_socket, file_size, sizeof(file_size), 0);
    enviarInt(peer_socket, file_stat.st_size);
    /* envio nombre archivo */
    // send(peer_socket, archivo, sizeof(archivo), 0);

    enviarMensaje(peer_socket, archivo);

    if (len < 0)
    {
          fprintf(stderr, "Error enviando datos preliminares --> %s", strerror(errno));

          exit(EXIT_FAILURE);
    }

    fprintf(stdout, "FS envia %d bytes de tamaño de archivo\n", len);

    offset = 0;
    remain_data = file_stat.st_size;

    /* Sending file data */
    while (((sent_bytes = sendfile(peer_socket, fd, &offset, TAMBUFFER)) > 0) && (remain_data > 0))
    {
            fprintf(stdout, "1. FS envio %d bytes de los datos de archivo, el offset es : %d y faltan = %d bytes\n", sent_bytes, offset, remain_data);
            remain_data -= sent_bytes;
            fprintf(stdout, "2. FS envio %d bytes de los datos de archivo, el offset es : %d y faltan = %d bytes\n", sent_bytes, offset, remain_data);
    }

    int fileClose = close(fd);

    if(fileClose == 0){
    	printf("Se cerro el archivo archivo %s", archivo);
    }
    close(peer_socket);
    return 0;

}
void *recibirArchivo(int client_socket){

			ssize_t len;
			char * nombre_archivo;
	        struct sockaddr_in remote_addr;
	        char buffer[TAMBUFFER];
	        int file_size;
	        FILE *received_file;


	        //recv(client_socket, buffer, TAMBUFFER, 0);
	        //file_size = atoi(buffer);

	        recibirInt(client_socket, &file_size);
	        int remain_data = file_size;
	        //recv(client_socket, nombre_archivo, TAMBUFFER, 0);
	        nombre_archivo = recibirMensaje(client_socket);
	        if(!strcmp(nombre_archivo,"-1")){
	        	printf("error al recibir el nombre del archivo");
	        }else{
	        	received_file = fopen(nombre_archivo, "w");

				if (received_file == NULL)
				{
						fprintf(stderr, "Fallo al abrir el archivo %s\n", strerror(errno));

						exit(EXIT_FAILURE);
				}
				while (((len = recv(client_socket, buffer, TAMBUFFER, 0)) > 0) && (remain_data > 0))
			   {
					   fwrite(buffer, sizeof(char), len, received_file);
					   remain_data -= len;
					   fprintf(stdout, "Recibidos %d bytes y se esperan :- %d bytes\n", len, remain_data);
			   }
			   fclose(received_file);

			   close(client_socket);
	        }

}

char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[255];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    if(isspace(s)){return s;}
	char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *remueveBlancos(char *s)
{
    return rtrim(ltrim(s));
}

