/*
 * interfaz.c
 *
 *  Created on: 8/10/2017
 *      Author: utnso
 */

#ifndef INTERFAZ_C_
#define INTERFAZ_C_

#include "interface.h"

void serializarDato(char* buffer, void* dato, int size_to_send, int* offset){
	memcpy(buffer + *offset, dato, size_to_send);
	*offset += size_to_send;
}

/*void deserializarDato(void* dato, char* buffer, int size, int* offset){
	memcpy(dato,buffer + *offset,size);
	*offset += size;
}*/

char* serializarSolicitudTransformacion(solicitud_transformacion* solicitudTransformacion){

	uint32_t total_size = getLong_SolicitudTransformacion(solicitudTransformacion);

	char *serializedPackage = malloc(total_size);

	int offset = 0;
	int size_to_send;

	serializarDato(serializedPackage,&(solicitudTransformacion->item_cantidad),sizeof(uint32_t),&offset);

	//serializar items
	uint32_t size_items = getLong_items_transformacion(solicitudTransformacion->items_transformacion,solicitudTransformacion->item_cantidad);
	serializarDato(serializedPackage,&(size_items),sizeof(uint32_t),&offset);

	char* serialized_items = serializar_items_transformacion(&(solicitudTransformacion->items_transformacion),solicitudTransformacion->item_cantidad);
	serializarDato(serializedPackage,serialized_items,sizeof(char)*size_items,&offset);
	free(serialized_items);

	return serializedPackage;
}

char* serializar_items_transformacion(item_transformacion** items_transformacion, uint32_t item_cantidad){
	item_transformacion* aux_items_transformacion = *items_transformacion;
	uint32_t total_size = getLong_items_transformacion(aux_items_transformacion, item_cantidad);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	int i;
	for (i = 0; i < item_cantidad; i++) {
		char* serialized_item_transformacion = serializar_item_transformacion(&aux_items_transformacion[i]);//TODO: ver como pasarle el puntero como parametro
		uint32_t size_item_transformacion = getLong_one_item_transformacion(&aux_items_transformacion[i]);
		serializarDato(serializedPackage,&(size_item_transformacion),sizeof(uint32_t),&offset);//size_item_transformacion
		serializarDato(serializedPackage,serialized_item_transformacion,sizeof(char)*size_item_transformacion,&offset);//item_transformacion
		free(serialized_item_transformacion);
	}
	return serializedPackage;
}

uint32_t getLong_SolicitudTransformacion(solicitud_transformacion* solicitudTransformacion){
	uint32_t total_size = 0;

	uint32_t size_items = getLong_items_transformacion(solicitudTransformacion->items_transformacion,solicitudTransformacion->item_cantidad);
	total_size += sizeof(uint32_t)*2;//campo item_cantidad y size_items
	total_size += size_items;

	return total_size;
}

uint32_t getLong_items_transformacion(item_transformacion* items_transformacion, uint32_t item_cantidad){
	uint32_t total = 0;
	int i;
	for(i=0; i<item_cantidad; i++){
		total += sizeof(uint32_t);
		total += getLong_one_item_transformacion(&(items_transformacion[i]));
	}
	return total;
}

uint32_t getLong_one_item_transformacion(item_transformacion* items_transformacion){
	uint32_t longitud = 0;
	longitud += sizeof(uint32_t)*4; //nodo_id, bloque, bytes_ocupados,puerto_worker
	longitud += sizeof(char[20]); //ip_worker,
	longitud += sizeof(char[50]);//archivo_temporal
	return longitud;
}

char* serializar_item_transformacion(item_transformacion* item_transformacion){
	uint32_t total_size = getLong_one_item_transformacion(item_transformacion);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(item_transformacion->nodo_id),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->ip_worker),sizeof(char[20]),&offset);
	serializarDato(serializedPackage,&(item_transformacion->puerto_worker),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->bloque),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->bytes_ocupados),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->archivo_temporal),sizeof(char[50]),&offset);

	return serializedPackage;
}

solicitud_transformacion* deserializar_solicitud_transformacion(char* serialized){
	solicitud_transformacion* solicitudTransformacion = malloc(sizeof(solicitud_transformacion));
	int offset = 0;
	deserializarDato(&(solicitudTransformacion->item_cantidad),serialized,sizeof(uint32_t),&offset);

	uint32_t size_items;
	deserializarDato(&size_items,serialized,sizeof(uint32_t),&offset);

	char* serialized_items = malloc(sizeof(char)*size_items);
	deserializarDato(serialized_items,serialized,size_items,&offset);
	solicitudTransformacion->items_transformacion = deserializar_items_transformacion(serialized_items,solicitudTransformacion->item_cantidad);
	free(serialized_items);

	return solicitudTransformacion;
}

item_transformacion* deserializar_items_transformacion(char* serialized, uint32_t items_cantidad){
	int offset = 0;

	//item_transformacion* item_transformacion = NULL;
	item_transformacion* itemsTransformacion = malloc(sizeof(item_transformacion)*items_cantidad);
	int i;
	for (i = 0; i < items_cantidad; i++) {
		uint32_t size_item;
		deserializarDato(&(size_item),serialized,sizeof(uint32_t),&offset);
		char* serialized_item = malloc(sizeof(char)*size_item);
		deserializarDato(serialized_item,serialized,size_item,&offset);
		item_transformacion* aux = deserializar_item_transformacion(serialized_item);
		itemsTransformacion[i] = *(aux);
		free(aux);
		free(serialized_item);
	}
	return itemsTransformacion;
}

