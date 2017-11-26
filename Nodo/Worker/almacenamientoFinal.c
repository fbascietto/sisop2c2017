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

void almacenamientoFinal(char* IP_fs, int puerto_fs, char* ruta_archivo_en_fs, char* archivo_a_enviar){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);

	//conexion al fs
	int socketConn;
	socketConn = conectarseA(IP_fs, puerto_fs);
	log_trace(worker_log, "Conexion con filesystem establecida");

	//le aviso que soy el proceso Worker
	enviarInt(socketConn,PROCESO_WORKER);

	//le envio el archivo temporal de reduccion global resultante al fs
	enviarMensajeSocketConLongitud(socketConn, RECIBIR_RUTA_ARCHIVO_FINAL, ruta_archivo_en_fs, strlen(ruta_archivo_en_fs));
	log_trace(worker_log, "Comienzo de envio de archivo");
	envioArchivo(socketConn, archivo_a_enviar);
	log_trace(worker_log, "Finalizacion de envio de archivo");

	log_destroy(worker_log);

}
