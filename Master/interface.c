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

void deserializarDato(void* dato, char* buffer, int size, int* offset){
	memcpy(dato,buffer + *offset,size);
	*offset += size;
}

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
	longitud += sizeof(uint32_t)*3; //nodo_id, bloque, bytes_ocupados
	longitud += sizeof(char[20]); //ip_puerto_worker,
	longitud += sizeof(char[50]);//archivo_temporal
	return longitud;
}

char* serializar_item_transformacion(item_transformacion* item_transformacion){
	uint32_t total_size = getLong_one_item_transformacion(item_transformacion);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(item_transformacion->nodo_id),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->ip_puerto_worker),sizeof(char[20]),&offset);
	serializarDato(serializedPackage,&(item_transformacion->bloque),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->bytes_ocupados),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->archivo_temporal),sizeof(char[50]),&offset);

	return serializedPackage;
}

solicitud_transformacion* deserializar_solicitud_transformacion(char* serialized){
	solicitud_transformacion* solicitudTransformacion = malloc(sizeof(solicitud_transformacion));
	int offset = 0;
	printf("ip_puerto_worker = %s\n", "afafas");
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
	deserializarDato(&(itemTransformacion->ip_puerto_worker),serialized,sizeof(char[20]),&offset);
	deserializarDato(&(itemTransformacion->bloque),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemTransformacion->bytes_ocupados),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemTransformacion->archivo_temporal),serialized,sizeof(char[50]),&offset);

	return itemTransformacion;
}

item_transformacion* crearItemTransformacion(uint32_t nodo,char* ipPuertoWorker, uint32_t bloque, uint32_t bytesOcupados, char* archivoTemporal){
	item_transformacion *item = malloc(sizeof(item_transformacion));
	strcpy(item->archivo_temporal,archivoTemporal);
	item->bloque = bloque;
	item->bytes_ocupados = bytesOcupados;
	item->nodo_id = nodo;
	strcpy(item->ip_puerto_worker,ipPuertoWorker);
	return item;
}

void testSerializarSolicitudTrasnformacion(){

	item_transformacion* item1 = crearItemTransformacion(1,"127.0.0.1:8080",2222,12345,"/temp1/archivo1.txt");
	item_transformacion* item2 = crearItemTransformacion(12,"127.23.0.1:0101",523,5777666,"/temsssssp1211/otro.txt");
	item_transformacion* item3 = crearItemTransformacion(137,"187.0.56.1:9090",62,643,"/temp655/tercero.txt");

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
		printf("ip_puerto_worker = %s\n", solicitudTransfDeserializada->items_transformacion[var2].ip_puerto_worker );
	}
}

void agregarItemTransformacion(solicitud_transformacion* solicitudTransformacion, item_transformacion* item){
	solicitudTransformacion->items_transformacion = realloc(solicitudTransformacion->items_transformacion,sizeof(item_transformacion)*(solicitudTransformacion->item_cantidad+1));
	strcpy(solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].archivo_temporal,item->archivo_temporal);
	solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].bloque = item->bloque;
	solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].bytes_ocupados = item->bytes_ocupados;
	solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].nodo_id = item->nodo_id;
	strcpy(solicitudTransformacion->items_transformacion[solicitudTransformacion->item_cantidad].ip_puerto_worker,item->ip_puerto_worker);
	solicitudTransformacion->item_cantidad++;
}

void testSerializarItemTransformacion(){

	item_transformacion *item1 = crearItemTransformacion(1,"127.0.0.1:8080",1,12345,"/temp1/archivo1.txt");

	char* itemSerializado = serializar_item_transformacion(item1);

	item_transformacion *itemDeserializado = deserializar_item_transformacion(itemSerializado);
	printf("archivo_temporal = %s\n", itemDeserializado->archivo_temporal );
	printf("bloque = %d\n", itemDeserializado->bloque );
	printf("bytes_ocupados = %d\n", itemDeserializado->bytes_ocupados );
	printf("nodo_id = %d\n", itemDeserializado->nodo_id );
	printf("ip_puerto_worker = %s\n", itemDeserializado->ip_puerto_worker );
}

#endif /* INTERFAZ_C_ */
