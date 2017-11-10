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
	t_log_level level_INFO = LOG_LEVEL_INFO;
	t_log* worker_log_info = log_create("logWorker.txt", "WORKER", 1, level_INFO);


	//creo directorio de scripts
	retorno_mkdir = mkdir("scripts", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(retorno_mkdir == 0){
		log_trace(worker_log, "Se crea el directorio scripts");
	}else{
		log_info(worker_log_info, "El directorio scripts ya existe");
	}

	/*testeo de la funcion de persistencia de programas.OK
	**persistirPrograma("script_test.sh", "abcdefghijklmnopqrstuvwxyz 123456789101112131415161718192021222324252627282930 ea funciono");*/
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

	log_destroy(worker_log);
	log_destroy(worker_log_info);

	while(1){

		esperarConexionesMasterYWorker((void*) esperarConexion);

	}

}
