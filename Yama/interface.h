/*
 * interface.h
 *
 *  Created on: 21/11/2017
 *      Author: utnso
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "../bibliotecas/estructuras.h"
#include "../bibliotecas/protocolo.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


void serializarDato(char* buffer, void* dato, int size_to_send, int* offset);
void deserializarDato(void* dato, char* buffer, int size, int* offset);

char* serializar_workers(t_worker** workers, uint32_t item_cantidad);
char* serializar_t_worker(t_worker* worker);

uint32_t getLong_one_t_worker(t_worker* workers);
uint32_t getLong_workers(t_worker* workers, uint32_t cantidad);

t_worker* deserializar_t_worker(char* serialized);
t_worker* deserializar_t_workers(char* serialized, uint32_t items_cantidad);
solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char* serialized);

uint32_t getLong_one_archivos_temporal(archivo_temp* archivo_temp);
uint32_t getLong_archivos_temporales(archivo_temp* archivos_temporales, uint32_t cantidad_archivos_temp);
char* serializar_archivo_temporal(archivo_temp* archivos_temporal);
char* serializar_archivos_temporales(archivo_temp** archivos_temporales, uint32_t cantidad_archivos_temp);
archivo_temp* deserializar_archivo_temp(char* serialized);
archivo_temp* deserializar_archivos_temporales(char* serialized, uint32_t items_cantidad);

t_bloques_enviados* deserializarBloques(char* serialized, uint32_t* idMaster);
t_bloque_serializado* deserializar_bloques_serializados(char* serialized, uint32_t items_cantidad);
t_bloque_serializado* deserializar_bloque_serializado(char* serialized);

#endif /* INTERFACE_H_ */
