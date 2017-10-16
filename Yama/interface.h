/*
 * interface.h
 *
 *  Created on: 8/10/2017
 *      Author: utnso
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <stdint.h>
#include <stdbool.h>

/*ESTRUCTURA SOLICITUD TRANSFORMACION*/
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

/*ESTRUCTURA SOLICITUD REDUCCION LOCAL*/
typedef struct item_reduccion_local {
	uint32_t nodo_id;		//nodo
	char ip_puerto_worker[20];		//IP y Puerto del Worker
	char archivo_temporal_transformacion[50];		//Archivo temporal transformacion
	char archivo_temporal_reduccion_local[50];		//Archivo temporal reduccion local
} item_reduccion_local;

typedef struct solicitud_reduccion_local {
	item_reduccion_local* items_reduccion_local;		//array de item_reduccion_local
	uint32_t item_cantidad; //cantidad de items
} solicitud_reduccion_local;

/*ESTRUCTURA SOLICITUD REDUCCION GLOBAL*/
typedef struct item_reduccion_global {
	uint32_t nodo_id;		//nodo
	char ip_puerto_worker[20];		//IP y Puerto del Worker
	char archivo_temporal_reduccion_local[50];		//Archivo temporal reduccion local
	char archivo_temporal_reduccion_global[50];		//Archivo temporal reduccion global
	bool esEncargado;
} item_reduccion_global;

typedef struct solicitud_reduccion_global {
	item_reduccion_global* items_reduccion_global;		//array de item_reduccion_global
	uint32_t item_cantidad; //cantidad de items
} solicitud_reduccion_global;

/*ESTRUCTURA SOLICITUD ALMACENADO FINAL*/
typedef struct solicitud_almacenado_final {
	uint32_t nodo_id;		//nodo
	char ip_puerto_worker[20];		//IP y Puerto del Worker
	char archivo_temporal_reduccion_global[50];		//Archivo temporal reduccion global
} solicitud_almacenado_final;

void serializarDato(char* buffer, void* dato, int size_to_send, int* offset);
void deserializarDato(void* dato, char* buffer, int size, int* offset);

/*SERIALIZACION DE SOLICUTUD TRANSFORMACION*/
char* serializarSolicitudTransformacion(solicitud_transformacion* solicitudTransformacion);
char* serializar_items_transformacion(item_transformacion** items_transformacion, uint32_t item_cantidad);
char* serializar_item_transformacion(item_transformacion* item_transformacion);

uint32_t getLong_SolicitudTransformacion(solicitud_transformacion* solicitudTransformacion);
uint32_t getLong_items_transformacion(item_transformacion* items_transformacion, uint32_t item_cantidad);
uint32_t getLong_one_item_transformacion(item_transformacion* items_transformacion);

solicitud_transformacion* deserializar_solicitud_transformacion(char* serialized);
item_transformacion* deserializar_items_transformacion(char* serialized, uint32_t items_cantidad);
item_transformacion* deserializar_item_transformacion(char* serialized);

item_transformacion* crearItemTransformacion(uint32_t nodo,char* ipPuertoWorker, uint32_t bloque, uint32_t bytesOcupados, char* archivoTemporal);
void agregarItemTransformacion(solicitud_transformacion* solicitudTransformacion, item_transformacion* item);

void testSerializarSolicitudTrasnformacion();
void testSerializarItemTransformacion();

/*SERIALIZACION DE SOLICUTUD REDUCCION LOCAL*/
char* serializarSolicitudReduccionLocal(solicitud_reduccion_local* solicitudReduccionLocal);
char* serializar_items_reduccion_local(item_reduccion_local** items_reduccion_local, uint32_t item_cantidad);
char* serializar_item_reduccion_local(item_reduccion_local* item_reduccion_local);

uint32_t getLong_SolicitudReduccionLocal(solicitud_reduccion_local* solicitudReduccionLocal);
uint32_t getLong_items_reduccion_local(item_reduccion_local* items_reduccion_local, uint32_t item_cantidad);
uint32_t getLong_one_item_reduccion_local(item_reduccion_local* items_reduccion_local);

solicitud_reduccion_local* deserializar_solicitud_reduccion_local(char* serialized);
item_reduccion_local* deserializar_items_reduccion_local(char* serialized, uint32_t items_cantidad);
item_reduccion_local* deserializar_item_reduccion_local(char* serialized);

item_reduccion_local* crearItemReduccionLocal(uint32_t nodo,char* ipPuertoWorker, char* archivoTemporalTransformacion, char* archivoTemporalReduccionLocal);
void agregarItemReduccionLocal(solicitud_reduccion_local* solicitudReduccionLocal, item_reduccion_local* item);

void testSerializarSolicitudReduccionLocal();
void testSerializarItemReduccionLocal();

/*SERIALIZACION DE SOLICUTUD REDUCCION GLOBAL*/
char* serializarSolicitudReduccionGlobal(solicitud_reduccion_global* solicitudReduccionGlobal);
char* serializar_items_reduccion_global(item_reduccion_global** items_reduccion_global, uint32_t item_cantidad);
char* serializar_item_reduccion_global(item_reduccion_global* item_reduccion_global);

uint32_t getLong_SolicitudReduccionGlobal(solicitud_reduccion_global* solicitudReduccionGlobal);
uint32_t getLong_items_reduccion_global(item_reduccion_global* items_reduccion_global, uint32_t item_cantidad);
uint32_t getLong_one_item_reduccion_global(item_reduccion_global* items_reduccion_global);

solicitud_reduccion_global* deserializar_solicitud_reduccion_global(char* serialized);
item_reduccion_global* deserializar_items_reduccion_global(char* serialized, uint32_t items_cantidad);
item_reduccion_global* deserializar_item_reduccion_global(char* serialized);

item_reduccion_global* crearItemReduccionGlobal(uint32_t nodo,char* ipPuertoWorker, char* archivoTemporalReduccionLocal, char* archivoTemporalReduccionGlobal, bool esEncargado);
void agregarItemReduccionGlobal(solicitud_reduccion_global* solicitudReduccionGlobal, item_reduccion_global* item);

void testSerializarSolicitudReduccionGlobal();
void testSerializarItemReduccionGlobal();

/*SERIALIZACION DE SOLICUTUD ALMACENADO FINAL*/
char* serializarSolicitudAlmacenadoFinal(solicitud_almacenado_final* solicitudAlmacenadoFinal);
uint32_t getLong_SolicitudAlmacenadoFinal(solicitud_almacenado_final* solicitudAlmacenadoFinal);
solicitud_almacenado_final* deserializar_solicitud_almacenado_final(char* serialized);
void testSerializarSolicitudAlmacenadoFinal();

#endif /* INTERFACE_H_ */
