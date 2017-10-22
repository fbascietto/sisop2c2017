#include "funcionesmaster.h"
#include "interface.h"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "interfaceWorker.h"


void *enviarTransformacionWorker(void *args){
	item_transformacion *itemTransformacion = (item_transformacion*) args;
	int socketConn = conectarseA(itemTransformacion->ip_worker, itemTransformacion->puerto_worker);
	enviarInt(socketConn,PROCESO_MASTER);

	/*solicitud_programa_transformacion* solicitud = malloc(sizeof(solicitud_programa_transformacion));
	strcpy(&(solicitud_programa_transformacion->ruta_programa_transformacion),"unaRuta.sh");
	char* filebuffer = fileToChar("unaRuta.sh");
	strcpy(&(solicitud_programa_transformacion->programa,filebuffer));
	strcpy(&(solicitud_programa_transformacion->archivo_temporal,"archivotemp.txt"));
	solicitud_programa_transformacion->length_programa = strlen(filebuffer);
	solicitud_programa_transformacion->bloque = 12;
	solicitud_programa_transformacion->bytes_ocupados = 2000;

	char* serializado = serializarSolicitudProgramaTransformacion(solicitud_programa_transformacion);
	int len = getLong_SolicitudProgramaTransformacion(solicitud_programa_transformacion);

	enviarMensajeSocketConLongitud(socketConn, ENVIAR_PROGRAMA_TRANSFORMACION, serializado, len);
	free(args);*/
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
		//int er1 = pthread_create(&threadSolicitudTransformacionWorker, NULL,enviarTransformacionWorker,(void*) itemTransformacion);
		//pthread_join(threadSolicitudTransformacionWorker, NULL);
		printf("\nNUEVO ITEM DESERIALIZADO//////////////////////////////////////////////\n");
		printf("archivo_temporal = %s\n", solicitudTransfDeserializada->items_transformacion[var].archivo_temporal );
		printf("bloque = %d\n", solicitudTransfDeserializada->items_transformacion[var].bloque );
		printf("bytes_ocupados = %d\n", solicitudTransfDeserializada->items_transformacion[var].bytes_ocupados );
		printf("nodo_id = %d\n", solicitudTransfDeserializada->items_transformacion[var].nodo_id );
		printf("ip_worker = %s\n", solicitudTransfDeserializada->items_transformacion[var].ip_worker );
		printf("puerto_worker = %d\n", solicitudTransfDeserializada->items_transformacion[var].puerto_worker );
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
