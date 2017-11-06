#include "funcionesmaster.h"
#include "interface.h"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "interfaceWorker.h"
#include <pthread.h>

/*
 typedef struct {
	char programa_transformacion[LENGTH_NOMBRE_PROGRAMA];
	char* programa; //contenido del programa
	uint32_t length_programa;
	uint32_t bloque;  //bloque a aplicar programa de Transformacion
	uint32_t bytes_ocupados;
	char archivo_temporal[LENGTH_RUTA_ARCHIVO_TEMP]; //ruta de archivo temporal
} solicitud_programa_transformacion;
 */


void *enviarTransformacionWorker(void *args){
	item_transformacion *itemTransformacion = (item_transformacion*) args;
	int socketConn = conectarseA(itemTransformacion->ip_worker, itemTransformacion->puerto_worker);
	enviarInt(socketConn,PROCESO_MASTER);

	solicitud_programa_transformacion* solicitud = malloc(sizeof(solicitud_programa_transformacion));
	strcpy(&(solicitud->programa_transformacion),"script_transformacion.py");
	char* filebuffer = fileToChar("script_transformacion.py");
	printf("file = %s\n", filebuffer );
	solicitud->programa = filebuffer;
	strcpy(&(solicitud->archivo_temporal),itemTransformacion->archivo_temporal);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->bloque = itemTransformacion->bloque;
	solicitud->bytes_ocupados = itemTransformacion->bytes_ocupados;

	char* serializado = serializarSolicitudProgramaTransformacion(solicitud);
	int len = getLong_SolicitudProgramaTransformacion(solicitud);

	solicitud_programa_transformacion* deserializado = deserializarSolicitudProgramaTransformacion(serializado);
	printf("programa = %s\n", deserializado->programa );

	enviarMensajeSocketConLongitud(socketConn, ACCION_TRANSFORMACION, serializado, len);
	free(args);
}

void procesarSolicitudTransformacion(int socket, int message_long, char* message){
	solicitud_transformacion* solicitudTransfDeserializada = deserializar_solicitud_transformacion(message);

	printf("cantidad de items = %d\n", solicitudTransfDeserializada->item_cantidad );
	int var;
	for (var = 0; var < solicitudTransfDeserializada->item_cantidad; ++var) {
		item_transformacion* itemTransformacion = crearItemTransformacion(
				solicitudTransfDeserializada->items_transformacion[var].nodo_id,
				solicitudTransfDeserializada->items_transformacion[var].ip_worker,
				solicitudTransfDeserializada->items_transformacion[var].puerto_worker,
				solicitudTransfDeserializada->items_transformacion[var].bloque,
				solicitudTransfDeserializada->items_transformacion[var].bytes_ocupados,
				solicitudTransfDeserializada->items_transformacion[var].archivo_temporal);
		pthread_t threadSolicitudTransformacionWorker;
		enviarTransformacionWorker((void*) itemTransformacion);
		//int er1 = pthread_create(&threadSolicitudTransformacionWorker, NULL,enviarTransformacionWorker,(void*) itemTransformacion);
		//pthread_join(threadSolicitudTransformacionWorker, NULL);
	}
}

void procesarSolicitudReduccionLocal(int socket, int message_long, char* message){
	solicitud_reduccion_local* solicitudReducLocalDeserializado = deserializar_solicitud_reduccion_local(message);

	printf("cantidad de items = %d\n", solicitudReducLocalDeserializado->item_cantidad );
	int var;
	for (var = 0; var < solicitudReducLocalDeserializado->item_cantidad; ++var) {
		printf("\nNUEVO ITEM DESERIALIZADO//////////////////////////////////////////////\n");
		printf("nodo_id = %d\n", solicitudReducLocalDeserializado->items_reduccion_local[var].nodo_id );
		printf("puerto_worker = %d\n", solicitudReducLocalDeserializado->items_reduccion_local[var].puerto_worker );
		printf("ip_worker = %s\n", solicitudReducLocalDeserializado->items_reduccion_local[var].ip_worker );
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
		printf("ip_worker = %s\n", solicitudReducGlobalDeserializado->items_reduccion_global[var].ip_worker );
		printf("puerto_worker = %d\n", solicitudReducGlobalDeserializado->items_reduccion_global[var].puerto_worker );
		printf("archivo_temporal_transformacion = %s\n", solicitudReducGlobalDeserializado->items_reduccion_global[var].archivo_temporal_reduccion_local );
	}
}

void procesarSolicitudAlmacenadoFinal(int socket, int message_long, char* message){
	solicitud_almacenado_final* solicitudAlmacFinalDeserializado = deserializar_solicitud_almacenado_final(message);

	printf("archivo_temporal = %s\n", solicitudAlmacFinalDeserializado->archivo_temporal_reduccion_global );
	printf("nodo_id = %d\n", solicitudAlmacFinalDeserializado->nodo_id );
	printf("ip_worker = %s\n", solicitudAlmacFinalDeserializado->ip_worker );
	printf("puerto_worker = %d\n", solicitudAlmacFinalDeserializado->puerto_worker );
}
