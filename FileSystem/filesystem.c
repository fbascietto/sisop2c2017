/*
 * filesystem.c
 *
 *  Created on: 3/9/2017
 *      Author: utnso
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "funcionesfs.h"
#include <pthread.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

void main(){
	t_log_level LogL;
	t_log* logFS = log_create("log.txt","YAMAFS",0,LogL);

	int socketEscucha, socketArchivo;
	fd_set fdSocketsEscucha;

	FD_ZERO(&fdSocketsEscucha);
	socketEscucha = escuchar(4000);

	socketEscucha= escuchar(4000);
	FD_SET(socketEscucha, &fdSocketsEscucha);

	pthread_t threadEsperaConexiones;
	pthread_t threadEscucharConsola;
	t_esperar_conexion *esperarConexion;

	esperarConexion = malloc(sizeof(t_esperar_conexion));


	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;


	int er1 = pthread_create(&threadEscucharConsola,NULL,escucharConsola,NULL);
	int er2 = pthread_create(&threadEsperaConexiones, NULL,esperarConexiones,(void*) esperarConexion);


	while(1);

	log_error(logFS,"Se sale forzadamente del fs.");
	log_destroy(logFS);
}

