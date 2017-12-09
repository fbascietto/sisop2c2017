/*
 * reduccionLocal.c
 *
 *  Created on: 4/11/2017
 *      Author: utnso
 */

#include "etapas.h"
#include <commons/log.h>
#include <commons/string.h>
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"


int reduccionLocal(solicitud_programa_reduccion_local* solicitudDeserializada){

	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	//retorno de la funcion que persiste el programa de reduccion
	int retorno;

	int i;
	char* buffer = string_new();
	string_append(&buffer, "cat");

	for(i=0; i<solicitudDeserializada->cantidad_archivos_temp; i++){

		//concatena string con formato "cat temp1 temp2 temp3...tempN"
		string_append_with_format(&buffer, " \"%s\"",solicitudDeserializada->archivos_temporales[i].archivo_temp);

	}

	//concatena lo anterior con el sort y la ejecucion del programa de reduccion
	string_append_with_format(&buffer, " | sort | .\"/scripts/%s\" > \"%s\"",
			solicitudDeserializada->programa_reduccion, solicitudDeserializada->archivo_temporal_resultante);

	retorno = system(buffer);
	if(retorno == -1){
		log_error(worker_error_log, "No se pudo realizar la reduccion local");
		log_destroy(worker_error_log);
		free(buffer);
		return -3;
	}

	free(buffer);
	return 0;

}

void responderSolicitudRL(int socket, int exit_code){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	printf("Numero socket: %d. Exit code: %d\n", socket, exit_code);

	int enviados;

	switch(exit_code){

	case 0:
		log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de reduccion local a Master");
		enviados = enviarInt(socket, REDUCCION_LOCAL_OK);
		printf("Bytes enviados a Master: %d\n", enviados);
		break;
	case -1:
		log_error(worker_error_log, "Se envia a Master el error de creacion del programa de reduccion");
		enviados = enviarInt(socket, REDUCCION_LOCAL_ERROR_CREACION);
		printf("Bytes enviados a Master: %d\n", enviados);
		break;
	case -2:
		log_error(worker_error_log, "Se envia a Master el error de escritura del contenido del programa de reduccion");
		enviados = enviarInt(socket, REDUCCION_LOCAL_ERROR_ESCRITURA);
		printf("Bytes enviados a Master: %d\n", enviados);
		break;
	case -3:
		log_error(worker_error_log, "Se envia a Master el error al realizar la llamada system para concatenar los archivos y ejecutar el script");
		enviados = enviarInt(socket, REDUCCION_LOCAL_ERROR_SYSTEM);
		printf("Bytes enviados a Master: %d\n", enviados);
		break;
	case -10:
		log_error(worker_error_log, "Se envia a Master el error al dar permisos de ejecucion al programa de reduccion");
		enviados = enviarInt(socket, REDUCCION_LOCAL_ERROR_PERMISOS);
		printf("Bytes enviados a Master: %d\n", enviados);
		break;

	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);
}
