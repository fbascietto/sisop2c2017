/*
 * interfaceWorker.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "interfaceWorker.h"

void serializarDato(char* buffer, void* dato, int size_to_send, int* offset){
	memcpy(buffer + *offset, dato, size_to_send);
	*offset += size_to_send;
}

void deserializarDato(void* dato, char* buffer, int size, int* offset){
	memcpy(dato,buffer + *offset,size);
	*offset += size;
}

uint32_t getLong_SolicitudProgramaTransformacion(solicitud_programa_transformacion* solicitud){
	uint32_t total_size = 0;

	total_size += sizeof(uint32_t)*2;//campo bloque y bytes_ocupados
	total_size += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	total_size += sizeof(char[LENGTH_RUTA_PROGRAMA]);
	total_size += sizeof(char)*solicitud->length_programa; //campo programa
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
	serializarDato(serializedPackage,&(solicitud->programa),sizeof(char)*solicitud->length_programa,&offset);
	serializarDato(serializedPackage,&(solicitud->length_programa),sizeof(uint32_t),&offset);

	return solicitud;
}

solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char* serialized){
	solicitud_programa_transformacion* solicitud = malloc(sizeof(solicitud_programa_transformacion));
	int offset = 0;
	deserializarDato(&(solicitud->programa_transformacion),serialized,sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	deserializarDato(&(solicitud->bloque),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->bytes_ocupados),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->archivo_temporal),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);
	deserializarDato(&(solicitud->length_programa),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->programa),serialized,sizeof(char)*solicitud->length_programa,&offset);

	return solicitud;
}

uint32_t getLong_one_archivos_temporal(archivo_temp* archivo_temp){
	uint32_t longitud = 0;
	longitud += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]); //ip_worker,
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

uint32_t getLong_SolicitudProgramaReduccionLocal(solicitud_programa_reduccion_local* solicitud){
	uint32_t total_size = 0;

	uint32_t size_items = getLong_archivos_temporales(solicitud->archivos_temporales,solicitud->cantidad_archivos_temp);
	total_size += sizeof(uint32_t)*2;//campo cantidad_archivos_temp y size_items
	total_size += sizeof(char[LENGTH_RUTA_PROGRAMA]);
	total_size += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	total_size += size_items;
	total_size += sizeof(char)*solicitud->length_programa; //campo programa
	total_size += sizeof(uint32_t);//campo length_programa

	return total_size;
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

char* serializarSolicitudProgramaReduccionLocal(solicitud_programa_reduccion_local* solicitud){
	uint32_t total_size = getLong_SolicitudProgramaReduccionLocal(solicitud);

	char *serializedPackage = malloc(total_size);

	int offset = 0;
	int size_to_send;

	serializarDato(serializedPackage,&(solicitud->programa_reduccion),sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	serializarDato(serializedPackage,&(solicitud->cantidad_archivos_temp),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(solicitud->archivo_temporal_resultante),sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);
	serializarDato(serializedPackage,&(solicitud->programa),sizeof(char)*solicitud->length_programa,&offset);
	serializarDato(serializedPackage,&(solicitud->length_programa),sizeof(uint32_t),&offset);

	//serializar items
	uint32_t size_items = getLong_archivos_temporales(solicitud->archivos_temporales,solicitud->cantidad_archivos_temp);
	serializarDato(serializedPackage,&(size_items),sizeof(uint32_t),&offset);

	char* serialized_items = serializar_archivos_temporales(&(solicitud->archivos_temporales),solicitud->cantidad_archivos_temp);
	serializarDato(serializedPackage,serialized_items,sizeof(char)*size_items,&offset);
	free(serialized_items);

	return serializedPackage;
}

archivo_temp* deserializar_archivo_temp(char* serialized){
	archivo_temp* archivoTemporal = malloc(sizeof(item_transformacion));
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
		free(aux);
		free(serialized_item);
	}
	return archivos_temporales;
}

solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char* serialized){
	solicitud_programa_reduccion_local* solicitud = malloc(sizeof(solicitud_programa_reduccion_local));
	int offset = 0;
	deserializarDato(&(solicitud->cantidad_archivos_temp),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->archivo_temporal_resultante),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);
	deserializarDato(&(solicitud->programa_reduccion),serialized,sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	deserializarDato(&(solicitud->length_programa),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->programa),serialized,sizeof(char)*solicitud->length_programa,&offset);

	uint32_t size_items;
	deserializarDato(&size_items,serialized,sizeof(uint32_t),&offset);

	char* serialized_items = malloc(sizeof(char)*size_items);
	deserializarDato(serialized_items,serialized,size_items,&offset);
	solicitud->archivos_temporales = deserializar_archivos_temporales(serialized_items,solicitud->cantidad_archivos_temp);
	free(serialized_items);

	return solicitud;
}

uint32_t getLong_one_item_reduccion_global(item_programa_reduccion_global* items_programa_reduccion_global){
	uint32_t longitud = 0;
	longitud += sizeof(char[LENGTH_IP]); //ip_worker,
	longitud += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	longitud += sizeof(uint32_t);
	return longitud;
}

uint32_t getLong_items_reduccion_global(item_programa_reduccion_global* items_programa_reduccion_global, uint32_t cantidad){
	uint32_t total = 0;
	int i;
	for(i=0; i<cantidad; i++){
		total += sizeof(uint32_t);
		total += getLong_one_item_reduccion_global(&(items_programa_reduccion_global[i]));
	}
	return total;
}

uint32_t getLong_SolicitudProgramaReduccionGlobal(solicitud_programa_reduccion_global* solicitud){
	uint32_t total_size = 0;

	uint32_t size_items = getLong_items_reduccion_global(solicitud->items_programa_reduccion_global,solicitud->cantidad_item_programa_reduccion);
	total_size += sizeof(uint32_t);//campo cantidad_item_programa_reduccion
	total_size += sizeof(char[LENGTH_RUTA_PROGRAMA]);
	total_size += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);
	total_size += size_items;
	total_size += sizeof(char)*solicitud->length_programa; //campo programa
	total_size += sizeof(uint32_t);//campo length_programa

	return total_size;
}

char* serializar_item_reduccion_global(item_programa_reduccion_global* item_reduccion_global){
	uint32_t total_size = getLong_one_item_reduccion_global(item_reduccion_global);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(item_reduccion_global->ip_worker),sizeof(char[LENGTH_IP]),&offset);
	serializarDato(serializedPackage,&(item_reduccion_global->puerto_worker),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_reduccion_global->archivo_temp_red_local),sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	return serializedPackage;
}

char* serializar_items_reduccion_global(item_programa_reduccion_global** items_reduccion_global, uint32_t cantidad_item){
	item_programa_reduccion_global* aux_items_reduccion_global = *items_reduccion_global;
	uint32_t total_size = getLong_items_reduccion_global(aux_items_reduccion_global, cantidad_item);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	int i;
	for (i = 0; i < cantidad_item; i++) {
		char* serialized_item = serializar_item_reduccion_global(&aux_items_reduccion_global[i]);//TODO: ver como pasarle el puntero como parametro
		uint32_t size_item = getLong_one_item_reduccion_global(&aux_items_reduccion_global[i]);
		serializarDato(serializedPackage,&(size_item),sizeof(uint32_t),&offset);
		serializarDato(serializedPackage,serialized_item,sizeof(char)*size_item,&offset);
		free(serialized_item);
	}
	return serializedPackage;
}

char* serializarSolicitudProgramaReduccionGlobal(solicitud_programa_reduccion_global* solicitud){
	uint32_t total_size = getLong_SolicitudProgramaReduccionGlobal(solicitud);

	char *serializedPackage = malloc(total_size);

	int offset = 0;
	int size_to_send;

	serializarDato(serializedPackage,&(solicitud->programa_reduccion),sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	serializarDato(serializedPackage,&(solicitud->cantidad_item_programa_reduccion),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(solicitud->archivo_temporal_resultante),sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);
	serializarDato(serializedPackage,&(solicitud->programa),sizeof(char)*solicitud->length_programa,&offset);
	serializarDato(serializedPackage,&(solicitud->length_programa),sizeof(uint32_t),&offset);

	//serializar items
	uint32_t size_items = getLong_items_reduccion_global(solicitud->items_programa_reduccion_global,solicitud->cantidad_item_programa_reduccion);
	serializarDato(serializedPackage,&(size_items),sizeof(uint32_t),&offset);

	char* serialized_items = serializar_items_reduccion_global(&(solicitud->items_programa_reduccion_global),solicitud->cantidad_item_programa_reduccion);
	serializarDato(serializedPackage,serialized_items,sizeof(char)*size_items,&offset);
	free(serialized_items);

	return serializedPackage;
}

item_programa_reduccion_global* deserializar_item_programa_reduccion_global(char* serialized){
	item_programa_reduccion_global* item = malloc(sizeof(item_programa_reduccion_global));
	int offset = 0;

	deserializarDato(&(item->ip_worker),serialized,sizeof(char[LENGTH_IP]),&offset);
	deserializarDato(&(item->puerto_worker),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(item->archivo_temp_red_local),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	return item;
}

item_programa_reduccion_global* deserializar_items_programa_reduccion_global(char* serialized, uint32_t items_cantidad){
	int offset = 0;

	item_programa_reduccion_global* items_programa_reduccion_global = malloc(sizeof(item_programa_reduccion_global)*items_cantidad);
	int i;
	for (i = 0; i < items_cantidad; i++) {
		uint32_t size_item;
		deserializarDato(&(size_item),serialized,sizeof(uint32_t),&offset);
		char* serialized_item = malloc(sizeof(char)*size_item);
		deserializarDato(serialized_item,serialized,size_item,&offset);
		item_programa_reduccion_global* aux = deserializar_item_programa_reduccion_global(serialized_item);
		items_programa_reduccion_global[i] = *(aux);
		free(aux);
		free(serialized_item);
	}
	return items_programa_reduccion_global;
}

solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char* serialized){
	solicitud_programa_reduccion_global* solicitud = malloc(sizeof(solicitud_programa_reduccion_global));
	int offset = 0;
	deserializarDato(&(solicitud->cantidad_item_programa_reduccion),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->archivo_temporal_resultante),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);
	deserializarDato(&(solicitud->programa_reduccion),serialized,sizeof(char[LENGTH_NOMBRE_PROGRAMA]),&offset);
	deserializarDato(&(solicitud->length_programa),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitud->programa),serialized,sizeof(char)*solicitud->length_programa,&offset);

	uint32_t size_items;
	deserializarDato(&size_items,serialized,sizeof(uint32_t),&offset);

	char* serialized_items = malloc(sizeof(char)*size_items);
	deserializarDato(serialized_items,serialized,size_items,&offset);
	solicitud->items_programa_reduccion_global = deserializar_items_programa_reduccion_global(serialized_items,solicitud->cantidad_item_programa_reduccion);
	free(serialized_items);

	return solicitud;
}

solicitud_enviar_archivo_temp* deserializarSolicitudEnviarArchivoTemp(char* message){
	solicitud_enviar_archivo_temp* mock = malloc(sizeof(solicitud_enviar_archivo_temp));
	return mock;
}

solicitud_leer_archivo_temp* deserializarSolicitudLeerArchivoTemp(char* message){
	solicitud_leer_archivo_temp* mock = malloc(sizeof(solicitud_leer_archivo_temp));
	return mock;
}
