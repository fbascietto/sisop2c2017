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

void almacenamientoFinal(char* IP_fs, int puerto_fs, solicitud_realizar_almacenamiento_final* solicitudDeserializada){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* worker_log = log_create(ruta_archivo_log, "WORKER", 1, level);

	//conexion al fs
	int socketConn;
	socketConn = conectarseA(IP_fs, puerto_fs);
	log_trace(worker_log, "Conexion con filesystem establecida");

	//le aviso que soy el proceso Worker
	enviarInt(socketConn,PROCESO_WORKER);

	char* ruta_final = malloc(LENGTH_RUTA_ARCHIVO_TEMP);
	strcpy(ruta_final, solicitudDeserializada->ruta_archivo_final_fs);

	//le envio el archivo temporal de reduccion global resultante al fs
	enviarMensajeSocketConLongitud(socketConn, RECIBIR_RUTA_ARCHIVO_FINAL, ruta_final, strlen(ruta_final));
	log_trace(worker_log, "Comienzo de envio de archivo");
	envioArchivo(socketConn, solicitudDeserializada->ruta_archivo_temporal_resultante_reduccion_global);
	log_trace(worker_log, "Finalizacion de envio de archivo");

	log_destroy(worker_log);
	free(ruta_final);

}


void responderSolicitudAlmacenadoFinal(int socket){

	enviarMensajeSocketConLongitud(socket, ALMACENADO_FINAL_OK, NULL, 0);

}
