/*
 * interfaceWorker.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "interfaceWorker.h"

solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char* message){

	solicitud_programa_transformacion* mock = malloc(sizeof(solicitud_programa_transformacion));
	strcpy(mock->programa_transformacion, "/scripts/transformador.sh");
	mock->bloque = 1;
	mock->bytes_ocupados = 0;
	strcpy(mock->archivo_temporal, "/tmp/Master1-temp1");
	return mock;
}

solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char* message){

	archivo_temp* lista_de_temps = malloc(sizeof(archivo_temp));
	solicitud_programa_reduccion_local* mock = malloc(sizeof(solicitud_programa_reduccion_local));
	strcpy(mock->programa_reduccion, "/scripts/reductor.rb");
	mock->archivos_temporales = lista_de_temps;
	mock->cantidad_archivos_temp = 5;
	strcpy(mock->archivo_temporal_resultante, "/tmp/Master1-Worker1");
	return mock;
}

solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char* message){

	solicitud_programa_reduccion_global* mock = malloc(sizeof(solicitud_programa_reduccion_global));
	return mock;
}
