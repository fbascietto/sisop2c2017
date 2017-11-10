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

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	//entero para iterar array
	int i;
	//entero para depositar longitud de archivos temporales y leerlos completos
	int longitud_archivo_temporal;
	//para guardar el contenido de 1 archivo
	char* buffer;
	FILE* f1;
	int leidos;

	//retorno de la funcion que persiste el programa de reduccion
	int retorno;

	//persisto el programa reductor
	retorno = persistirPrograma(solicitudDeserializada->programa_reduccion, solicitudDeserializada->programa);
	if(retorno == -1 || retorno == -2 || retorno == -10){
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		return retorno;
	}

	//para guardar el contenido de todos los archivos
	char* buffer_total = string_new();

	for(i=0; i<solicitudDeserializada->cantidad_archivos_temp; i++){

		//abro el archivo temporal de transformacion
		f1 = fopen(solicitudDeserializada->archivos_temporales[i].archivo_temp, "r");
		if (f1==NULL){
			log_error(worker_error_log, "No se pudo abrir el archivo temporal");
			free(buffer_total);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -3;
		}


		//me posiciono al final del archivo
		retorno = fseek(f1, 0, SEEK_END);
		if(retorno!=0){
			log_error(worker_error_log, "No se pudo posicional al final del archivo temporal");
			free(buffer_total);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -4;
		}
		//determino longitud del archivo
		longitud_archivo_temporal = ftell(f1);
		//me posiciono al principio del archivo para poder leer
		rewind(f1);

		buffer = realloc(buffer, longitud_archivo_temporal);
		//leo archivo y lo pongo en el buffer
		leidos = fread(buffer, 1, longitud_archivo_temporal, f1);
		if(leidos != longitud_archivo_temporal){
			log_error(worker_error_log, "No se leyo correctamente el archivo temporal");
			free(buffer);
			free(buffer_total);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -5;
		}

		string_append(&buffer_total, buffer);

		fclose(f1);
	}

	free(buffer);

	//puntero que va a tener la cadena de caracteres que se le pasa a la funcion system para ejecutar el script
	char* s = string_from_format("printf \"%s\" | .\"/scripts/%s\" > \"%s\"", buffer_total,
										solicitudDeserializada->programa_reduccion, solicitudDeserializada->archivo_temporal_resultante);
	free(buffer_total);
	retorno = system(s);
	if(retorno == -1){
		log_error(worker_error_log, "No se pudo realizar la reduccion local");
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		free(s);
		return -10;
	}

	free(s);

	log_trace(worker_log, "Reduccion local finalizada");
	log_destroy(worker_log);
	log_destroy(worker_error_log);

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
		//enviar ERROR de apertura de archivo temporal
		break;
	case -4:
		//enviar ERROR de posicionamiento al final del archivo temporal
		break;
	case -5:
		//enviar ERROR de lectura de archivo temporal
		break;
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
