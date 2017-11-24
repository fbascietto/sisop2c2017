/*
 * interfaceWorkerWorker.c
 *
 *  Created on: 23/11/2017
 *      Author: utnso
 */

#include "interfaceWorkerWorker.h"


uint32_t getLong_SolicitudRecibirPalabra(solicitud_recibir_palabra* solicitud){

	uint32_t total_size = 0;

	total_size += sizeof(bool);
	total_size += strlen(solicitud->palabra);

	return total_size;

}

char* serializarSolicitudRecibirPalabra(solicitud_recibir_palabra* solicitud){

	uint32_t total_size = getLong_SolicitudRecibirPalabra(solicitud);

	char* serializedPackage = malloc(total_size);

	int offset = 0;

	serializarDato(serializedPackage, &(solicitud->fin_de_archivo), sizeof(bool), &offset);
	int size_to_send = strlen(solicitud->palabra);
	memcpy(serializedPackage + offset, solicitud->palabra, size_to_send);


	return serializedPackage;

}

solicitud_leer_y_enviar_archivo_temp* deserializarSolicitudLeerYEnviarArchivoTemp(char* message){
	solicitud_leer_y_enviar_archivo_temp* mock = malloc(sizeof(solicitud_leer_y_enviar_archivo_temp));
	return mock;
}

solicitud_recibir_palabra* deserializarSolicitudRecibirPalabra(char* serialized){

	solicitud_recibir_palabra* solicitud = malloc(sizeof(solicitud_recibir_palabra));

	int offset = 0;

	deserializarDato(&(solicitud->fin_de_archivo), serialized, sizeof(bool), &offset);
	solicitud->palabra = strdup(serialized+offset);

	return solicitud;
}

