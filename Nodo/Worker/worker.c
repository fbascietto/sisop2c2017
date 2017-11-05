/*
 * nodo.c
 *
 *  Created on: 14/9/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "../../bibliotecas/sockets.h"
#include "funcionesWorker.h"


void main() {

	int retorno_mkdir;

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log_error = log_create("logWorker.txt", "WORKER", 1, level_ERROR);


	//creo directorio de scripts
	retorno_mkdir = mkdir("scripts", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(retorno_mkdir == 0){
		log_trace(worker_log, "Se crea el directorio scripts");
	}else{
		log_error(worker_log_error, "El directorio scripts no pude crearse porque ya existia");
	}

	/*testeo de la funcion de persistencia de programas.OK
	persistirPrograma("test", "contenido de prueba", "testing");*/
	iniciarWorker();
	log_trace(worker_log, "Lectura de archivo de configuracion");

	int socketEscucha;
	fd_set fdSocketsEscucha;
	FD_ZERO(&fdSocketsEscucha);


	socketEscucha= escuchar(puerto);
	FD_SET(socketEscucha, &fdSocketsEscucha);


	t_esperar_conexion *esperarConexion;

	esperarConexion = malloc(sizeof(t_esperar_conexion));

	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;

	log_trace(worker_log, "Creacion socket para conexion con Master");

	while(1){

		esperarConexionesMaster((void*) esperarConexion);

	}

}
