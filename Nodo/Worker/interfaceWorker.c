/*
 * interfaceWorker.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "interfaceWorker.h"

solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char* message){

	//TODO: hacer funcion real, esta mockeado
	solicitud_programa_transformacion* mock = malloc(sizeof(solicitud_programa_transformacion));
	strcpy(mock->programa_transformacion, "/scripts/transformador.sh");
	mock->bloque = 1;
	mock->bytes_ocupados = 50;
	strcpy(mock->archivo_temporal, "/tmp/Master1-temp1");
	return mock;
}

solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char* message){

	//TODO: hacer funcion real, esta mockeado
	archivo_temp* lista_de_temps = malloc(sizeof(archivo_temp));
	solicitud_programa_reduccion_local* mock = malloc(sizeof(solicitud_programa_reduccion_local));
	strcpy(mock->programa_reduccion, "/scripts/reductor.rb");
	mock->archivos_temporales = lista_de_temps;
	mock->cantidad_archivos_temp = 5;
	strcpy(mock->archivo_temporal_resultante, "/tmp/Master1-Worker1");
	return mock;
}

solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char* message){

	//TODO: hacer funcion real, esta mockeado
	item_programa_reduccion_global* lista_de_workers = malloc(sizeof(item_programa_reduccion_global)*2);
	strcpy(lista_de_workers[0].ip_worker, "192.168.0.15");
	lista_de_workers[0].puerto_worker = 9000;
	strcpy(lista_de_workers[0].archivo_temp_red_local, "Master1-Worker1");
	strcpy(lista_de_workers[1].ip_worker, "192.168.0.14");
	lista_de_workers[1].puerto_worker = 9001;
	strcpy(lista_de_workers[1].ip_worker, "Master1-Worker2");
	solicitud_programa_reduccion_global* mock = malloc(sizeof(solicitud_programa_reduccion_global));
	strcpy(mock->programa_reduccion, "/scripts/reductor.rb");
	mock->items_programa_reduccion_global = lista_de_workers;
	mock->cantidad_item_programa_reduccion = 2;
	strcpy(mock->archivo_temporal_resultante, "/tmp/Master1-final");
	return mock;
}

solicitud_enviar_archivo_temp* deserializarSolicitudEnviarArchivoTemp(char* message){
	solicitud_enviar_archivo_temp* mock = malloc(sizeof(solicitud_enviar_archivo_temp));
	return mock;
}

solicitud_leer_archivo_temp* deserializarSolicitudLeerArchivoTemp(char* message){
	solicitud_leer_archivo_temp* mock = malloc(sizeof(solicitud_leer_archivo_temp));
	return mock;
}
