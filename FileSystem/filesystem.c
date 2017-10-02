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
	t_log_level LogL = LOG_LEVEL_TRACE;
	t_log* logFS = log_create("log.txt","YAMAFS",0,LogL);
	int nuevoSocket;

	nodos_file = "nodos.bin";
	nodos = list_create();

	t_list *carpetas;
	carpetas = malloc(sizeof(t_list)*100);
	carpetas = inicializarDirectorios(carpetas);

	t_arg_consola *args;
	args = malloc(sizeof(t_arg_consola));

	args->lista = carpetas;
	args->indice = 1;
	args->padre = 0;


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


	int er1 = pthread_create(&threadEscucharConsola,NULL,escucharConsola,(void*) args );
	int er2 = pthread_create(&threadEsperaConexiones, NULL,esperarConexiones,(void*) esperarConexion);

	pthread_join(threadEsperaConexiones, NULL);
	pthread_join(threadEscucharConsola, NULL);


	log_error(logFS,"Se sale forzadamente del fs.");
	log_destroy(logFS);
}