item_transformacion* deserializar_item_transformacion(char* serialized){
	item_transformacion* itemTransformacion = malloc(sizeof(item_transformacion));
	int offset = 0;

	deserializarDato(&(itemTransformacion->nodo_id),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemTransformacion->ip_worker),serialized,sizeof(char[20]),&offset);
	deserializarDato(&(itemTransformacion->puerto_worker),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemTransformacion->bloque),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemTransformacion->bytes_ocupados),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemTransformacion->archivo_temporal),serialized,sizeof(char[50]),&offset);

	return itemTransformacion;
}

item_transformacion* crearItemTransformacion(uint32_t nodo,char* ipWorker,uint32_t puerto_worker, uint32_t bloque, uint32_t bytesOcupados, char* archivoTemporal){
	item_transformacion *item = malloc(sizeof(item_transformacion));
	strcpy(item->archivo_temporal,archivoTemporal);
	item->bloque = bloque;
	item->bytes_ocupados = bytesOcupados;
	item->nodo_id = nodo;
	strcpy(item->ip_worker,ipWorker);
	item->puerto_worker = puerto_worker;
	return item;
}

void testSerializarSolicitudTrasnformacion(){

	item_transformacion* item1 = crearItemTransformacion(1,"127.0.0.1",8080,2222,12345,"/temp1/archivo1.txt");
	item_transformacion* item2 = crearItemTransformacion(12,"127.23.0.1",0101,523,5777666,"/temsssssp1211/otro.txt");
	item_transformacion* item3 = crearItemTransformacion(137,"187.0.56.1",9090,62,643,"/temp655/tercero.txt");

	solicitud_transformacion* solicitudTransformacion = malloc(sizeof(solicitud_transformacion));

	solicitudTransformacion->item_cantidad = 0;

	agregarItemTransformacion(solicitudTransformacion,item1);
	free(item1);
	agregarItemTransformacion(solicitudTransformacion,item2);
	free(item2);
	agregarItemTransformacion(solicitudTransformacion,item3);
	free(item3);

	char* solicitudTransfSerializada = serializarSolicitudTransformacion(solicitudTransformacion);

	solicitud_transformacion* solicitudTransfDeserializada = deserializar_solicitud_transformacion(solicitudTransfSerializada);

	printf("cantidad de items = %d\n", solicitudTransfDeserializada->item_cantidad );
	int var2;
	for (var2 = 0; var2 < solicitudTransfDeserializada->item_cantidad; ++var2) {
		printf("\nNUEVO ITEM DESERIALIZADO//////////////////////////////////////////////\n");
		printf("archivo_temporal = %s\n", solicitudTransfDeserializada->items_transformacion[var2].archivo_temporal );
		printf("bloque = %d\n", solicitudTransfDeserializada->items_transformacion[var2].bloque );
		printf("bytes_ocupados = %d\n", solicitudTransfDeserializada->items_transformacion[var2].bytes_ocupados );
		printf("nodo_id = %d\n", solicitudTransfDeserializada->items_transformacion[var2].nodo_id );
		printf("ip_worker = %s\n", solicitudTransfDeserializada->items_transformacion[var2].ip_worker );
		printf("puerto_worker = %d\n", solicitudTransfDeserializada->items_transformacion[var2].puerto_worker );
	}
}

void agregarItemTransformacion(solicitud_transformacion* solicitudTransformacion, item_transformacion* item){
	solicitudTransformacion->items_transformacion = realloc(solicitudTransformacion->items_transformacion,sizeof(item_transformacion)*(solicitudTransformacion->item_cantidad+1));
	strcpy(solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].archivo_temporal,item->archivo_temporal);
	solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].bloque = item->bloque;
	solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].bytes_ocupados = item->bytes_ocupados;
	solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].nodo_id = item->nodo_id;
	solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].puerto_worker = item->puerto_worker;
	strcpy(solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].ip_worker,item->ip_worker);
	solicitudTransformacion->item_cantidad++;
}

