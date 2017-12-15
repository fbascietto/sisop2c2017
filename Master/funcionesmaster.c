#include "funcionesmaster.h"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "interfaceWorker.h"
#include "interfaceMaster.h"

double timeval_diff(struct timeval *a, struct timeval *b)
{
	return
			(double)(a->tv_sec + (double)a->tv_usec/1000000) -
			(double)(b->tv_sec + (double)b->tv_usec/1000000);
}

//todo agregar id de job
void enviarResultadoTransformacionYama(int socket, uint32_t code, int bloque, char* nodo_id){
	int total_size = sizeof(char[NOMBRE_NODO]) + sizeof(uint32_t);
	char *serializedPackage = malloc(total_size);
	int offset = 0;
	serializarDato(serializedPackage,&(bloque),sizeof(uint32_t),&offset);
	serializarDato(serializedPackage,nodo_id,sizeof(char[NOMBRE_NODO]),&offset);
	enviarMensajeSocketConLongitud(socket, code, serializedPackage, total_size);
}

void enviarResultadoReduccionLocalYama(int socket, uint32_t code, char* nodo_id){
	int total_size = sizeof(char[NOMBRE_NODO]);
	char *serializedPackage = malloc(total_size);
	int offset = 0;
	serializarDato(serializedPackage,nodo_id,sizeof(char[NOMBRE_NODO]),&offset);
	enviarMensajeSocketConLongitud(socket, code, serializedPackage, total_size);
}

void enviarResultadoReduccionGlobalYama(int socket, uint32_t code, char* nodo_id){
	int total_size = sizeof(char[NOMBRE_NODO]);
	char *serializedPackage = malloc(total_size);
	int offset = 0;
	serializarDato(serializedPackage,nodo_id,sizeof(char[NOMBRE_NODO]),&offset);
	enviarMensajeSocketConLongitud(socket, code, serializedPackage, total_size);
}

void enviarTransformacionWorker(void *args){
	uint32_t id_job = idJob;
	struct timeval init, end;
	gettimeofday(&init, NULL);
	item_transformacion *itemTransformacion = (item_transformacion*) args;

	solicitud_programa_transformacion* solicitud = malloc(sizeof(solicitud_programa_transformacion));
	strcpy(&(solicitud->programa_transformacion),basename(ruta_programa_transformador));
	char* filebuffer = fileToChar(ruta_programa_transformador);
	//printf("file = %s\n", filebuffer );
	solicitud->programa = filebuffer;
	strcpy(&(solicitud->archivo_temporal),itemTransformacion->archivo_temporal);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->bloque = itemTransformacion->bloque;
	solicitud->bytes_ocupados = itemTransformacion->bytes_ocupados;

	char* serializado = serializarSolicitudProgramaTransformacion(solicitud);
	int len = getLong_SolicitudProgramaTransformacion(solicitud);

	int socketConn = conectarseA(itemTransformacion->ip_worker, itemTransformacion->puerto_worker);
	enviarInt(socketConn,PROCESO_MASTER);
	enviarMensajeSocketConLongitud(socketConn, ACCION_TRANSFORMACION, serializado, len);

	Package* package = createPackage();
	printf("Esperando conexiones del worker con archivo temp: %s\n", solicitud->archivo_temporal);

	int msgcode;
	recibirInt(socketConn, &msgcode);

	printf("Socket %d. Numero de mensaje: %d\n", socketConn, msgcode);
	if(id_job != idJob){
		return;
	}

	switch(msgcode){
	case TRANSFORMACION_OK:
		activosTransformacion--;
		//log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de transformacion de un bloque a Yama");
		enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_OK,solicitud->bloque,itemTransformacion->nodo_id);
		break;
	case TRANSFORMACION_ERROR_CREACION:
		activosTransformacion--;
		fallosEnTotal++;
		//log_error(worker_error_log, "Se envia a Master el error de creacion del programa de transformacion");
		enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_ERROR,solicitud->bloque,itemTransformacion->nodo_id);
		break;
	case TRANSFORMACION_ERROR_ESCRITURA:
		activosTransformacion--;
		fallosEnTotal++;
		//log_error(worker_error_log, "Se envia a Master el error de escritura del contenido del programa de transformacion");
		enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_ERROR,solicitud->bloque,itemTransformacion->nodo_id);
		break;
	case FSTAT_ERROR:
		activosTransformacion--;
		fallosEnTotal++;
		enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_ERROR,solicitud->bloque,itemTransformacion->nodo_id);
		break;
	case TRANSFORMACION_ERROR_PERMISOS:
		activosTransformacion--;
		fallosEnTotal++;
		//log_error(worker_error_log, "Se envia a Master el error al dar permisos de ejecucion al programa de transformacion");
		enviarResultadoTransformacionYama(socketYama,TRANSFORMACION_ERROR,solicitud->bloque,itemTransformacion->nodo_id);
		break;
	}
	gettimeofday(&end, NULL);
	double secs = timeval_diff(&end, &init);
	cantidadEtapasTranformacion++;
	tiempoAcumEtapasTransformacion += secs;
}

