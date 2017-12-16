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
#include <errno.h>
#include <fcntl.h>
#include <signal.h>


int valorSocket;

typedef struct {
	char * mensajeRecibido;
	int socketQueEnvia;
} t_resultadoEsperarMensaje;

typedef struct Package {
	uint32_t msgCode;
	uint32_t message_long;
	char* message;
} Package;

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
int recibirArchivo(int client_socket);

char* fileToChar(char* fileName);

//usar esta funcion para enviar un buffer.
int enviarMensajeSocketConLongitud(int socket, uint32_t accion, char* mensaje, uint32_t longitud);
//usar estas funciones para recibir un buffer, el resultado queda en package.
Package* createPackage();
int recieve_and_deserialize(Package *package, int socketCliente);

/*Estas funciones son utilizadas por enviarMensajeSocketConLongitud y recieve_and_deserialize
No es necesario utilizarlas directamente*/
Package* fillPackage(uint32_t msgCode, char* message, uint32_t message_long); //crear un Package con datos
void destroyPackage(Package* package);
int getLongitudPackage(Package *package);
int enviarMensajeSocket(int socket, uint32_t accion, char* mensaje);
int leerSocketClient(int fd, char *datos, int longitud);
int escribirSocketClient(int fd, char *datos, int longitud);

char *replace_str(char *str, char *orig, char *rep);

#endif /* BIBLIOTECAS_SOCKETS_H_ */