void testSerializarItemTransformacion(){

	item_transformacion *item1 = crearItemTransformacion(1,"127.0.0.1",8080,1,12345,"/temp1/archivo1.txt");

	char* itemSerializado = serializar_item_transformacion(item1);

	item_transformacion *itemDeserializado = deserializar_item_transformacion(itemSerializado);
	printf("archivo_temporal = %s\n", itemDeserializado->archivo_temporal );
	printf("bloque = %d\n", itemDeserializado->bloque );
	printf("bytes_ocupados = %d\n", itemDeserializado->bytes_ocupados );
	printf("nodo_id = %d\n", itemDeserializado->nodo_id );
	printf("ip_worker = %s\n", itemDeserializado->ip_worker );
	printf("puerto_worker = %d\n", itemDeserializado->puerto_worker );
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

char* serializarSolicitudReduccionLocal(solicitud_reduccion_local* solicitudReduccionLocal){

	uint32_t total_size = getLong_SolicitudReduccionLocal(solicitudReduccionLocal);

	char *serializedPackage = malloc(total_size);

	int offset = 0;
	int size_to_send;

	serializarDato(serializedPackage,&(solicitudReduccionLocal->item_cantidad),sizeof(uint32_t),&offset);

	//serializar items
	uint32_t size_items = getLong_items_reduccion_local(solicitudReduccionLocal->items_reduccion_local,solicitudReduccionLocal->item_cantidad);
	serializarDato(serializedPackage,&(size_items),sizeof(uint32_t),&offset);

	char* serialized_items = serializar_items_reduccion_local(&(solicitudReduccionLocal->items_reduccion_local),solicitudReduccionLocal->item_cantidad);
	serializarDato(serializedPackage,serialized_items,sizeof(char)*size_items,&offset);
	free(serialized_items);

	return serializedPackage;
}

char* serializar_items_reduccion_local(item_reduccion_local** items_reduccion_local, uint32_t item_cantidad){
	item_reduccion_local* aux_items_reduccion_local = *items_reduccion_local;
	uint32_t total_size = getLong_items_reduccion_local(aux_items_reduccion_local, item_cantidad);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	int i;
	for (i = 0; i < item_cantidad; i++) {
		char* serialized_item_reduccion_local = serializar_item_reduccion_local(&aux_items_reduccion_local[i]);//TODO: ver como pasarle el puntero como parametro
		uint32_t size_item_reduccion_local = getLong_one_item_reduccion_local(&aux_items_reduccion_local[i]);
		serializarDato(serializedPackage,&(size_item_reduccion_local),sizeof(uint32_t),&offset);//size_item_transformacion
		serializarDato(serializedPackage,serialized_item_reduccion_local,sizeof(char)*size_item_reduccion_local,&offset);//item_transformacion
		free(serialized_item_reduccion_local);
	}
	return serializedPackage;
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

char* serializar_item_reduccion_local(item_reduccion_local* item_reduccion_local){
	uint32_t total_size = getLong_one_item_reduccion_local(item_reduccion_local);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(item_reduccion_local->nodo_id),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_reduccion_local->ip_worker),sizeof(char[20]),&offset);
	serializarDato(serializedPackage,&(item_reduccion_local->puerto_worker),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_reduccion_local->archivo_temporal_reduccion_local),sizeof(char[50]),&offset);

	serializarDato(serializedPackage,&(item_reduccion_local->cantidad_archivos_temp),sizeof(uint32_t),&offset);

	uint32_t size_items = getLong_archivos_temporales(item_reduccion_local->archivos_temporales_transformacion,item_reduccion_local->cantidad_archivos_temp);
	serializarDato(serializedPackage,&(size_items),sizeof(uint32_t),&offset);

	char* serialized_items = serializar_archivos_temporales(&(item_reduccion_local->archivos_temporales_transformacion),item_reduccion_local->cantidad_archivos_temp);
	serializarDato(serializedPackage,serialized_items,sizeof(char)*size_items,&offset);
	free(serialized_items);

	return serializedPackage;
}

uint32_t getLong_SolicitudReduccionLocal(solicitud_reduccion_local* solicitudReduccionLocal){
	uint32_t total_size = 0;

	uint32_t size_items = getLong_items_reduccion_local(solicitudReduccionLocal->items_reduccion_local,solicitudReduccionLocal->item_cantidad);
	total_size += sizeof(uint32_t)*2;//campo item_cantidad y size_items
	total_size += size_items;

	return total_size;
}

uint32_t getLong_items_reduccion_local(item_reduccion_local* items_reduccion_local, uint32_t item_cantidad){
	uint32_t total = 0;
	int i;
	for(i=0; i<item_cantidad; i++){
		total += sizeof(uint32_t);
		total += getLong_one_item_reduccion_local(&(items_reduccion_local[i]));
	}
	return total;
}

uint32_t getLong_one_item_reduccion_local(item_reduccion_local* items_reduccion_local){
	uint32_t longitud = 0;
	uint32_t size_items = getLong_archivos_temporales(items_reduccion_local->archivos_temporales_transformacion,items_reduccion_local->cantidad_archivos_temp);
	longitud += sizeof(uint32_t)*2;//campo item_cantidad y size_items
	longitud += size_items;
	longitud += sizeof(uint32_t)*2; //nodo_id, puerto_worker
	longitud += sizeof(char[20]); //ip_worker,
	longitud += sizeof(char[50]);//archivo_temporal reduccion_local
	return longitud;
}

solicitud_reduccion_local* deserializar_solicitud_reduccion_local(char* serialized){
	solicitud_reduccion_local* solicitudReduccionLocal = malloc(sizeof(solicitud_reduccion_local));
	int offset = 0;
	deserializarDato(&(solicitudReduccionLocal->item_cantidad),serialized,sizeof(uint32_t),&offset);

	uint32_t size_items;
	deserializarDato(&size_items,serialized,sizeof(uint32_t),&offset);

	char* serialized_items = malloc(sizeof(char)*size_items);
	deserializarDato(serialized_items,serialized,size_items,&offset);
	solicitudReduccionLocal->items_reduccion_local = deserializar_items_reduccion_local(serialized_items,solicitudReduccionLocal->item_cantidad);
	free(serialized_items);

	return solicitudReduccionLocal;
}

