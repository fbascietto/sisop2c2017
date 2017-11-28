#include "funcionesmaster.h"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "interfaceWorker.h"
#include "interfaceMaster.h"

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


void enviarTransformacionWorker(void *args){
	item_transformacion *itemTransformacion = (item_transformacion*) args;

	solicitud_programa_transformacion* solicitud = malloc(sizeof(solicitud_programa_transformacion));
	strcpy(&(solicitud->programa_transformacion),ruta_programa_transformador);
	char* filebuffer = fileToChar(ruta_programa_transformador);
	printf("file = %s\n", filebuffer );
	solicitud->programa = filebuffer;
	strcpy(&(solicitud->archivo_temporal),itemTransformacion->archivo_temporal);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->bloque = itemTransformacion->bloque;
	solicitud->bytes_ocupados = itemTransformacion->bytes_ocupados;

	char* serializado = serializarSolicitudProgramaTransformacion(solicitud);
	int len = getLong_SolicitudProgramaTransformacion(solicitud);

	//solicitud_programa_transformacion* deserializado = deserializarSolicitudProgramaTransformacion(serializado);
	//printf("programa = %s\n", deserializado->programa );

	int socketConn = conectarseA(itemTransformacion->ip_worker, itemTransformacion->puerto_worker);
	enviarInt(socketConn,PROCESO_MASTER);
	enviarMensajeSocketConLongitud(socketConn, ACCION_TRANSFORMACION, serializado, len);

	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, socketConn);
	switch(package->msgCode){
		case TRANSFORMACION_OK:
			//log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de transformacion de un bloque a Yama");
			enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_OK,solicitud->bloque,itemTransformacion->nodo_id);
			break;
		case TRANSFORMACION_ERROR_CREACION:
			//log_error(worker_error_log, "Se envia a Master el error de creacion del programa de transformacion");
			enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_ERROR,solicitud->bloque,itemTransformacion->nodo_id);
			break;
		case TRANSFORMACION_ERROR_ESCRITURA:
			//log_error(worker_error_log, "Se envia a Master el error de escritura del contenido del programa de transformacion");
			enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_ERROR,solicitud->bloque,itemTransformacion->nodo_id);
			break;
		case 100:
			//recibir ERROR de apertura de data.bin
			break;
		case 200:
			//recibir ERROR de lectura de data.bin
			break;
		case TRANSFORMACION_ERROR_PERMISOS:
			//log_error(worker_error_log, "Se envia a Master el error al dar permisos de ejecucion al programa de transformacion");
			enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_ERROR,solicitud->bloque,itemTransformacion->nodo_id);
			break;
	}
}

void enviarResultadoTransformacionYama(int socket, uint32_t code, int bloque, char* nodo_id){
	int total_size = sizeof(char[NOMBRE_NODO]) + sizeof(uint32_t);
	char *serializedPackage = malloc(total_size);
	int offset = 0;
	serializarDato(serializedPackage,&(bloque),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,&(nodo_id),sizeof(char[NOMBRE_NODO]),&offset);
	enviarMensajeSocketConLongitud(socket, code, serializedPackage, total_size);
}

void enviarResultadoReduccionLocalYama(int socket, uint32_t code, char* nodo_id){
	int total_size = sizeof(char[NOMBRE_NODO]);
	char *serializedPackage = malloc(total_size);
	int offset = 0;
	serializarDato(serializedPackage,&(nodo_id),sizeof(char[NOMBRE_NODO]),&offset);
	enviarMensajeSocketConLongitud(socket, code, serializedPackage, total_size);
}

void enviarResultadoReduccionGlobalYama(int socket, uint32_t code, char* nodo_id){
	int total_size = sizeof(char[NOMBRE_NODO]);
	char *serializedPackage = malloc(total_size);
	int offset = 0;
	serializarDato(serializedPackage,&(nodo_id),sizeof(char[NOMBRE_NODO]),&offset);
	enviarMensajeSocketConLongitud(socket, code, serializedPackage, total_size);
}

void enviarReduccionLocalWorker(void *args){
	item_reduccion_local *itemRedLocal = (item_reduccion_local*) args;

	solicitud_programa_reduccion_local* solicitud = malloc(sizeof(solicitud_programa_reduccion_local));
	strcpy(&(solicitud->programa_reduccion),ruta_programa_reductor);
	char* filebuffer = fileToChar(ruta_programa_reductor);
	solicitud->programa = filebuffer;
	strcpy(&(solicitud->archivo_temporal_resultante),itemRedLocal->archivo_temporal_reduccion_local);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->cantidad_archivos_temp = itemRedLocal->cantidad_archivos_temp;
	solicitud->archivos_temporales = itemRedLocal->archivos_temporales_transformacion;

	char* serializado = serializarSolicitudProgramaReduccionLocal(solicitud);
	int len = getLong_SolicitudProgramaReduccionLocal(solicitud);

	//solicitud_programa_reduccion_local* deserializado = deserializarSolicitudProgramaReduccionLocal(serializado);
	//printf("programa = %s\n", deserializado->programa );

	int socketConn = conectarseA(itemRedLocal->ip_worker, itemRedLocal->puerto_worker);
	enviarInt(socketConn,PROCESO_MASTER);
	enviarMensajeSocketConLongitud(socketConn, ACCION_REDUCCION_LOCAL, serializado, len);

	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, socketConn);
	switch(package->msgCode){
		case REDUCCION_LOCAL_OK:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_OK,itemRedLocal->nodo_id);
			break;
		case REDUCCION_LOCAL_ERROR_CREACION:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,itemRedLocal->nodo_id);
			break;
		case REDUCCION_LOCAL_ERROR_ESCRITURA:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,itemRedLocal->nodo_id);
			break;
		case REDUCCION_LOCAL_ERROR_SYSTEM:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,itemRedLocal->nodo_id);
			break;
		case REDUCCION_LOCAL_ERROR_PERMISOS:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,itemRedLocal->nodo_id);
			break;
	}

}

