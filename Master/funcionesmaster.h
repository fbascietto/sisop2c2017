/*
 * funcionesmaster.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#ifndef MASTER_FUNCIONESMASTER_H_
#define MASTER_FUNCIONESMASTER_H_

#include <stdint.h>

typedef struct item_transformacion {
	uint32_t nodo_id;		//nodo
	char ip_puerto_worker[20];		//IP y Puerto del Worker
	uint32_t bloque;		//bloque a aplicar programa de Transformacion
	uint32_t bytes_ocupados;		//Bytes Ocupados
	char archivo_temporal[50];		//Archivo temporal
} item_transformacion;

typedef struct solicitud_transformacion {
	item_transformacion* items_transformacion;		//array de item_transformacion
	uint32_t item_cantidad; //cantidad de items
} solicitud_transformacion;

void serializarDato(char* buffer, void* dato, int size_to_send, int* offset);
void deserializarDato(void* dato, char* buffer, int size, int* offset);

char* serializarSolicitudTransformacion(solicitud_transformacion* solicitudTransformacion);
char* serializar_items_transformacion(item_transformacion** items_transformacion, uint32_t item_cantidad);
char* serializar_item_transformacion(item_transformacion* item_transformacion);

uint32_t getLong_SolicitudTransformacion(solicitud_transformacion* solicitudTransformacion);
uint32_t getLong_items_transformacion(item_transformacion* items_transformacion, uint32_t item_cantidad);
uint32_t getLong_one_item_transformacion(item_transformacion* items_transformacion);

solicitud_transformacion* deserializar_solicitud_transformacion(char* serialized);
item_transformacion* deserializar_items_transformacion(char* serialized, uint32_t items_cantidad);
item_transformacion* deserializar_item_transformacion(char* serialized);

void testSerializarItem();

#endif /* MASTER_FUNCIONESMASTER_H_ */