item_reduccion_local* deserializar_items_reduccion_local(char* serialized, uint32_t items_cantidad){
	int offset = 0;

	//item_reduccion_local* item_reduccion_local = NULL;
	item_reduccion_local* itemsReduccionLocal = malloc(sizeof(item_reduccion_local)*items_cantidad);
	int i;
	for (i = 0; i < items_cantidad; i++) {
		uint32_t size_item;
		deserializarDato(&(size_item),serialized,sizeof(uint32_t),&offset);
		char* serialized_item = malloc(sizeof(char)*size_item);
		deserializarDato(serialized_item,serialized,size_item,&offset);
		item_reduccion_local* aux = deserializar_item_reduccion_local(serialized_item);
		itemsReduccionLocal[i] = *(aux);
		free(aux);
		free(serialized_item);
	}
	return itemsReduccionLocal;
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
		free(aux);
		free(serialized_item);
	}
	return archivos_temporales;
}

item_reduccion_local* deserializar_item_reduccion_local(char* serialized){
	item_reduccion_local* itemReduccionLocal = malloc(sizeof(item_reduccion_local));
	int offset = 0;

	deserializarDato(&(itemReduccionLocal->nodo_id),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemReduccionLocal->ip_worker),serialized,sizeof(char[20]),&offset);
	deserializarDato(&(itemReduccionLocal->puerto_worker),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemReduccionLocal->archivo_temporal_reduccion_local),serialized,sizeof(char[50]),&offset);
	deserializarDato(&(itemReduccionLocal->cantidad_archivos_temp),serialized,sizeof(uint32_t),&offset);

	uint32_t size_items;
	deserializarDato(&size_items,serialized,sizeof(uint32_t),&offset);

	char* serialized_items = malloc(sizeof(char)*size_items);
	deserializarDato(serialized_items,serialized,size_items,&offset);
	itemReduccionLocal->archivos_temporales_transformacion = deserializar_archivos_temporales(serialized_items,itemReduccionLocal->cantidad_archivos_temp);
	free(serialized_items);

	return itemReduccionLocal;
}


item_reduccion_local* crearItemReduccionLocal(uint32_t nodo,char* ipWorker,uint32_t puerto_worker, char* archivoTemporalReduccionLocal){
	item_reduccion_local *item = malloc(sizeof(item_reduccion_local));
	item->cantidad_archivos_temp = 0;
	strcpy(item->archivo_temporal_reduccion_local,archivoTemporalReduccionLocal);
	item->nodo_id = nodo;
	item->puerto_worker = puerto_worker;
	strcpy(item->ip_worker,ipWorker);
	return item;
}

void agregarArchivoTransformacion(item_reduccion_local* item, archivo_temp* archivo_temp){
	item->archivos_temporales_transformacion = realloc(item->archivos_temporales_transformacion,sizeof(archivo_temp)*(item->cantidad_archivos_temp+1));
	item->cantidad_archivos_temp++;
}

void agregarItemReduccionLocal(solicitud_reduccion_local* solicitudReduccionLocal, item_reduccion_local* item){
	solicitudReduccionLocal->items_reduccion_local = realloc(solicitudReduccionLocal->items_reduccion_local,sizeof(item_reduccion_local)*(solicitudReduccionLocal->item_cantidad+1));
	solicitudReduccionLocal->items_reduccion_local[solicitudReduccionLocal->item_cantidad].archivos_temporales_transformacion=(item->archivos_temporales_transformacion);
	strcpy(solicitudReduccionLocal->items_reduccion_local[solicitudReduccionLocal->item_cantidad].archivo_temporal_reduccion_local,item->archivo_temporal_reduccion_local);
	solicitudReduccionLocal->items_reduccion_local[solicitudReduccionLocal->item_cantidad].nodo_id = item->nodo_id;
	solicitudReduccionLocal->items_reduccion_local[solicitudReduccionLocal->item_cantidad].cantidad_archivos_temp = item->cantidad_archivos_temp;
	solicitudReduccionLocal->items_reduccion_local[solicitudReduccionLocal->item_cantidad].puerto_worker = item->puerto_worker;
	strcpy(solicitudReduccionLocal->items_reduccion_local[solicitudReduccionLocal->item_cantidad].ip_worker,item->ip_worker);
	solicitudReduccionLocal->item_cantidad++;
}

archivo_temp* crearArchivoTemporal(char* archivo){
	archivo_temp *arch_temp1 = malloc(sizeof(archivo_temp));
	strcpy(arch_temp1->archivo_temp,archivo);
	return arch_temp1;
}

void agregarArchivoTemporalTransf(item_reduccion_local* item_reduccion_local, archivo_temp* archivoTemp){
	item_reduccion_local->archivos_temporales_transformacion = realloc(item_reduccion_local->archivos_temporales_transformacion,sizeof(archivo_temp)*(item_reduccion_local->cantidad_archivos_temp+1));
	strcpy(item_reduccion_local->archivos_temporales_transformacion[item_reduccion_local->cantidad_archivos_temp].archivo_temp,archivoTemp->archivo_temp);
	item_reduccion_local->cantidad_archivos_temp++;
}