void enviarReduccionGlobalWorker(void *args){
	solicitud_reduccion_global *solicitudRedGlobal = (solicitud_reduccion_global*) args;

	solicitud_programa_reduccion_global* solicitud = malloc(sizeof(solicitud_programa_reduccion_global));
	strcpy(&(solicitud->programa_reduccion),ruta_programa_reductor);
	char* filebuffer = fileToChar(ruta_programa_reductor);
	solicitud->programa = filebuffer;
	strcpy(&(solicitud->archivo_temporal_resultante),solicitudRedGlobal->archivo_temporal_reduccion_global);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->cantidad_workers = solicitudRedGlobal->item_cantidad;
	solicitud->workers = solicitudRedGlobal->workers;

	char* serializado = serializarSolicitudProgramaReduccionGlobal(solicitud);
	int len = getLong_SolicitudProgramaReduccionLocal(solicitud);

	//solicitud_programa_reduccion_global* deserializado = deserializarSolicitudProgramaReduccionGlobal(serializado);
	//printf("programa = %s\n", deserializado->programa );

	int socketConn = conectarseA(solicitudRedGlobal->encargado_worker->ip_worker, solicitudRedGlobal->encargado_worker->ip_worker);
	enviarInt(socketConn,PROCESO_MASTER);
	enviarMensajeSocketConLongitud(socketConn, ACCION_REDUCCION_GLOBAL, serializado, len);

	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, socketConn);
	switch(package->msgCode){
		case REDUCCION_GLOBAL_OK:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_OK,solicitudRedGlobal->encargado_worker->nodo_id);
			break;
		case REDUCCION_GLOBAL_ERROR_CREACION:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
			break;
		case REDUCCION_GLOBAL_ERROR_ESCRITURA:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
			break;
		case REDUCCION_GLOBAL_ERROR_APAREO:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
			break;
		case REDUCCION_GLOBAL_ERROR_SYSTEM:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
			break;
		case REDUCCION_GLOBAL_ERROR_PERMISOS:
			enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
			break;
	}
}

void procesarSolicitudTransformacion(int socket, int message_long, char* message){
	solicitud_transformacion* solicitudTransfDeserializada = deserializar_solicitud_transformacion(message);

	printf("cantidad de items = %d\n", solicitudTransfDeserializada->item_cantidad );
	int var;
	for (var = 0; var < solicitudTransfDeserializada->item_cantidad; ++var) {
		item_transformacion* itemTransformacion = malloc(sizeof(item_transformacion));
		itemTransformacion = &(solicitudTransfDeserializada->items_transformacion[var]);
		pthread_t threadSolicitudTransformacionWorker;
		int er1 = pthread_create(&threadSolicitudTransformacionWorker, NULL,enviarTransformacionWorker,(void*) itemTransformacion);
		//enviarTransformacionWorker((void*) itemTransformacion);
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
		int er1 = pthread_create(&threadSolicitudRedLocalWorker, NULL,enviarReduccionLocalWorker,(void*) itemRedLocal);
		//enviarReduccionLocalWorker((void*) itemRedLocal);
		//pthread_join(threadSolicitudTransformacionWorker, NULL);
	}
}

void procesarSolicitudReduccionGlobal(int socket, int message_long, char* message){
	solicitud_reduccion_global* solicitudReducLocalDeserializado = deserializar_solicitud_reduccion_global(message);
	pthread_t threadSolicitudRedGlobalWorker;
	int er1 = pthread_create(&threadSolicitudRedGlobalWorker, NULL,enviarReduccionGlobalWorker,(void*) solicitudReducLocalDeserializado);
	//enviarReduccionGlobalWorker((void*) solicitudReducLocalDeserializado);
	//pthread_join(threadSolicitudRedGlobalWorker, NULL);

}

void enviarSolicitudFinalWorker(void *args){
	solicitud_almacenado_final *solicitudFinal = (solicitud_almacenado_final*) args;
	//int socketConn = conectarseA(solicitudFinal->ip_worker, solicitudFinal->puerto_worker);
	//enviarInt(socketConn,PROCESO_MASTER);

	solicitud_realizar_almacenamiento_final* solicitud = malloc(sizeof(solicitud_realizar_almacenamiento_final));

	strcpy(&(solicitud->ruta_archivo_temporal_resultante_reduccion_global), solicitudFinal->archivo_temporal_reduccion_global);
	strcpy(&(solicitud->ruta_archivo_final_fs), ruta_archivo_final_fs);

	char* serializado = serializarSolicitudRealizarAlmacenadoFinal(solicitud);
	int len = getLong_SolicitudRealizarAlmacenadoFinal(solicitud);

	int socketConn= conectarseA(solicitudFinal->ip_worker, solicitudFinal->puerto_worker);

	enviarMensajeSocketConLongitud(socketConn, ACCION_ALMACENAMIENTO_FINAL, serializado, len);
}

void procesarSolicitudAlmacenadoFinal(int socket, int message_long, char* message){
	solicitud_almacenado_final* solicitudAlmacFinalDeserializado = deserializar_solicitud_almacenado_final(message);
	//enviarSolicitudFinalWorker((void*) solicitudAlmacFinalDeserializado);
	pthread_t threadSolicitudFinalWorker;
	int er1 = pthread_create(&threadSolicitudFinalWorker, NULL,enviarSolicitudFinalWorker,(void*) solicitudAlmacFinalDeserializado);
	//pthread_join(threadSolicitudFinalWorker, NULL);
}
