/*
 * interfaceWorkerWorker.c
 *
 *  Created on: 23/11/2017
 *      Author: utnso
 */

#include "interfaceWorkerWorker.h"


solicitud_leer_y_enviar_archivo_temp* deserializarSolicitudLeerYEnviarArchivoTemp(char* message){
	solicitud_leer_y_enviar_archivo_temp* mock = malloc(sizeof(solicitud_leer_y_enviar_archivo_temp));
	return mock;
}

solicitud_recibir_palabra* deserializarSolicitudRecibirPalabra(char* message){
	solicitud_recibir_palabra* mock = malloc(sizeof(solicitud_recibir_palabra));
	return mock;
}