void testSerializarSolicitudReduccionLocal(){

	item_reduccion_local* item1 = crearItemReduccionLocal(1,"127.0.0.1",8080,"/tmp/Master1-temp38");
	item_reduccion_local* item2 = crearItemReduccionLocal(12,"127.23.0.1",0101,"/tmp/Master1-Worker1");
	item_reduccion_local* item3 = crearItemReduccionLocal(137,"187.0.56.1",9090,"/tmp/Master1-Worker2");

	solicitud_reduccion_local* solicitudReduccionLocal = malloc(sizeof(solicitud_reduccion_local));

	solicitudReduccionLocal->item_cantidad = 0;

	archivo_temp *arch_temp1 = crearArchivoTemporal("/tmp/Master1-temp38");
	archivo_temp *arch_temp2 = crearArchivoTemporal("/tmp/Master1-temp39");
	archivo_temp *arch_temp3 = crearArchivoTemporal("/tmp/Master1-temp40");
	archivo_temp *arch_temp4 = crearArchivoTemporal("/tmp/Master1-temp41");
	archivo_temp *arch_temp5 = crearArchivoTemporal("/tmp/Master1-temp42");
	archivo_temp *arch_temp6 = crearArchivoTemporal("/tmp/Master1-temp43");
	archivo_temp *arch_temp7 = crearArchivoTemporal("/tmp/Master1-temp44");
	archivo_temp *arch_temp8 = crearArchivoTemporal("/tmp/Master1-temp45");
	archivo_temp *arch_temp9 = crearArchivoTemporal("/tmp/Master1-temp46");

	agregarArchivoTemporalTransf(item1, arch_temp1);
	free(arch_temp1);
	agregarArchivoTemporalTransf(item1, arch_temp2);
	free(arch_temp2);
	agregarArchivoTemporalTransf(item1, arch_temp3);
	free(arch_temp3);
	agregarArchivoTemporalTransf(item2, arch_temp4);
	free(arch_temp4);
	agregarArchivoTemporalTransf(item2, arch_temp5);
	free(arch_temp5);
	agregarArchivoTemporalTransf(item2, arch_temp6);
	free(arch_temp6);
	agregarArchivoTemporalTransf(item3, arch_temp7);
	free(arch_temp7);
	agregarArchivoTemporalTransf(item3, arch_temp8);
	free(arch_temp8);
	agregarArchivoTemporalTransf(item3, arch_temp9);
	free(arch_temp9);
	agregarItemReduccionLocal(solicitudReduccionLocal,item1);
	free(item1);
	agregarItemReduccionLocal(solicitudReduccionLocal,item2);
	free(item2);
	agregarItemReduccionLocal(solicitudReduccionLocal,item3);
	free(item3);

	char* solicitudTransfSerializada = serializarSolicitudReduccionLocal(solicitudReduccionLocal);

	solicitud_reduccion_local* solicitudRedLocalDeserializada = deserializar_solicitud_reduccion_local(solicitudTransfSerializada);

	printf("cantidad de items = %d\n", solicitudRedLocalDeserializada->item_cantidad );
	int var2;
	for (var2 = 0; var2 < solicitudRedLocalDeserializada->item_cantidad; ++var2) {
		printf("\nNUEVO ITEM DESERIALIZADO//////////////////////////////////////////////\n");
		printf("archivo_temporal_reduccion_local = %s\n", solicitudRedLocalDeserializada->items_reduccion_local[var2].archivo_temporal_reduccion_local );
		printf("nodo_id = %d\n", solicitudRedLocalDeserializada->items_reduccion_local[var2].nodo_id );
		printf("ip_worker = %s\n", solicitudRedLocalDeserializada->items_reduccion_local[var2].ip_worker );
		printf("puerto_worker = %d\n", solicitudRedLocalDeserializada->items_reduccion_local[var2].puerto_worker );
		printf("cantidad de archivos temporales = %d\n", solicitudRedLocalDeserializada->items_reduccion_local[var2].cantidad_archivos_temp );
		int var3;
		for (var3 = 0; var3 < solicitudRedLocalDeserializada->items_reduccion_local[var2].cantidad_archivos_temp; ++var3) {
			printf("\nNUEVO ARCHIVO TEMPORAL Transformacion//////////////////////////////////////////////\n");
			printf("archivotemp = %s\n", solicitudRedLocalDeserializada->items_reduccion_local[var2].archivos_temporales_transformacion[var3].archivo_temp );
		}
	}
}

void testSerializarItemReduccionLocal(){

	item_reduccion_local* item1 = crearItemReduccionLocal(1,"127.0.0.1",8080,"/tmp/Master1-temp38");
	archivo_temp *arch_temp1 = crearArchivoTemporal("/tmp/Master1-temp38");
	archivo_temp *arch_temp2 = crearArchivoTemporal("/tmp/Master1-temp39");
	archivo_temp *arch_temp3 = crearArchivoTemporal("/tmp/Master1-temp40");

	agregarArchivoTemporalTransf(item1, arch_temp1);
	free(arch_temp1);
	agregarArchivoTemporalTransf(item1, arch_temp2);
	free(arch_temp2);
	agregarArchivoTemporalTransf(item1, arch_temp3);
	free(arch_temp3);

	char* itemSerializado = serializar_item_reduccion_local(item1);

	item_reduccion_local *itemDeserializado = deserializar_item_reduccion_local(itemSerializado);
	printf("archivo_temporal_reduccion_local = %s\n", itemDeserializado->archivo_temporal_reduccion_local );
	printf("nodo_id = %d\n", itemDeserializado->nodo_id );
	printf("ip_worker = %s\n", itemDeserializado->ip_worker );
	printf("puerto_worker = %d\n", itemDeserializado->puerto_worker );
	int var3;
	for (var3 = 0; var3 < itemDeserializado->cantidad_archivos_temp; ++var3) {
		printf("\nNUEVO ARCHIVO TEMPORAL Transformacion//////////////////////////////////////////////\n");
		printf("archivotemp = %s\n", itemDeserializado->archivos_temporales_transformacion[var3].archivo_temp );
	}
}

