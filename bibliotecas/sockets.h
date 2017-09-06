/*
 * sockets.h
 *
 *  Created on: 6/9/2017
 *      Author: utnso
 */

#ifndef BIBLIOTECAS_SOCKETS_H_
#define BIBLIOTECAS_SOCKETS_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct {
	char * mensajeRecibido;
	int socketQueEnvia;
} t_resultadoEsperarMensaje;


int escuchar(int puerto);
int aceptarConexion(int socketEscucha);
int conectarseA(char *ip, int puerto);
int enviarMensaje(int socketDestino, char* mensaje);
char *recibirMensaje(int socketDestino);
void enviarInt(int socketDestino, int num);
int recibirInt(int socketDestino, int* i);
void selectRead(int descriptor, fd_set* bag, struct timeval* timeout);
void selectWrite(int descriptor, fd_set* bag, struct timeval* timeout);
void selectException(int descriptor, fd_set* bag, struct timeval* timeout);
int esperarConexiones(fd_set *master, int socketEscucha);
int recibirHandShake (int unSocket);



#endif /* BIBLIOTECAS_SOCKETS_H_ */
