#include "funcionesmaster.h"
#include "../bibliotecas/sockets.h"

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
	longitud += sizeof(uint32_t)*4; //nodo_id, ip_puerto_worker, bloque, bytes_ocupados
	longitud += sizeof(char)*2; //ip_puerto_worker, archivo_temporal
	return longitud;
}

char* serializar_item_transformacion(item_transformacion* item_transformacion){
	uint32_t total_size = getLong_one_item_transformacion(item_transformacion);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	serializarDato(serializedPackage,&(item_transformacion->nodo_id),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->ip_puerto_worker),sizeof(char),&offset);
	serializarDato(serializedPackage,&(item_transformacion->bloque),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->bytes_ocupados),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(item_transformacion->archivo_temporal),sizeof(char),&offset);

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
	item_transformacion* itemsTransformacion = malloc(sizeof(itemsTransformacion)*items_cantidad);
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
	deserializarDato(&(itemTransformacion->ip_puerto_worker),serialized,sizeof(char),&offset);
	deserializarDato(&(itemTransformacion->bloque),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemTransformacion->bytes_ocupados),serialized,sizeof(uint32_t),&offset);
	deserializarDato(&(itemTransformacion->archivo_temporal),serialized,sizeof(char),&offset);

	return itemTransformacion;
}



