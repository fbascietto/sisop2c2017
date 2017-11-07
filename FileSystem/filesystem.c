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
#include <sys/stat.h>
#include <sys/types.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

void main(){
	t_log_level LogL = LOG_LEVEL_TRACE;
	t_log* logFS = log_create("log.txt","YAMAFS",0,LogL);

	/*creo carpeta metadata*/
	int status = mkdir("metadata", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	/*creo carpeta bitmap*/
	status = mkdir("metadata/bitmap", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	/*creo carpeta para archivos*/
	status = mkdir("metadata/archivos", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	nodos_file = "nodos.bin";
	nodos = list_create();
	pthread_mutex_init(&mx_nodobin, NULL);



	int socketEscucha;
	fd_set fdSocketsEscucha;

	FD_ZERO(&fdSocketsEscucha);
	socketEscucha = escuchar(5140);

	FD_SET(socketEscucha, &fdSocketsEscucha);

	pthread_t threadEsperaConexiones;
	pthread_t threadEscucharConsola;
	t_esperar_conexion *esperarConexion;


	levantarNodos(1);

	esperarConexion = malloc(sizeof(t_esperar_conexion));
	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;


	int er1 = pthread_create(&threadEscucharConsola,NULL,escucharConsola,NULL);
	int er2 = pthread_create(&threadEsperaConexiones, NULL,esperarConexiones,(void*) esperarConexion);


	pthread_join(threadEscucharConsola, NULL);


	log_error(logFS,"Se sale forzadamente del fs.");
	log_destroy(logFS);

	list_destroy_and_destroy_elements(nodos,free);
	free(esperarConexion);
	exit(1);
}

