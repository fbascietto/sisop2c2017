/*
 * interface.c
 *
 *  Created on: 21/11/2017
 *      Author: utnso
 */

#include "interface.h"

void serializarDato(char* buffer, void* dato, int size_to_send, int* offset){
	memcpy(buffer + *offset, dato, size_to_send);
	*offset += size_to_send;
}

void deserializarDato(void* dato, char* buffer, int size, int* offset){
	memcpy(dato,buffer + *offset,size);
	*offset += size;
}

uint32_t getLong_workers(t_worker* workers, uint32_t item_cantidad){
	uint32_t total = 0;
	int i;
	for(i=0; i<item_cantidad; i++){
		total += sizeof(uint32_t);
		total += getLong_one_t_worker(&(workers[i]));
	}
	return total;
}

uint32_t getLong_one_t_worker(t_worker* workers){
	uint32_t longitud = 0;
	longitud += sizeof(char[LENGTH_IP]); //ip_worker,
	longitud += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	longitud += sizeof(uint32_t);
	longitud += sizeof(NOMBRE_NODO);

	return longitud;
}

char* serializar_workers(t_worker** workers, uint32_t item_cantidad){
	t_worker* aux_workers = *workers;
	uint32_t total_size = getLong_workers(aux_workers, item_cantidad);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	int i;
	for (i = 0; i < item_cantidad; i++) {
		char* serialized_worker = serializar_t_worker(&aux_workers[i]);//TODO: ver como pasarle el puntero como parametro
		uint32_t size_worker = getLong_one_t_worker(&aux_workers[i]);
		serializarDato(serializedPackage,&(size_worker),sizeof(uint32_t),&offset);//size_item_transformacion
		serializarDato(serializedPackage,serialized_worker,sizeof(char)*size_worker,&offset);//item_transformacion
		free(serialized_worker);
	}
	return serializedPackage;
}

char* serializar_t_worker(t_worker* worker){
	uint32_t total_size = getLong_one_t_worker(worker);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(worker->nodo_id),sizeof(char[NOMBRE_NODO]),&offset);
	serializarDato(serializedPackage,&(worker->ip_worker),sizeof(char[LENGTH_IP]),&offset);
	serializarDato(serializedPackage,&(worker->puerto_worker),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(worker->archivo_temporal_reduccion_local),sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	return serializedPackage;
}

uint32_t getLong_one_archivos_temporal(archivo_temp* archivo_temp){
	uint32_t longitud = 0;
	longitud += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	return longitud;
}

uint32_t getLong_archivos_temporales(archivo_temp* archivos_temporales, uint32_t cantidad_archivos_temp){
	uint32_t total = 0;
	int i;
	for(i=0; i<cantidad_archivos_temp; i++){
		total += sizeof(uint32_t);
		total += getLong_one_archivos_temporal(&(archivos_temporales[i]));
	}
	return total;
}

t_worker* deserializar_t_worker(char* serialized){
	t_worker* item = malloc(sizeof(t_worker));
	int offset = 0;

	deserializarDato(&(item->nodo_id), serialized, sizeof(char[NOMBRE_NODO]), &offset);
	deserializarDato(&(item->ip_worker),serialized,sizeof(char[LENGTH_IP]),&offset);
	deserializarDato(&(item->puerto_worker),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(item->archivo_temporal_reduccion_local),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	return item;
}

t_worker* deserializar_t_workers(char* serialized, uint32_t items_cantidad){
	int offset = 0;

	t_worker* workers = malloc(sizeof(t_worker)*items_cantidad);
	int i;
	for (i = 0; i < items_cantidad; i++) {
		uint32_t size_item;
		deserializarDato(&(size_item),serialized,sizeof(uint32_t),&offset);
		char* serialized_item = malloc(sizeof(char)*size_item);
		deserializarDato(serialized_item,serialized,size_item,&offset);
		t_worker* aux = deserializar_t_worker(serialized_item);
		workers[i] = *(aux);
		free(aux);
		//free(serialized_item);
	}
	return workers;
}

char* serializar_archivo_temporal(archivo_temp* archivos_temporal){
	uint32_t total_size = getLong_one_archivos_temporal(archivos_temporal);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(archivos_temporal->archivo_temp),sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	return serializedPackage;
}

char* serializar_archivos_temporales(archivo_temp** archivos_temporales, uint32_t cantidad_archivos_temp){
	archivo_temp* aux_archivos_temporales = *archivos_temporales;
	uint32_t total_size = getLong_archivos_temporales(aux_archivos_temporales, cantidad_archivos_temp);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	int i;
	for (i = 0; i < cantidad_archivos_temp; i++) {
		char* serialized_archivo_temporal = serializar_archivo_temporal(&aux_archivos_temporales[i]);//TODO: ver como pasarle el puntero como parametro
		uint32_t size_archivo_temporal = getLong_one_archivos_temporal(&aux_archivos_temporales[i]);
		serializarDato(serializedPackage,&(size_archivo_temporal),sizeof(uint32_t),&offset);
		serializarDato(serializedPackage,serialized_archivo_temporal,sizeof(char)*size_archivo_temporal,&offset);
		free(serialized_archivo_temporal);
	}
	return serializedPackage;
}

archivo_temp* deserializar_archivo_temp(char* serialized){
	archivo_temp* archivoTemporal = malloc(sizeof(archivo_temp));
	int offset = 0;

	deserializarDato(&(archivoTemporal->archivo_temp),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	return archivoTemporal;
}

archivo_temp* deserializar_archivos_temporales(char* serialized, uint32_t items_cantidad){
	int offset = 0;

	archivo_temp* archivos_temporales = malloc(sizeof(archivo_temp)*items_cantidad);
	int i;
	for (i = 0; i < items_cantidad; i++) {
		uint32_t size_item;
		deserializarDato(&(size_item),serialized,sizeof(uint32_t),&offset);
		char* serialized_item = malloc(sizeof(char)*size_item);
		deserializarDato(serialized_item,serialized,size_item,&offset);
		archivo_temp* aux = deserializar_archivo_temp(serialized_item);
		archivos_temporales[i] = *(aux);
		//free(aux);
		//free(serialized_item);
	}
	return archivos_temporales;
}