void enviarReduccionLocalWorker(void *args){
	uint32_t id_job = idJob;
	struct timeval init, end;
	gettimeofday(&init, NULL);
	item_reduccion_local *itemRedLocal = (item_reduccion_local*) args;

	solicitud_programa_reduccion_local* solicitud = malloc(sizeof(solicitud_programa_reduccion_local));
	strcpy(solicitud->programa_reduccion,basename(ruta_programa_reductor));
	char* filebuffer = fileToChar(ruta_programa_reductor);
	solicitud->programa = filebuffer;
	strcpy(solicitud->archivo_temporal_resultante,itemRedLocal->archivo_temporal_reduccion_local);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->cantidad_archivos_temp = itemRedLocal->cantidad_archivos_temp;
	solicitud->archivos_temporales = itemRedLocal->archivos_temporales_transformacion;

	/*printf("----------------\n");
	printf("La ruta del archivo resultante de reduccion local es: %s\n", solicitud->archivo_temporal_resultante);
	int aux;
	for(aux=0; aux<solicitud->cantidad_archivos_temp; aux++){
		printf("La ruta del archivo temporal %d de transformacion es: %s\n", aux, solicitud->archivos_temporales[aux].archivo_temp);
	}
	printf("La cantidad de archivos temporales de transformacion a reducir son: %d\n", solicitud->cantidad_archivos_temp);
	printf("La longitud del script de reduccion es: %d\n", solicitud->length_programa);
	printf("El nombre del script de reduccion es: %s\n", solicitud->programa_reduccion);
	printf("El contenido del script de reduccion es:\n\n%s\n", solicitud->programa);
	printf("----------------\n");*/

	char* serializado = serializarSolicitudProgramaReduccionLocal(solicitud);
	int len = getLong_SolicitudProgramaReduccionLocal(solicitud);

	//solicitud_programa_reduccion_local* deserializado = deserializarSolicitudProgramaReduccionLocal(serializado);
	//printf("programa = %s\n", deserializado->programa );

	int socketConn = conectarseA(itemRedLocal->ip_worker, itemRedLocal->puerto_worker);
	enviarInt(socketConn,PROCESO_MASTER);
	enviarMensajeSocketConLongitud(socketConn, ACCION_REDUCCION_LOCAL, serializado, len);

	printf("Esperando conexiones del worker con archivo temp: %s y %d archivos a reducir\n", solicitud->archivo_temporal_resultante, solicitud->cantidad_archivos_temp);

	int msgcode;
	recibirInt(socketConn, &msgcode);

	printf("Socket %d. Numero de mensaje: %d\n", socketConn, msgcode);

	if(id_job != idJob){
		return;
	}

	switch(msgcode){
	case REDUCCION_LOCAL_OK:
		activosReduccionLocal--;
		enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_OK,itemRedLocal->nodo_id);
		break;
	case REDUCCION_LOCAL_ERROR_CREACION:
		activosReduccionLocal--;
		fallosEnTotal++;
		enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,itemRedLocal->nodo_id);
		break;
	case REDUCCION_LOCAL_ERROR_ESCRITURA:
		activosReduccionLocal--;
		fallosEnTotal++;
		enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,itemRedLocal->nodo_id);
		break;
	case REDUCCION_LOCAL_ERROR_SYSTEM:
		activosReduccionLocal--;
		fallosEnTotal++;
		enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,itemRedLocal->nodo_id);
		break;
	case REDUCCION_LOCAL_ERROR_PERMISOS:
		activosReduccionLocal--;
		fallosEnTotal++;
		enviarResultadoReduccionLocalYama(socketYama,REDUCCION_LOCAL_ERROR,itemRedLocal->nodo_id);
		break;
	}
	gettimeofday(&end, NULL);
	double secs = timeval_diff(&end, &init);
	cantidadEtapasReduccionLocal++;
	tiempoAcumEtapasReduccionLocal += secs;

}

