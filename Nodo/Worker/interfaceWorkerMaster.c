/*
 * interfaceWorker.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "interfaceWorkerMaster.h"

uint32_t getLong_SolicitudProgramaTransformacion(solicitud_programa_transformacion* solicitud){
	uint32_t total_size = 0;

	total_size += sizeof(uint32_t)*2;//campo bloque y bytes_ocupados
	total_size += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	total_size += sizeof(char[LENGTH_NOMBRE_PROGRAMA]);
	total_size += strlen(solicitud->programa); //campo programa
	total_size += sizeof(uint32_t);//campo length_programa

	return total_size;
}

char* serializarSolicitudProgramaTransformacion(solicitud_programa_transformacion* solicitud){
	uint32_t total_size = getLong_SolicitudProgramaTransformacion(solicitud);

	char *serializedPackage = malloc(total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(solicitud->programa_transformacion),sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	serializarDato(serializedPackage,&(solicitud->bloque),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(solicitud->bytes_ocupados),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(solicitud->archivo_temporal),sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);
	serializarDato(serializedPackage,&(solicitud->length_programa),sizeof(uint32_t),&offset);
	int size_to_send = strlen(solicitud->programa);
	memcpy(serializedPackage + offset, solicitud->programa, size_to_send);
	//serializarDato(serializedPackage,&(solicitud->programa),sizeof(char)*solicitud->length_programa,&offset);

	return serializedPackage;
}

solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char* serialized){
	solicitud_programa_transformacion* solicitud = malloc(sizeof(solicitud_programa_transformacion));
	int offset = 0;
	deserializarDato(&(solicitud->programa_transformacion),serialized,sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	deserializarDato(&(solicitud->bloque),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->bytes_ocupados),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->archivo_temporal),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);
	deserializarDato(&(solicitud->length_programa),serialized,sizeof(uint32_t),&offset);
	solicitud->programa = strdup(serialized+offset);


	return solicitud;
}

uint32_t getLong_SolicitudProgramaReduccionLocal(solicitud_programa_reduccion_local* solicitud){
	uint32_t total_size = 0;

	uint32_t size_items = getLong_archivos_temporales(solicitud->archivos_temporales,solicitud->cantidad_archivos_temp);
	total_size += sizeof(uint32_t)*2;//campo cantidad_archivos_temp y size_items
	total_size += sizeof(char[LENGTH_NOMBRE_PROGRAMA]);
	total_size += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	total_size += size_items;
	total_size += sizeof(char)*solicitud->length_programa; //campo programa
	total_size += sizeof(uint32_t);//campo length_programa

	return total_size;
}

char* serializarSolicitudProgramaReduccionLocal(solicitud_programa_reduccion_local* solicitud){
	uint32_t total_size = getLong_SolicitudProgramaReduccionLocal(solicitud);

	char *serializedPackage = malloc(total_size);

	int offset = 0;
	int size_to_send;

	serializarDato(serializedPackage,&(solicitud->programa_reduccion),sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	serializarDato(serializedPackage,&(solicitud->cantidad_archivos_temp),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(solicitud->archivo_temporal_resultante),sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	//serializar items
	uint32_t size_items = getLong_archivos_temporales(solicitud->archivos_temporales,solicitud->cantidad_archivos_temp);
	serializarDato(serializedPackage,&(size_items),sizeof(uint32_t),&offset);

	char* serialized_items = serializar_archivos_temporales(&(solicitud->archivos_temporales),solicitud->cantidad_archivos_temp);
	serializarDato(serializedPackage,serialized_items,sizeof(char)*size_items,&offset);
	free(serialized_items);

	serializarDato(serializedPackage,&(solicitud->length_programa),sizeof(uint32_t),&offset);
	size_to_send = strlen(solicitud->programa);
	memcpy(serializedPackage + offset, solicitud->programa, size_to_send);

	return serializedPackage;
}

solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char* serialized){
	solicitud_programa_reduccion_local* solicitud = malloc(sizeof(solicitud_programa_reduccion_local));
	int offset = 0;
	deserializarDato(&(solicitud->programa_reduccion),serialized,sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	deserializarDato(&(solicitud->cantidad_archivos_temp),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->archivo_temporal_resultante),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	uint32_t size_items;
	deserializarDato(&size_items,serialized,sizeof(uint32_t),&offset);

	char* serialized_items = malloc(sizeof(char)*size_items);
	deserializarDato(serialized_items,serialized,size_items,&offset);
	solicitud->archivos_temporales = deserializar_archivos_temporales(serialized_items,solicitud->cantidad_archivos_temp);
	free(serialized_items);

	deserializarDato(&(solicitud->length_programa),serialized,sizeof(uint32_t),&offset);
	solicitud->programa = strdup(serialized+offset);

	return solicitud;
}

uint32_t getLong_SolicitudProgramaReduccionGlobal(solicitud_programa_reduccion_global* solicitud){
	uint32_t total_size = 0;

	uint32_t size_items = getLong_workers(solicitud->workers,solicitud->cantidad_item_programa_reduccion);
	total_size += sizeof(uint32_t);//campo cantidad_item_programa_reduccion
	total_size += sizeof(char[LENGTH_NOMBRE_PROGRAMA]);
	total_size += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	total_size += size_items;
	total_size += sizeof(char)*solicitud->length_programa; //campo programa
	total_size += sizeof(uint32_t);//campo length_programa

	return total_size;
}

char* serializarSolicitudProgramaReduccionGlobal(solicitud_programa_reduccion_global* solicitud){
	uint32_t total_size = getLong_SolicitudProgramaReduccionGlobal(solicitud);

	char *serializedPackage = malloc(total_size);

	int offset = 0;
	int size_to_send;

	serializarDato(serializedPackage,&(solicitud->programa_reduccion),sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	serializarDato(serializedPackage,&(solicitud->cantidad_item_programa_reduccion),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(solicitud->archivo_temporal_resultante),sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	//serializar items
	uint32_t size_items = getLong_workers(solicitud->workers,solicitud->cantidad_item_programa_reduccion);
	serializarDato(serializedPackage,&(size_items),sizeof(uint32_t),&offset);

	char* serialized_items = serializar_workers(&(solicitud->workers),solicitud->cantidad_item_programa_reduccion);
	serializarDato(serializedPackage,serialized_items,sizeof(char)*size_items,&offset);
	free(serialized_items);

	serializarDato(serializedPackage,&(solicitud->length_programa),sizeof(uint32_t),&offset);
	size_to_send = strlen(solicitud->programa) + 1;
	memcpy(serializedPackage + offset, solicitud->programa, size_to_send);

	return serializedPackage;
}

solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char* serialized){
	solicitud_programa_reduccion_global* solicitud = malloc(sizeof(solicitud_programa_reduccion_global));
	int offset = 0;
	deserializarDato(&(solicitud->programa_reduccion),serialized,sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	deserializarDato(&(solicitud->cantidad_item_programa_reduccion),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->archivo_temporal_resultante),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	uint32_t size_items;
	deserializarDato(&size_items,serialized,sizeof(uint32_t),&offset);

	char* serialized_items = malloc(sizeof(char)*size_items);
	deserializarDato(serialized_items,serialized,size_items,&offset);
	solicitud->workers = deserializar_t_workers(serialized_items,solicitud->cantidad_item_programa_reduccion);
	free(serialized_items);

	deserializarDato(&(solicitud->length_programa),serialized,sizeof(uint32_t),&offset);
	solicitud->programa = strdup(serialized+offset);

	return solicitud;
}

