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

	//persisto el programa reductor
	retorno = persistirPrograma(solicitudDeserializada->programa_reduccion, solicitudDeserializada->programa);
	if(retorno == -1 || retorno == -2 || retorno == -10){
		log_destroy(worker_error_log);
		return retorno;
	}

	int i;
	char* buffer = string_new();
	string_append(&buffer, "cat");

	for(i=0; i<solicitudDeserializada->cantidad_archivos_temp; i++){

		//concatena string con formato "cat temp1 temp2 temp3...tempN"
		string_append_with_format(&buffer, " %s",solicitudDeserializada->archivos_temporales[i].archivo_temp);

	}

	//concatena lo anterior con el sort y la ejecucion del programa de reduccion
	string_append_with_format(&buffer, " | sort | .\"/scripts/%s\" > %s",
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

	switch(exit_code){

	case 0:
		log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de reduccion local a Master");
		enviarMensajeSocketConLongitud(socket, REDUCCION_LOCAL_OK, NULL, 0);
		break;
	case -1:
		//enviar ERROR de creacion de programa de reduccion
		break;
	case -2:
		//enviar ERROR de escritura de programa de reduccion
		break;
	case -3:
		//enviar ERROR de llamada system() al concatenar, sortear y reducir
	case -10:
		//enviar ERROR de llamada system() al darle permisos al script
		break;
	case -6:
		log_error(worker_error_log, "Se envia aviso de error en etapa de reduccion local a Master");
		enviarMensajeSocketConLongitud(socket, REDUCCION_LOCAL_ERROR, NULL, 0);
		break;
	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);
}
