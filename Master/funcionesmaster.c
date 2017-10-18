#include "funcionesmaster.h"
#include "interface.h"
#include "../bibliotecas/sockets.h"

void procesarSolicitudTransformacion(int socket, int message_long, char* message){
	solicitud_transformacion* solicitudTransfDeserializada = deserializar_solicitud_transformacion(message);

	printf("cantidad de items = %d\n", solicitudTransfDeserializada->item_cantidad );
	int var;
	for (var = 0; var < solicitudTransfDeserializada->item_cantidad; ++var) {
		printf("\nNUEVO ITEM DESERIALIZADO//////////////////////////////////////////////\n");
		printf("archivo_temporal = %s\n", solicitudTransfDeserializada->items_transformacion[var].archivo_temporal );
		printf("bloque = %d\n", solicitudTransfDeserializada->items_transformacion[var].bloque );
		printf("bytes_ocupados = %d\n", solicitudTransfDeserializada->items_transformacion[var].bytes_ocupados );
		printf("nodo_id = %d\n", solicitudTransfDeserializada->items_transformacion[var].nodo_id );
		printf("ip_puerto_worker = %s\n", solicitudTransfDeserializada->items_transformacion[var].ip_puerto_worker );
	}
}

void procesarSolicitudReduccionLocal(int socket, int message_long, char* message){
	solicitud_reduccion_local* solicitudReducLocalDeserializado = deserializar_solicitud_reduccion_local(message);

	printf("cantidad de items = %d\n", solicitudReducLocalDeserializado->item_cantidad );
	int var;
	for (var = 0; var < solicitudReducLocalDeserializado->item_cantidad; ++var) {
		printf("\nNUEVO ITEM DESERIALIZADO//////////////////////////////////////////////\n");
		printf("nodo_id = %d\n", solicitudReducLocalDeserializado->items_reduccion_local[var].nodo_id );
		printf("ip_puerto_worker = %s\n", solicitudReducLocalDeserializado->items_reduccion_local[var].ip_puerto_worker );
		printf("archivo_temporal_transformacion = %s\n", solicitudReducLocalDeserializado->items_reduccion_local[var].archivo_temporal_transformacion );
		printf("archivo_temporal_reduccion_local = %s\n", solicitudReducLocalDeserializado->items_reduccion_local[var].archivo_temporal_reduccion_local );
	}
}

void procesarSolicitudReduccionGlobal(int socket, int message_long, char* message){
	solicitud_reduccion_global* solicitudReducGlobalDeserializado = deserializar_solicitud_reduccion_global(message);

	printf("cantidad de items = %d\n", solicitudReducGlobalDeserializado->item_cantidad );
	int var;
	for (var = 0; var < solicitudReducGlobalDeserializado->item_cantidad; ++var) {
		printf("\nNUEVO ITEM DESERIALIZADO//////////////////////////////////////////////\n");
		printf("nodo_id = %d\n", solicitudReducGlobalDeserializado->items_reduccion_global[var].nodo_id );
		printf("ip_puerto_worker = %s\n", solicitudReducGlobalDeserializado->items_reduccion_global[var].ip_puerto_worker );
		printf("archivo_temporal_transformacion = %s\n", solicitudReducGlobalDeserializado->items_reduccion_global[var].archivo_temporal_reduccion_local );
		printf("archivo_temporal_reduccion_local = %s\n", solicitudReducGlobalDeserializado->items_reduccion_global[var].archivo_temporal_reduccion_global );
	}
}

void procesarSolicitudAlmacenadoFinal(int socket, int message_long, char* message){
	solicitud_almacenado_final* solicitudAlmacFinalDeserializado = deserializar_solicitud_almacenado_final(message);

	printf("archivo_temporal = %s\n", solicitudAlmacFinalDeserializado->archivo_temporal_reduccion_global );
	printf("nodo_id = %d\n", solicitudAlmacFinalDeserializado->nodo_id );
	printf("ip_puerto_worker = %s\n", solicitudAlmacFinalDeserializado->ip_puerto_worker );
}
