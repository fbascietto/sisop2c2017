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
	//int socketConn = conectarseA(itemTransformacion->ip_worker, itemTransformacion->puerto_worker);
	//enviarInt(socketConn,PROCESO_MASTER);

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
	//enviarMensajeSocketConLongitud(socketConn, ACCION_TRANSFORMACION, serializado, len);
}

void *enviarReduccionLocalWorker(void *args){
	item_reduccion_local *itemRedLocal = (item_reduccion_local*) args;
	//int socketConn = conectarseA(itemRedLocal->ip_worker, itemRedLocal->puerto_worker);
	//enviarInt(socketConn,PROCESO_MASTER);

	solicitud_programa_reduccion_local* solicitud = malloc(sizeof(solicitud_programa_reduccion_local));
	strcpy(&(solicitud->programa_reduccion),"script_transformacion.py");
	char* filebuffer = fileToChar("script_transformacion.py");
	solicitud->programa = filebuffer;
	strcpy(&(solicitud->archivo_temporal_resultante),itemRedLocal->archivo_temporal_reduccion_local);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->cantidad_archivos_temp = itemRedLocal->cantidad_archivos_temp;
	solicitud->archivos_temporales = itemRedLocal->archivos_temporales_transformacion;

	char* serializado = serializarSolicitudProgramaReduccionLocal(solicitud);
	int len = getLong_SolicitudProgramaReduccionLocal(solicitud);

	solicitud_programa_reduccion_local* deserializado = deserializarSolicitudProgramaReduccionLocal(serializado);
	printf("programa = %s\n", deserializado->programa );


	//enviarMensajeSocketConLongitud(socketConn, ACCION_TRANSFORMACION, serializado, len);
}

void *enviarReduccionGlobalWorker(void *args){
	solicitud_reduccion_global *solicitudRedGlobal = (solicitud_reduccion_global*) args;
	//int socketConn = conectarseA(itemRedLocal->encargado_worker.ip_worker, itemRedLocal->encargado_worker.puerto_worker);
	//enviarInt(socketConn,PROCESO_MASTER);

	solicitud_programa_reduccion_global* solicitud = malloc(sizeof(solicitud_programa_reduccion_global));
	strcpy(&(solicitud->programa_reduccion),"script_transformacion.py");
	char* filebuffer = fileToChar("script_transformacion.py");
	solicitud->programa = filebuffer;
	strcpy(&(solicitud->archivo_temporal_resultante),solicitudRedGlobal->archivo_temporal_reduccion_global);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->cantidad_item_programa_reduccion = solicitudRedGlobal->item_cantidad;
	solicitud->workers = solicitudRedGlobal->workers;

	char* serializado = serializarSolicitudProgramaReduccionGlobal(solicitud);
	int len = getLong_SolicitudProgramaReduccionLocal(solicitud);

	solicitud_programa_reduccion_global* deserializado = deserializarSolicitudProgramaReduccionGlobal(serializado);
	printf("programa = %s\n", deserializado->programa );


	//enviarMensajeSocketConLongitud(socketConn, ACCION_TRANSFORMACION, serializado, len);
}

void procesarSolicitudTransformacion(int socket, int message_long, char* message){
	solicitud_transformacion* solicitudTransfDeserializada = deserializar_solicitud_transformacion(message);

	printf("cantidad de items = %d\n", solicitudTransfDeserializada->item_cantidad );
	int var;
	for (var = 0; var < solicitudTransfDeserializada->item_cantidad; ++var) {
		item_transformacion* itemTransformacion = malloc(sizeof(item_transformacion));
		itemTransformacion = &(solicitudTransfDeserializada->items_transformacion[var]);
		pthread_t threadSolicitudTransformacionWorker;
		enviarTransformacionWorker((void*) itemTransformacion);
		//int er1 = pthread_create(&threadSolicitudTransformacionWorker, NULL,enviarTransformacionWorker,(void*) itemTransformacion);
		//pthread_join(threadSolicitudTransformacionWorker, NULL);
	}
}

void procesarSolicitudReduccionLocal(int socket, int message_long, char* message){
	solicitud_reduccion_local* solicitudReducLocalDeserializado = deserializar_solicitud_reduccion_local(message);
	int var;
	for (var = 0; var < solicitudReducLocalDeserializado->item_cantidad; ++var) {
		item_reduccion_local* itemRedLocal = malloc(sizeof(item_reduccion_local));
		itemRedLocal = &(solicitudReducLocalDeserializado->items_reduccion_local[var]);
		pthread_t threadSolicitudRedLocalWorker;
		enviarReduccionLocalWorker((void*) itemRedLocal);
		//int er1 = pthread_create(&threadSolicitudRedLocalWorker, NULL,enviarReduccionLocalWorker,(void*) itemRedLocal);
		//pthread_join(threadSolicitudTransformacionWorker, NULL);
	}
}

void procesarSolicitudReduccionGlobal(int socket, int message_long, char* message){
	solicitud_reduccion_global* solicitudReducLocalDeserializado = deserializar_solicitud_reduccion_global(message);
	enviarReduccionGlobalWorker((void*) solicitudReducLocalDeserializado);
	//int er1 = pthread_create(&threadSolicitudRedGlobalWorker, NULL,enviarReduccionGlobalWorker,(void*) solicitudReducLocalDeserializado);
	//pthread_join(threadSolicitudRedGlobalWorker, NULL);

}

void *enviarSolicitudFinalWorker(void *args){
	solicitud_almacenado_final *solicitudFinal = (solicitud_almacenado_final*) args;
	//int socketConn = conectarseA(solicitudFinal->ip_worker, solicitudFinal->puerto_worker);
	//enviarInt(socketConn,PROCESO_MASTER);

	char* serializado = serializarSolicitudAlmacenadoFinal(solicitudFinal);
	int len = getLong_SolicitudAlmacenadoFinal(solicitudFinal);

	solicitud_almacenado_final* deserializado = deserializar_solicitud_almacenado_final(serializado);

	printf("final = %s\n", deserializado->archivo_temporal_reduccion_global );
	//enviarMensajeSocketConLongitud(socketConn, ACCION_ALMACENADO_FINAL, serializado, len);
}

void procesarSolicitudAlmacenadoFinal(int socket, int message_long, char* message){
	solicitud_almacenado_final* solicitudAlmacFinalDeserializado = deserializar_solicitud_almacenado_final(message);
	enviarSolicitudFinalWorker((void*) solicitudAlmacFinalDeserializado);
	//int er1 = pthread_create(&threadSolicitudFinalWorker, NULL,enviarSolicitudFinalWorker,(void*) solicitudAlmacFinalDeserializado);
	//pthread_join(threadSolicitudFinalWorker, NULL);
}
