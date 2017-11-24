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

uint32_t getLong_SolicitudLeerYEnviarArchivoTemp(solicitud_leer_y_enviar_archivo_temp* solicitud){

	uint32_t total_size = 0;

	total_size += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);

	return total_size;

}

char* serializar_solicitud_leer_y_enviar_archivo_temp(solicitud_leer_y_enviar_archivo_temp* solicitud){

	uint32_t total_size = getLong_SolicitudLeerYEnviarArchivoTemp(solicitud);

	char* serializedPackage = malloc(total_size);

	int offset = 0;

	serializarDato(serializedPackage, &(solicitud->ruta_archivo_red_local_temp), sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]), &offset);

	return serializedPackage;

}

solicitud_leer_y_enviar_archivo_temp* deserializarSolicitudLeerYEnviarArchivoTemp(char* serialized){

	solicitud_leer_y_enviar_archivo_temp* solicitud = malloc(sizeof(solicitud_leer_y_enviar_archivo_temp));

	int offset = 0;

	deserializarDato(&(solicitud->ruta_archivo_red_local_temp), serialized, sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]), &offset);

	return solicitud;
}

solicitud_recibir_palabra* deserializarSolicitudRecibirPalabra(char* serialized){

	solicitud_recibir_palabra* solicitud = malloc(sizeof(solicitud_recibir_palabra));

	int offset = 0;

	deserializarDato(&(solicitud->fin_de_archivo), serialized, sizeof(bool), &offset);
	solicitud->palabra = strdup(serialized+offset);

	return solicitud;
}

