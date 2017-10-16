/*
 * interfaceWorker.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#ifndef INTERFACEWORKER_H_
#define INTERFACEWORKER_H_

#include <stdint.h>

#define LENGTH_RUTA_ARCHIVO_TEMP 50
#define LENGTH_RUTA_PROGRAMA 200

typedef struct {
	char programa_transformacion[LENGTH_RUTA_PROGRAMA];
	uint32_t bloque;  //bloque a aplicar programa de Transformacion
	uint32_t bytes_ocupados;
	char archivo_temporal[LENGTH_RUTA_ARCHIVO_TEMP]; //ruta de archivo temporal
} solicitud_programa_transformacion;

typedef struct {
	char archivo_temp[LENGTH_RUTA_ARCHIVO_TEMP];
} archivo_temp;

typedef struct {
	char programa_reduccion[LENGTH_RUTA_PROGRAMA];
	archivo_temp* archivos_temporales;  //lista de archivos temporales
	uint32_t cantidad_archivos_temp;  //cantidad de archivos temporales en dicha lista
	char archivo_temporal_resultante[LENGTH_RUTA_ARCHIVO_TEMP];
} solicitud_programa_reduccion_local;

typedef struct {
//definir este struct

} solicitud_programa_reduccion_global;

solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char*);
solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char*);
solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char*);


#endif /* INTERFACEWORKER_H_ */