char* serializarSolicitudReduccionGlobal(solicitud_reduccion_global* solicitudReduccionGlobal){

	uint32_t total_size = getLong_SolicitudReduccionGlobal(solicitudReduccionGlobal);

	char *serializedPackage = malloc(total_size);

	int offset = 0;
	int size_to_send;

	serializarDato(serializedPackage,&(solicitudReduccionGlobal->item_cantidad),sizeof(uint32_t),&offset);

	serializarDato(serializedPackage,&(solicitudReduccionGlobal->archivo_temporal_reduccion_global),sizeof(char[50]),&offset);

	uint32_t size_item_encargado = getLong_one_item_reduccion_global(solicitudReduccionGlobal->encargado_reduccion_global);
	serializarDato(serializedPackage,&(size_item_encargado),sizeof(uint32_t),&offset);

	char* serialized_encargado = serializar_t_worker(solicitudReduccionGlobal->encargado_reduccion_global);
	serializarDato(serializedPackage,serialized_encargado,sizeof(char)*size_item_encargado,&offset);
	free(serialized_encargado);

	//serializar items
	uint32_t size_items = getLong_items_reduccion_global(solicitudReduccionGlobal->items_reduccion_global,solicitudReduccionGlobal->item_cantidad);
	serializarDato(serializedPackage,&(size_items),sizeof(uint32_t),&offset);

	char* serialized_items = serializar_workers(&(solicitudReduccionGlobal->items_reduccion_global),solicitudReduccionGlobal->item_cantidad);
	serializarDato(serializedPackage,serialized_items,sizeof(char)*size_items,&offset);
	free(serialized_items);

	return serializedPackage;
}

char* serializar_workers(item_reduccion_global** items_reduccion_global, uint32_t item_cantidad){
	item_reduccion_global* aux_items_reduccion_global = *items_reduccion_global;
	uint32_t total_size = getLong_items_reduccion_global(aux_items_reduccion_global, item_cantidad);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	int i;
	for (i = 0; i < item_cantidad; i++) {
		char* serialized_item_reduccion_global = serializar_t_worker(&aux_items_reduccion_global[i]);//TODO: ver como pasarle el puntero como parametro
		uint32_t size_item_reduccion_global = getLong_one_item_reduccion_global(&aux_items_reduccion_global[i]);
		serializarDato(serializedPackage,&(size_item_reduccion_global),sizeof(uint32_t),&offset);//size_item_transformacion
		serializarDato(serializedPackage,serialized_item_reduccion_global,sizeof(char)*size_item_reduccion_global,&offset);//item_transformacion
		free(serialized_item_reduccion_global);
	}
	return serializedPackage;
}

char* serializar_t_worker(item_reduccion_global* item_reduccion_global){
	uint32_t total_size = getLong_one_item_reduccion_global(item_reduccion_global);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(item_reduccion_global->nodo_id),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_reduccion_global->ip_worker),sizeof(char[20]),&offset);
	serializarDato(serializedPackage,&(item_reduccion_global->puerto_worker),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_reduccion_global->archivo_temporal_reduccion_local),sizeof(char[50]),&offset);

	return serializedPackage;
}

uint32_t getLong_SolicitudReduccionGlobal(solicitud_reduccion_global* solicitudReduccionGlobal){
	uint32_t total_size = 0;

	uint32_t size_item_encargado = getLong_one_item_reduccion_global(solicitudReduccionGlobal->encargado_reduccion_global);
	size_item_encargado += sizeof(uint32_t);

	uint32_t size_items = getLong_items_reduccion_global(solicitudReduccionGlobal->items_reduccion_global,solicitudReduccionGlobal->item_cantidad);
	total_size += sizeof(uint32_t)*2;//campo item_cantidad y size_items
	total_size += sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]);//campo archivo_temporal_reduccion_global
	total_size += size_items;
	total_size += size_item_encargado;

	return total_size;
}

uint32_t getLong_items_reduccion_global(item_reduccion_global* items_reduccion_global, uint32_t item_cantidad){
	uint32_t total = 0;
	int i;
	for(i=0; i<item_cantidad; i++){
		total += sizeof(uint32_t);
		total += getLong_one_item_reduccion_global(&(items_reduccion_global[i]));
	}
	return total;
}

uint32_t getLong_one_item_reduccion_global(item_reduccion_global* items_reduccion_global){
	uint32_t longitud = 0;
	longitud += sizeof(uint32_t)*2; //nodo_id, puerto_worker
	longitud += sizeof(char[20]); //ip_worker,
	longitud += sizeof(char[50])*2;//archivo_temporal (reduccion_local)
	return longitud;
}