void enviarReduccionGlobalWorker(void *args){
	struct timeval init, end;
	gettimeofday(&init, NULL);
	solicitud_reduccion_global *solicitudRedGlobal = (solicitud_reduccion_global*) args;

	solicitud_programa_reduccion_global* solicitud = malloc(sizeof(solicitud_programa_reduccion_global));
	strcpy(&(solicitud->programa_reduccion),basename(ruta_programa_reductor));
	char* filebuffer = fileToChar(ruta_programa_reductor);
	solicitud->programa = filebuffer;
	strcpy(&(solicitud->archivo_temporal_resultante),solicitudRedGlobal->archivo_temporal_reduccion_global);
	solicitud->length_programa = strlen(filebuffer);
	solicitud->cantidad_workers = solicitudRedGlobal->item_cantidad;
	solicitud->workers = solicitudRedGlobal->workers;

	char* serializado = serializarSolicitudProgramaReduccionGlobal(solicitud);
	int len = getLong_SolicitudProgramaReduccionGlobal(solicitud);

	//solicitud_programa_reduccion_global* deserializado = deserializarSolicitudProgramaReduccionGlobal(serializado);
	//printf("programa = %s\n", deserializado->programa );

	int socketConn = conectarseA(solicitudRedGlobal->encargado_worker->ip_worker, solicitudRedGlobal->encargado_worker->puerto_worker);
	enviarInt(socketConn,PROCESO_MASTER);
	enviarMensajeSocketConLongitud(socketConn, ACCION_REDUCCION_GLOBAL, serializado, len);

	int msgcode;
	recibirInt(socketConn, &msgcode);

	switch(msgcode){
	case REDUCCION_GLOBAL_OK:
		enviarResultadoReduccionGlobalYama(socketYama,REDUCCION_GLOBAL_OK,solicitudRedGlobal->encargado_worker->nodo_id);
		break;
	case REDUCCION_GLOBAL_ERROR_CREACION:
		fallosEnTotal++;
		enviarResultadoReduccionGlobalYama(socketYama,REDUCCION_GLOBAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
		break;
	case REDUCCION_GLOBAL_ERROR_ESCRITURA:
		fallosEnTotal++;
		enviarResultadoReduccionGlobalYama(socketYama,REDUCCION_GLOBAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
		break;
	case REDUCCION_GLOBAL_ERROR_APAREO:
		fallosEnTotal++;
		enviarResultadoReduccionGlobalYama(socketYama,REDUCCION_GLOBAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
		break;
	case REDUCCION_GLOBAL_ERROR_SYSTEM:
		fallosEnTotal++;
		enviarResultadoReduccionGlobalYama(socketYama,REDUCCION_GLOBAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
		break;
	case REDUCCION_GLOBAL_ERROR_PERMISOS:
		fallosEnTotal++;
		enviarResultadoReduccionGlobalYama(socketYama,REDUCCION_GLOBAL_ERROR,solicitudRedGlobal->encargado_worker->nodo_id);
		break;
	}

	gettimeofday(&end, NULL);
	double secs = timeval_diff(&end, &init);
	cantidadEtapasReduccionGlobal += solicitudRedGlobal->item_cantidad;
	tiempoAcumEtapasReduccionGlobal += secs;
}

void procesarSolicitudTransformacion(int socket, int message_long, char* message){
	//solicitud_transformacion* solicitudTransfDeserializada = deserializar_solicitud_transformacion(message);
	item_transformacion* itemTransfDeserializada = deserializar_item_transformacion(message);
	int var;

	pthread_t threadSolicitudTransformacionWorker;
	activosTransformacion++;
	if(activosTransformacion > cantidadMayorTransformacion){
		cantidadMayorTransformacion = activosTransformacion;
	}
	int er1 = pthread_create(
			&threadSolicitudTransformacionWorker,
			NULL,
			enviarTransformacionWorker,
			(void*) (itemTransfDeserializada));
//	pthread_join(threadSolicitudTransformacionWorker, NULL);

}

void procesarSolicitudReduccionLocal(int socket, int message_long, char* message){
	//solicitud_reduccion_local* solicitudReducLocalDeserializado = deserializar_solicitud_reduccion_local(message);
	item_reduccion_local* itemReducLocalDeserializado = deserializar_item_reduccion_local(message);

	printf("----------------\n");
	printf("La ruta del archivo resultante de reduccion local es: %s\n", itemReducLocalDeserializado->archivo_temporal_reduccion_local);
	int aux;
	for(aux=0; aux<itemReducLocalDeserializado->cantidad_archivos_temp; aux++){
		printf(
				"La ruta del archivo temporal %d de transformacion es: %s\n",
				aux,
				itemReducLocalDeserializado->archivos_temporales_transformacion[aux].archivo_temp
				);
	}
	printf("La cantidad de archivos temporales de transformacion a reducir son: %d\n", itemReducLocalDeserializado->cantidad_archivos_temp);
	printf("El nombre del nodo es: %s\n", itemReducLocalDeserializado->nodo_id);
	printf("La ip del worker es: %s\n", itemReducLocalDeserializado->ip_worker);
	printf("El puerto del worker es: %d\n", itemReducLocalDeserializado->puerto_worker);
	printf("----------------\n");

	pthread_t threadSolicitudRedLocalWorker;
	//enviarReduccionLocalWorker((void*) itemReducLocalDeserializado);
	activosReduccionLocal++;
	if(activosReduccionLocal>cantidadMayorReduccionLocal){
		cantidadMayorReduccionLocal = activosReduccionLocal;
	}
	int er1 = pthread_create(&threadSolicitudRedLocalWorker, NULL,enviarReduccionLocalWorker,itemReducLocalDeserializado);
}

void procesarSolicitudReduccionGlobal(int socket, int message_long, char* message){
	solicitud_reduccion_global* solicitudReducLocalDeserializado = deserializar_solicitud_reduccion_global(message);
//	pthread_t threadSolicitudRedGlobalWorker;
	enviarReduccionGlobalWorker((void*) solicitudReducLocalDeserializado);
	//int er1 = pthread_create(&threadSolicitudRedGlobalWorker, NULL,enviarReduccionGlobalWorker,(void*) solicitudReducLocalDeserializado);
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
	enviarInt(socketConn,PROCESO_MASTER);
	enviarMensajeSocketConLongitud(socketConn, ACCION_ALMACENAMIENTO_FINAL, serializado, len);

	//TODO: enviar nodo id, ALMACENADO_FINAL_OK y ALMACENADO_FINAL_ERROR
	uint32_t termino;
	recibirInt(socketConn, &termino);
	int total_size = sizeof(char[NOMBRE_NODO]);
	char *serializedPackage = malloc(total_size);
	int offset = 0;
	serializarDato(serializedPackage,solicitudFinal->nodo_id,sizeof(char[NOMBRE_NODO]),&offset);
	enviarMensajeSocketConLongitud(socketYama, termino, serializedPackage, total_size);
}

void procesarSolicitudAlmacenadoFinal(int socket, int message_long, char* message){
	solicitud_almacenado_final* solicitudAlmacFinalDeserializado = deserializar_solicitud_almacenado_final(message);
	//enviarSolicitudFinalWorker((void*) solicitudAlmacFinalDeserializado);
	pthread_t threadSolicitudFinalWorker;
	int er1 = pthread_create(&threadSolicitudFinalWorker, NULL,enviarSolicitudFinalWorker,(void*) solicitudAlmacFinalDeserializado);
	//pthread_join(threadSolicitudFinalWorker, NULL);
}
