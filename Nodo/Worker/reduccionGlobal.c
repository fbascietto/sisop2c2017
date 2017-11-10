/*
 * reduccionGlobal.c
 *
 *  Created on: 4/11/2017
 *      Author: utnso
 */

#include "etapas.h"
#include <commons/log.h>
#include <commons/string.h>
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"
#include <semaphore.h>

sem_t sem;

int reduccionGlobal(solicitud_programa_reduccion_global* solicitudDeserializada, int puerto){

	//para recorrer array
	int i;
	//para realizar conexion con otros workers
	int socket;

	//retorno de la funcion que persiste el programa de reduccion
	int retorno;

	//persisto el programa reductor
	retorno = persistirPrograma(solicitudDeserializada->programa_reduccion, solicitudDeserializada->programa);
	if(retorno == -1 || retorno == -2 || retorno == -10){
		return retorno;
	}

	//para ver si uno de los archivos de reduccion local reside en el worker encargado
	for(i=0; i<solicitudDeserializada->cantidad_item_programa_reduccion; i++){

		if(puerto == solicitudDeserializada->items_programa_reduccion_global[i].puerto_worker){

			//se utiliza un archivo temporal local

		}else{

			//se debe pedir los registros mediante puerto/ip
			socket = conectarseA(solicitudDeserializada->items_programa_reduccion_global[i].ip_worker,
									solicitudDeserializada->items_programa_reduccion_global[i].puerto_worker);
			enviarInt(socket, PROCESO_WORKER);
			enviarMensajeSocketConLongitud(socket, ACCION_ENVIAR_ARCHIVO_TEMP_DE_RL, NULL, 0);

		}

	}

	return 0;
}

void responderSolicitudRG(int socket, int exit_code){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	switch(exit_code){

	case 0:
		log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de reduccion global a Master");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_OK, NULL, 0);
		break;
	case -1:
		//enviar ERROR de creacion de programa de reduccion
		break;
	case -2:
		//enviar ERROR de escritura de programa de reduccion
		break;
	case -10:
		//enviar ERROR de llamada system() al darle permisos al script
		break;
	case -9:
		log_error(worker_error_log, "Se envia aviso de error en etapa de reduccion global a Master");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_ERROR, NULL, 0);
		break;
	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);

}


int leerYEnviarArchivoTemp(solicitud_leer_y_enviar_archivo_temp* solicitudDeserializada, int socket){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	//fichero para recorrer el archivo de reduccion local
	FILE* f1;

	//buffer donde pongo cada registro (linea) que voy a enviar
	char* buffer;
	//entero donde almaceno longitud del archivo para reservar memoria en el malloc
	int longitud_archivo_temporal;

	//Inicializacion del semaforo

	/*pshared = 0 significa que el semaforo no se comparte entre proceso
	**pshared = 1 que si se comparte*/
	int pshared;
	int retorno;
	int value;

	//solo este proceso utiliza el semaforo => 0 de privado
	pshared = 0;
	//inicializo en 1 para que pueda leer un registro y enviar antes de bloquearse
	value = 1;
	retorno = sem_init(&sem, pshared, value);
	if(retorno != 0){
		log_error(worker_error_log, "No se pudo inicializar el semaforo");
		return -5;
	}

	signal(sem);

	f1 = fopen(solicitudDeserializada->ruta_archivo_red_local_temp, "r");
	if(f1 == NULL){
		log_error(worker_error_log, "No se pudo abrir el archivo temporal de reduccion local para recorrerlo");
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		return -3;
	}

	//me posiciono al final del archivo
	retorno = fseek(f1, 0, SEEK_END);
	if(retorno!=0){
		log_error(worker_error_log, "No se pudo posicional al final del archivo temporal");
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		return -4;
	}
	//determino longitud del archivo
	longitud_archivo_temporal = ftell(f1);
	//me posiciono al principio del archivo para poder leer
	rewind(f1);

	buffer = malloc(longitud_archivo_temporal);

	while(!feof(f1)){

		wait(sem);

		//leo de a un registro (una linea porque ya viene ordenado el archivo) para guardar en buffer y enviar
		fgets(buffer, longitud_archivo_temporal, f1);

		log_trace(worker_log, "Se envia al worker encargado un registro para la reduccion global");
		enviarMensajeSocketConLongitud(socket, ACCION_RECIBIR_REGISTRO, buffer, strlen(buffer));


	}

	free(buffer);

	return 0;

}

void recibirArchivoTemp(solicitud_recibir_archivo_temp* solicitudDeserializada){

}

void habilitarSemaforo(){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* sem_log = log_create("logWorker.txt", "WORKER", 1, level);

	log_trace(sem_log, "Se habilita el semaforo para enviar el siguiente registro");
	signal(sem);

}