solicitud_reduccion_global* deserializar_solicitud_reduccion_global(char* serialized){
	solicitud_reduccion_global* solicitudReduccionGlobal = malloc(sizeof(solicitud_reduccion_global));
	int offset = 0;
	deserializarDato(&(solicitudReduccionGlobal->item_cantidad),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitudReduccionGlobal->archivo_temporal_reduccion_global),serialized,sizeof(char[LENGTH_RUTA_ARCHIVO_TEMP]),&offset);

	int size_encargado;
	memcpy(&size_encargado,serialized+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	char* serialized_encargado = malloc(sizeof(char)*size_encargado);
	memcpy(serialized_encargado,serialized+offset,size_encargado);
	offset+=size_encargado;

	solicitudReduccionGlobal->encargado_reduccion_global = deserializar_item_reduccion_global(serialized_encargado);
	free(serialized_encargado);

	uint32_t size_items;
	deserializarDato(&size_items,serialized,sizeof(uint32_t),&offset);

	char* serialized_items = malloc(sizeof(char)*size_items);
	deserializarDato(serialized_items,serialized,size_items,&offset);
	solicitudReduccionGlobal->items_reduccion_global = deserializar_items_reduccion_global(serialized_items,solicitudReduccionGlobal->item_cantidad);
	free(serialized_items);

	return solicitudReduccionGlobal;
}

item_reduccion_global* deserializar_items_reduccion_global(char* serialized, uint32_t items_cantidad){
	int offset = 0;

	//item_reduccion_global* item_reduccion_global = NULL;
	item_reduccion_global* itemsReduccionGlobal = malloc(sizeof(item_reduccion_global)*items_cantidad);
	int i;
	for (i = 0; i < items_cantidad; i++) {
		uint32_t size_item;
		deserializarDato(&(size_item),serialized,sizeof(uint32_t),&offset);
		char* serialized_item = malloc(sizeof(char)*size_item);
		deserializarDato(serialized_item,serialized,size_item,&offset);
		item_reduccion_global* aux = deserializar_item_reduccion_global(serialized_item);
		itemsReduccionGlobal[i] = *(aux);
		free(aux);
		free(serialized_item);
	}
	return itemsReduccionGlobal;
}

item_reduccion_global* deserializar_item_reduccion_global(char* serialized){
	item_reduccion_global* itemReduccionGlobal = malloc(sizeof(item_reduccion_global));
	int offset = 0;

	deserializarDato(&(itemReduccionGlobal->nodo_id),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemReduccionGlobal->ip_worker),serialized,sizeof(char[20]),&offset);
	deserializarDato(&(itemReduccionGlobal->puerto_worker),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemReduccionGlobal->archivo_temporal_reduccion_local),serialized,sizeof(char[50]),&offset);

	return itemReduccionGlobal;
}

item_reduccion_global* crearItemReduccionGlobal(uint32_t nodo,char* ipWorker,uint32_t puerto_worker, char* archivoTemporalReduccionLocal){
	item_reduccion_global *item = malloc(sizeof(item_reduccion_global));
	strcpy(item->archivo_temporal_reduccion_local,archivoTemporalReduccionLocal);
	item->nodo_id = nodo;
	item->puerto_worker = puerto_worker;
	strcpy(item->ip_worker,ipWorker);
	return item;
}

void agregarItemReduccionGlobal(solicitud_reduccion_global* solicitudReduccionGlobal, item_reduccion_global* item){
	solicitudReduccionGlobal->items_reduccion_global = realloc(solicitudReduccionGlobal->items_reduccion_global,sizeof(item_reduccion_global)*(solicitudReduccionGlobal->item_cantidad+1));
	strcpy(solicitudReduccionGlobal->items_reduccion_global[solicitudReduccionGlobal->item_cantidad].archivo_temporal_reduccion_local,item->archivo_temporal_reduccion_local);
	solicitudReduccionGlobal->items_reduccion_global[solicitudReduccionGlobal->item_cantidad].nodo_id = item->nodo_id;
	strcpy(solicitudReduccionGlobal->items_reduccion_global[solicitudReduccionGlobal->item_cantidad].ip_worker,item->ip_worker);
	solicitudReduccionGlobal->items_reduccion_global[solicitudReduccionGlobal->item_cantidad].puerto_worker = item->puerto_worker;
	solicitudReduccionGlobal->item_cantidad++;
}

