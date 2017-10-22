/*
 * interfaceWorker.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "../bibliotecas/estructuras.h"

#ifndef INTERFACEWORKER_H_
#define INTERFACEWORKER_H_

/*Funciones de serializacion para comunicacion Master con Worker*/

void serializarDato(char* buffer, void* dato, int size_to_send, int* offset);
void deserializarDato(void* dato, char* buffer, int size, int* offset);

uint32_t getLong_SolicitudProgramaTransformacion(solicitud_programa_transformacion* solicitud);
char* serializarSolicitudProgramaTransformacion(solicitud_programa_transformacion* solicitud);
solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char* serialized);

uint32_t getLong_one_archivos_temporal(archivo_temp* archivo_temp);
uint32_t getLong_archivos_temporales(archivo_temp* archivos_temporales, uint32_t cantidad_archivos_temp);
uint32_t getLong_SolicitudProgramaReduccionLocal(solicitud_programa_reduccion_local* solicitud);

char* serializar_archivo_temporal(archivo_temp* archivos_temporal);
char* serializar_archivos_temporales(archivo_temp** archivos_temporales, uint32_t cantidad_archivos_temp);
char* serializarSolicitudProgramaReduccionLocal(solicitud_programa_reduccion_local* solicitud);
archivo_temp* deserializar_archivo_temp(char* serialized);
archivo_temp* deserializar_archivos_temporales(char* serialized, uint32_t items_cantidad);
solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char* serialized);

uint32_t getLong_one_item_prog_reduccion_global(item_programa_reduccion_global* items_programa_reduccion_global);
uint32_t getLong_items_prog_reduccion_global(item_programa_reduccion_global* items_programa_reduccion_global, uint32_t cantidad);
uint32_t getLong_SolicitudProgramaReduccionGlobal(solicitud_programa_reduccion_global* solicitud);

char* serializar_item_prog_reduccion_global(item_programa_reduccion_global* item_reduccion_global);
char* serializar_items_prog_reduccion_global(item_programa_reduccion_global** items_reduccion_global, uint32_t cantidad_item);
char* serializarSolicitudProgramaReduccionGlobal(solicitud_programa_reduccion_global* solicitud);
item_programa_reduccion_global* deserializar_item_programa_reduccion_global(char* serialized);
solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char* serialized);

solicitud_enviar_archivo_temp* deserializarSolicitudEnviarArchivoTemp(char* message);
solicitud_leer_archivo_temp* deserializarSolicitudLeerArchivoTemp(char* message);

#endif /* INTERFACEWORKER_H_ */
