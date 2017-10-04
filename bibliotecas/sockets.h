/*
 * sockets.h
 *
 *  Created on: 6/9/2017
 *      Author: utnso
 */

#ifndef BIBLIOTECAS_SOCKETS_H_
#define BIBLIOTECAS_SOCKETS_H_


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
int enviarInt(int socketDestino, int num);
int recibirInt(int socketDestino, int* i);
void selectRead(int descriptor, fd_set* bag, struct timeval* timeout);
void selectWrite(int descriptor, fd_set* bag, struct timeval* timeout);
void selectException(int descriptor, fd_set* bag, struct timeval* timeout);
int recibirHandShake (int unSocket);
int envioArchivo(int peer_socket, char* archivo);
int esperarConexionesSocket(fd_set *master, int socketEscucha);
void *recibirArchivo(int client_socket);


char *replace_str(char *str, char *orig, char *rep);
char *remueveBlancos(char *str);
char *ltrim(char *s);
char *rtrim(char *s);

#endif /* BIBLIOTECAS_SOCKETS_H_ */