void testSerializarSolicitudReduccionGlobal(){

	item_reduccion_global* item1 = crearItemReduccionGlobal(1,"127.0.0.1",8080,"/tmp/Master1-temp38");
	item_reduccion_global* item2 = crearItemReduccionGlobal(12,"127.23.0.1",0101,"/tmp/Master1-temp39");
	item_reduccion_global* item3 = crearItemReduccionGlobal(137,"187.0.56.1",9090,"/tmp/Master1-temp44");

	item_reduccion_global* itemEncargado = crearItemReduccionGlobal(137,"187.0.56.1",9090,"/tmp/ruta_encargado");

	solicitud_reduccion_global* solicitudReduccionGlobal = malloc(sizeof(solicitud_reduccion_global));

	solicitudReduccionGlobal->item_cantidad = 0;

	agregarItemReduccionGlobal(solicitudReduccionGlobal,item1);
	free(item1);
	agregarItemReduccionGlobal(solicitudReduccionGlobal,item2);
	free(item2);
	agregarItemReduccionGlobal(solicitudReduccionGlobal,item3);
	free(item3);

	solicitudReduccionGlobal->encargado_reduccion_global = itemEncargado;
	strcpy(solicitudReduccionGlobal->archivo_temporal_reduccion_global, "/tmp/archivoResultante");

	char* solicitudRedGlobalSerializada = serializarSolicitudReduccionGlobal(solicitudReduccionGlobal);

	solicitud_reduccion_global* solicitudRedGlobalDeserializada = deserializar_solicitud_reduccion_global(solicitudRedGlobalSerializada);

	printf("cantidad de items = %d\n", solicitudRedGlobalDeserializada->item_cantidad );
	printf("archivo_temporal_reduccion_global = %s\n", solicitudRedGlobalDeserializada->archivo_temporal_reduccion_global );
	int var2;
	printf("\nNUEVO ITEM ENCARGADO//////////////////////////////////////////////\n");
	printf("archivo_temporal_reduccion_local = %s\n", solicitudRedGlobalDeserializada->items_reduccion_global->archivo_temporal_reduccion_local );
	printf("nodo_id = %d\n", solicitudRedGlobalDeserializada->items_reduccion_global->nodo_id );
	printf("ip_worker = %s\n", solicitudRedGlobalDeserializada->items_reduccion_global->ip_worker );
	printf("puerto_worker = %d\n", solicitudRedGlobalDeserializada->items_reduccion_global->puerto_worker );
	for (var2 = 0; var2 < solicitudRedGlobalDeserializada->item_cantidad; ++var2) {
		printf("\nNUEVO ITEM DESERIALIZADO//////////////////////////////////////////////\n");
		printf("archivo_temporal_reduccion_local = %s\n", solicitudRedGlobalDeserializada->items_reduccion_global[var2].archivo_temporal_reduccion_local );
		printf("nodo_id = %d\n", solicitudRedGlobalDeserializada->items_reduccion_global[var2].nodo_id );
		printf("ip_worker = %s\n", solicitudRedGlobalDeserializada->items_reduccion_global[var2].ip_worker );
		printf("puerto_worker = %d\n", solicitudRedGlobalDeserializada->items_reduccion_global[var2].puerto_worker );
	}
}

void testSerializarItemReduccionGlobal(){

	item_reduccion_global *item1 = crearItemReduccionGlobal(1,"127.0.0.1",8080,"/tmp/Master1-temp38");

	char* itemSerializado = serializar_t_worker(item1);

	item_reduccion_global *itemDeserializado = deserializar_item_reduccion_global(itemSerializado);
	printf("archivo_temporal_transformacion = %s\n", itemDeserializado->archivo_temporal_reduccion_local );
	printf("nodo_id = %d\n", itemDeserializado->nodo_id );
	printf("ip_worker = %s\n", itemDeserializado->ip_worker );
	printf("puerto_worker = %s\n", itemDeserializado->puerto_worker );
}

uint32_t getLong_SolicitudAlmacenadoFinal(solicitud_almacenado_final* solicitudAlmacenadoFinal){
	uint32_t longitud = 0;
	longitud += sizeof(uint32_t)*2; //nodo_id, puerto_worker
	longitud += sizeof(char[20]); //ip_worker,
	longitud += sizeof(char[50]);//archivo_temporal_reduccion_global
	return longitud;
}

char* serializarSolicitudAlmacenadoFinal(solicitud_almacenado_final* solicitudAlmacenadoFinal){
	uint32_t total_size = getLong_SolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(solicitudAlmacenadoFinal->nodo_id),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(solicitudAlmacenadoFinal->ip_worker),sizeof(char[20]),&offset);
	serializarDato(serializedPackage,&(solicitudAlmacenadoFinal->puerto_worker),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(solicitudAlmacenadoFinal->archivo_temporal_reduccion_global),sizeof(char[50]),&offset);

	return serializedPackage;
}

solicitud_almacenado_final* deserializar_solicitud_almacenado_final(char* serialized){
	solicitud_almacenado_final* solicitudAlmacenadoFinal = malloc(sizeof(solicitud_almacenado_final));
	int offset = 0;

	deserializarDato(&(solicitudAlmacenadoFinal->nodo_id),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitudAlmacenadoFinal->ip_worker),serialized,sizeof(char[20]),&offset);
	deserializarDato(&(solicitudAlmacenadoFinal->puerto_worker),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(solicitudAlmacenadoFinal->archivo_temporal_reduccion_global),serialized,sizeof(char[50]),&offset);

	return solicitudAlmacenadoFinal;
}

void testSerializarSolicitudAlmacenadoFinal(){
	solicitud_almacenado_final *solicitud = malloc(sizeof(solicitud_almacenado_final));
	strcpy(solicitud->archivo_temporal_reduccion_global, "/tmp/Master1-final");
	solicitud->nodo_id = 2;
	solicitud->puerto_worker = 1234;
	strcpy(solicitud->ip_worker,"192.168.1.11");
	solicitud->nodo_id = 5555;

	char* solicitudSerializado = serializarSolicitudAlmacenadoFinal(solicitud);

	solicitud_almacenado_final *solicitudDeserializado = deserializar_solicitud_almacenado_final(solicitudSerializado);
	printf("archivo_temporal = %s\n", solicitudDeserializado->archivo_temporal_reduccion_global );
	printf("nodo_id = %d\n", solicitudDeserializado->nodo_id );
	printf("ip_worker = %s\n", solicitudDeserializado->ip_worker );
	printf("puerto_worker = %d\n", solicitudDeserializado->puerto_worker );
}


#endif /* INTERFAZ_C_ */

