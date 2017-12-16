/*
 * almacenamientoFinal.c
 *
 *  Created on: 5/11/2017
 *      Author: utnso
 */

#include "etapas.h"
#include "../../bibliotecas/protocolo.h"
#include "../../bibliotecas/sockets.h"
#include <commons/log.h>

int almacenamientoFinal(char* IP_fs, int puerto_fs, solicitud_realizar_almacenamiento_final* solicitudDeserializada){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);

	//conexion al fs
	int socketConn;
	socketConn = conectarseA(IP_fs, puerto_fs);
	log_trace(worker_log, "Conexion con filesystem establecida");

	//le aviso que soy el proceso Worker
	enviarInt(socketConn,PROCESO_WORKER);

	char* ruta_final = string_new();
	string_append_with_format(&ruta_final, solicitudDeserializada->ruta_archivo_final_fs);
	char* ruta_red_global = string_new();
	string_append_with_format(&ruta_red_global, "/tmp/archivoRG-%s", basename(solicitudDeserializada->ruta_archivo_temporal_resultante_reduccion_global));
	printf("La ruta del archivo final fs antes de enviar es: %s\n",ruta_final);
	printf("La ruta del archivo temporal resultante reduccion global antes de enviar es: %s\n",ruta_red_global);

	//le envio el archivo temporal de reduccion global resultante al fs
	enviarMensaje(socketConn, ruta_final);
	enviarMensaje(socketConn, ruta_red_global);

	log_trace(worker_log, "Comienzo de envio de archivo");
	int retorno = envioArchivo(socketConn, solicitudDeserializada->ruta_archivo_temporal_resultante_reduccion_global);
	if(retorno!=0){
		return retorno;
	}

	log_trace(worker_log, "Finalizacion de envio de archivo");

	log_destroy(worker_log);
	free(ruta_final);
	free(ruta_red_global);

	return 0;

}


void responderSolicitudAlmacenadoFinal(int socket, int exit_code){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	switch(exit_code){

	case 0:
		log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de almacenamiento final a Master");
		enviarInt(socket, ALMACENADO_FINAL_OK);
		break;
	case -1:
		log_error(worker_error_log, "Se envia error al abrir el archivo que se envia al Filesystem en la etapa de almacenamiento final a Master");
		enviarInt(socket, ALMACENADO_FINAL_ERROR_ABRIR_ARCHIVO);
		break;
	case -2:
		log_error(worker_error_log, "Se envia error al acceder al archivo que se envia al Filesystem en la etapa de almacenamiento final a Master");
		enviarInt(socket, ALMACENADO_FINAL_ERROR_ACCEDER_ARCHIVO);
		break;
	case -3:
		log_error(
				worker_error_log,
				"Se envia error al enviarle los datos preliminares al Filesytem sobre el archivo que se le envia en la etapa de almacenamiento final a Master"
				);
		enviarInt(socket, ALMACENADO_FINAL_ERROR_ENVIANDO_DATOS_PRELIMINARES);
		break;

	}

}
