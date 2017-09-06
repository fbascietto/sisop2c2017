/*
 * sockets.c
 *
 *  Created on: 6/9/2017
 *      Author: utnso
 */
#include "sockets.h"

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

void enviarInt(int socketDestino, int num) {
	int status;
	void* bufferEnviarInt = malloc(sizeof(int));
	memcpy(bufferEnviarInt,&num,sizeof(int));
	status = send(socketDestino,bufferEnviarInt,sizeof(int),0);
	free(bufferEnviarInt);
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






