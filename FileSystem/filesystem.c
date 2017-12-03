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
#include <signal.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#define PUERTO_FS 5141

void main(int argc, char *argv[]){
	t_log_level LogL = LOG_LEVEL_TRACE;
	t_log* logFS = log_create("log.txt","YAMAFS",0,LogL);

	/*creo carpeta metadata*/
	int status = mkdir("metadata", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	/*creo carpeta bitmap*/
	status = mkdir("metadata/bitmap", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	/*creo carpeta para archivos*/
	status = mkdir("metadata/archivos", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	nodos_file = "nodos.bin";
	archivos_file = "archivos.dat";
	cantNodos = 0;
	formatted = 0;
	nodos = list_create();
	pthread_mutex_init(&mx_nodobin, NULL);

	int socketEscucha;
	fd_set fdSocketsEscucha;

	FD_ZERO(&fdSocketsEscucha);
	socketEscucha = escuchar(PUERTO_FS);

	FD_SET(socketEscucha, &fdSocketsEscucha);

	pthread_t threadEsperaConexiones;
	pthread_t threadEscucharConsola;
	t_esperar_conexion *esperarConexion;

	if(argc == 2 && !strcmp(argv[1],"--clean")){
		levantarNodos(1);
	} else if (argc == 1){
		levantarNodos(0);
	} else if (argc > 2){
		printf("Demasiados parámetros. YamaFS se inicializa con --clean o sin argumentos.\n");
		return;
	}

	esperarConexion = malloc(sizeof(t_esperar_conexion));
	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;


	int er1 = pthread_create(&threadEscucharConsola,NULL,escucharConsola,NULL);
	int er2 = pthread_create(&threadEsperaConexiones, NULL,esperarConexiones,(void*) esperarConexion);

	pthread_join(threadEscucharConsola, NULL);
	//pthread_join(threadEsperaConexionstring_append(&es, NULL);
	pthread_kill(threadEsperaConexiones, SIGUSR1);

	log_trace(logFS,"Se sale del fs.");
	log_destroy(logFS);

	/*
	if(carpetas!=NULL){
	list_destroy(carpetas);}

	if(nodos != NULL){
	list_destroy(nodos);}
	 */

	free(esperarConexion);

}

