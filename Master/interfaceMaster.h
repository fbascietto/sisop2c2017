/*
 * interface.h
 *
 *  Created on: 8/10/2017
 *      Author: utnso
 */

#ifndef INTERFACEMASTER_H_
#define INTERFACEMASTER_H_

#include "../bibliotecas/estructuras.h"
#include "interface.h"


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

item_transformacion* crearItemTransformacion(char nodo[NOMBRE_NODO],char* ipWorker,uint32_t puerto_worker, uint32_t bloque, uint32_t bytesOcupados, char* archivoTemporal);
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

item_reduccion_local* crearItemReduccionLocal(char nodo[NOMBRE_NODO],char* ipWorker,uint32_t puerto_worker, char* archivoTemporalReduccionLocal);
void agregarItemReduccionLocal(solicitud_reduccion_local* solicitudReduccionLocal, item_reduccion_local* item);

void testSerializarSolicitudReduccionLocal();
void testSerializarItemReduccionLocal();

/*SERIALIZACION DE SOLICUTUD REDUCCION GLOBAL*/
char* serializarSolicitudReduccionGlobal(solicitud_reduccion_global* solicitudReduccionGlobal);

uint32_t getLong_SolicitudReduccionGlobal(solicitud_reduccion_global* solicitudReduccionGlobal);

solicitud_reduccion_global* deserializar_solicitud_reduccion_global(char* serialized);

t_worker* crearItemWorker(char nodo[NOMBRE_NODO],char* ipWorker,uint32_t puerto_worker, char* archivoTemporalReduccionLocal);
void agregarItemWorker(solicitud_reduccion_global* solicitudReduccionGlobal, t_worker* item);

void testSerializarSolicitudReduccionGlobal();
void testSerializarWorker();

/*SERIALIZACION DE SOLICUTUD ALMACENADO FINAL*/
char* serializarSolicitudAlmacenadoFinal(solicitud_almacenado_final* solicitudAlmacenadoFinal);
uint32_t getLong_SolicitudAlmacenadoFinal(solicitud_almacenado_final* solicitudAlmacenadoFinal);
solicitud_almacenado_final* deserializar_solicitud_almacenado_final(char* serialized);
void testSerializarSolicitudAlmacenadoFinal();

#endif /* INTERFACEMASTER_H_ */
