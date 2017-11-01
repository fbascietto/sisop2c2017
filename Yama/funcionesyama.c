#include "funcionesyama.h"
#include "prePlanificacion.h"
#include "../bibliotecas/protocolo.h"



void deserializarDato(void* dato, char* buffer, int size, int* offset){
	memcpy(dato,buffer + *offset,size);
	*offset += size;
}


void *recibirMensajeMaster(void *args){
	t_esperar_mensaje *argumentos = (t_esperar_mensaje*) args;
	int nuevoSocket = argumentos->socketCliente;
	free(args);
	while(1){
		Package* package = createPackage();
		int leidos = recieve_and_deserialize(package, nuevoSocket);
		printf("codigo de mensaje: %d\n",	package->msgCode);
		switch(package->msgCode){
		case ACCION_PROCESAR_ARCHIVO:
			procesarSolicitudArchivoMaster(nuevoSocket, package->message_long, package->message);
			break;
		case RESULTADO_TRANSFORMACION:
			procesarResultadoTransformacion(nuevoSocket, package->message_long, package->message);
			break;
		case RESULTADO_REDUCCION_LOCAL:
			procesarResultadoReduccionLocal(nuevoSocket, package->message_long, package->message);
			break;
		case RESULTADO_REDUCCION_GLOBAL:
			procesarResultadoReduccionGlobal(nuevoSocket, package->message_long, package->message);
			break;
		case RESULTADO_ALMACENADO_FINAL:
			procesarResultadoAlmacenadoFinal(nuevoSocket, package->message_long, package->message);
			break;
		}
	}
}

void *esperarConexionMaster(void *args) {

	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;

	printf("Esperando conexiones en Yama...\n");

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------


	/*int nuevoSocket;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,
				argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);

		}*/

	while(1){
		//TODO: Recibir instrucciones master y crear thread por cada una
		int nuevoSocket = -1;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			//log_trace(logSockets,"Nuevo Socket!");
			printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);
			int cliente;
			recibirInt(nuevoSocket,&cliente);
			switch(cliente){
			//TODO: iniciar un hilo para manejar cliente
			case PROCESO_MASTER:
				while(1){
					pthread_t threadSolicitudesMaster;
					t_esperar_mensaje *tEsperarMensaje = malloc(sizeof(t_esperar_mensaje));
					tEsperarMensaje->socketCliente = nuevoSocket;
					int er1 = pthread_create(&threadSolicitudesMaster, NULL,recibirMensajeMaster,(void*) tEsperarMensaje);
					pthread_join(threadSolicitudesMaster, NULL);
				}
				break;
			}
		}
	}
}



solicitud_transformacion* obtenerSolicitudTrasnformacionMock(char* message){
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
	return solicitudTransformacion;
}

solicitud_reduccion_local* obtenerSolicitudReduccionLocalMock(char* message){
	item_reduccion_local* item1 = crearItemReduccionLocal(1,"127.0.0.1",8080,"/tmp/Master1-temp38","/tmp/Master1-temp38");
	item_reduccion_local* item2 = crearItemReduccionLocal(12,"127.23.0.1",0101,"/tmp/Master1-temp39","/tmp/Master1-Worker1");
	item_reduccion_local* item3 = crearItemReduccionLocal(137,"187.0.56.1",9090,"/tmp/Master1-temp44","/tmp/Master1-Worker2");

	solicitud_reduccion_local* solicitudReduccionLocal = malloc(sizeof(solicitud_reduccion_local));

	solicitudReduccionLocal->item_cantidad = 0;

	agregarItemReduccionLocal(solicitudReduccionLocal,item1);
	free(item1);
	agregarItemReduccionLocal(solicitudReduccionLocal,item2);
	free(item2);
	agregarItemReduccionLocal(solicitudReduccionLocal,item3);
	free(item3);
	return solicitudReduccionLocal;
}

solicitud_reduccion_global* obtenerSolicitudReduccionGlobalMock(char* message){
	item_reduccion_global* item1 = crearItemReduccionGlobal(1,"127.0.0.1",8080,"/tmp/Master1-temp38","/tmp/Master1-temp38",true);
	item_reduccion_global* item2 = crearItemReduccionGlobal(12,"127.23.0.1",0101,"/tmp/Master1-temp39","/tmp/Master1-Worker1",false);
	item_reduccion_global* item3 = crearItemReduccionGlobal(137,"187.0.56.1",9090,"/tmp/Master1-temp44","/tmp/Master1-Worker2",false);

	solicitud_reduccion_global* solicitudReduccionGlobal = malloc(sizeof(solicitud_reduccion_global));

	solicitudReduccionGlobal->item_cantidad = 0;

	agregarItemReduccionGlobal(solicitudReduccionGlobal,item1);
	free(item1);
	agregarItemReduccionGlobal(solicitudReduccionGlobal,item2);
	free(item2);
	agregarItemReduccionGlobal(solicitudReduccionGlobal,item3);
	free(item3);
	return solicitudReduccionGlobal;
}

solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinalMock(char* message){
	solicitud_almacenado_final *solicitud = malloc(sizeof(solicitud_almacenado_final));
	strcpy(solicitud->archivo_temporal_reduccion_global, "/tmp/Master1-final");
	solicitud->nodo_id = 2;
	solicitud->puerto_worker = 5555;
	strcpy(solicitud->ip_worker,"192.168.1.11");
	return solicitud;
}


/*
 * TO DO
 * no son la version final
 */
solicitud_transformacion* obtenerSolicitudTrasnformacion(t_job* job){//t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasTemporales){
	int i;
	t_estado* unEstado;
	char* rutaArchivoTemporal;
	int tamanioJob = list_size(job->estadosJob);

	// menos 1 porque esta el de reduccion global en la planificacion
	solicitud_transformacion* solicitud = malloc(sizeof(solicitud_transformacion));
	item_transformacion* item = malloc(sizeof(item_transformacion));

	for(i=0; i<tamanioJob ;i++){
		unEstado = list_get(job->estadosJob, i);

		if(strcmp(unEstado->etapa, "transformacion")==0){
			item = crearItemTransformacion(unEstado->nodoPlanificado->nodo->idNodo,
					unEstado->nodoPlanificado->nodo->ipWorker,
					unEstado->nodoPlanificado->nodo->puerto,
					unEstado->nodoPlanificado->bloque->numeroBloque,
					unEstado->nodoPlanificado->bloque->bytesOcupados,
					unEstado->archivoTemporal);
			agregarItemTransformacion(solicitud, item);
		}
	}
	return solicitud;
	//return obtenerSolicitudTrasnformacionMock(message);
}

solicitud_reduccion_local* obtenerSolicitudReduccionLocal(t_job* job){ // t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasTransformacionTemporales, char* rutaReduccion){

	int i;
	int j;
	int k;
	t_estado* unEstado;
	t_estado* estadoAux;
	archivo_temp* listaRutasTransformacion;
	char* rutaArchivoTemporal;
	int tamanioJob = list_size(job->estadosJob);

	solicitud_reduccion_local* solicitud = malloc(sizeof(solicitud_reduccion_local));
	item_reduccion_local* item = malloc(sizeof(item_reduccion_local));

	for(i=0; i<tamanioJob ;i++){
		unEstado = list_get(job->estadosJob, i);
		if(strcmp(unEstado->etapa, "reduccion local")==0){
			k=0;
			for(j=0; j < tamanioJob; j++){
				estadoAux = list_get(job->estadosJob, j);
				if(strcmp(estadoAux->etapa, "transformacion") == 0){
					listaRutasTransformacion = realloc(listaRutasTransformacion, sizeof(archivo_temp) * (k+1));
					strcpy(listaRutasTransformacion[k].archivo_temp, estadoAux->archivoTemporal);
					k++;
				}
			}
			item = crearItemReduccionLocal(unEstado->nodoPlanificado->nodo->idNodo,
					unEstado->nodoPlanificado->nodo->ipWorker,
					unEstado->nodoPlanificado->nodo->puerto,
					unEstado->archivoTemporal);
			item->archivos_temporales_transformacion = listaRutasTransformacion;
			agregarItemReduccionLocal(solicitud, item);

		}
	}
	return solicitud;
	//return obtenerSolicitudReduccionLocalMock(message);
}

solicitud_reduccion_global* obtenerSolicitudReduccionGlobal(t_job* job){ //t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasRedLocalTemporales, char* rutaReduccionGlobal){

	int i;
	t_estado* unEstado;
	char* rutaArchivoTemporal;
	int tamanioJob = list_size(job->estadosJob);

	solicitud_reduccion_global* solicitud = malloc(sizeof(solicitud_reduccion_global));
	item_reduccion_global* item = malloc(sizeof(item_reduccion_global));

	for(i=0; i<tamanioJob ;i++){
		unEstado = list_get(job->estadosJob, i);
		if(strcmp(unEstado->etapa, "reduccion local")==0){

		item = crearItemReduccionGlobal(unEstado->nodoPlanificado->nodo->idNodo,
				unEstado->nodoPlanificado->nodo->ipWorker,
				unEstado->nodoPlanificado->nodo->puerto,
				unEstado->archivoTemporal);
			agregarItemReduccionGlobal(solicitud, item);
		} else if (strcmp(unEstado->etapa, "reduccion global") == 0){

			item = crearItemReduccionGlobal(unEstado->nodoPlanificado->nodo->idNodo,
							unEstado->nodoPlanificado->nodo->ipWorker,
							unEstado->nodoPlanificado->nodo->puerto,
							NULL);
			strcpy(solicitud->archivo_temporal_reduccion_global, item->archivo_temporal_reduccion_local);
			solicitud->encargado_reduccion_global = item;
		}
	}
	return solicitud;
}

solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinal(t_job* job){ //t_nodo* nodoEncargado, int puerto_worker, char* ip_worker, char* rutaReduccionGlobal){

	int i;
	t_estado* unEstado;
	char* rutaArchivoTemporal;
	int tamanioJob = list_size(job->estadosJob);

	solicitud_almacenado_final* solicitud = malloc(sizeof(solicitud_almacenado_final));

	for(i=0; i<tamanioJob ;i++){
		unEstado = list_get(job->estadosJob, i);
		if (strcmp(unEstado->etapa, "reduccion global") == 0){
			solicitud->nodo_id = unEstado->nodoPlanificado->nodo->idNodo;
			solicitud->puerto_worker = unEstado->nodoPlanificado->nodo->puerto;
			strcpy(solicitud->ip_worker, unEstado->nodoPlanificado->nodo->ipWorker);
			strcpy(solicitud->archivo_temporal_reduccion_global, unEstado->archivoTemporal);
		}
	}


	return solicitud;
	//return obtenerSolicitudAlmacenadoFinalMock(message);
}



void procesarResultadoTransformacion(int nuevoSocket, uint32_t message_long, char* message){
	//solicitud_reduccion_local* solicitudTransformacion = obtenerSolicitudReduccionLocal(message);
	solicitud_reduccion_local* solicitudTransformacion = obtenerSolicitudReduccionLocalMock(message);
	char* solicitudSerializado = serializarSolicitudReduccionLocal(solicitudTransformacion);
	uint32_t longitud = getLong_SolicitudReduccionLocal(solicitudTransformacion);
	int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_REDUCCION_LOCAL,solicitudSerializado,longitud);
}

void procesarResultadoReduccionLocal(int nuevoSocket, uint32_t message_long, char* message){
	//solicitud_reduccion_local* solicitudReduccionGlobal = obtenerSolicitudReduccionGlobal(message);
	solicitud_reduccion_local* solicitudReduccionGlobal = obtenerSolicitudReduccionGlobalMock(message);
	char* solicitudSerializado = serializarSolicitudReduccionGlobal(solicitudReduccionGlobal);
	uint32_t longitud = getLong_SolicitudReduccionGlobal(solicitudReduccionGlobal);
	int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_REDUCCION_GLOBAL,solicitudSerializado,longitud);

}

void procesarResultadoReduccionGlobal(int nuevoSocket, uint32_t message_long, char* message){
	//solicitud_almacenado_final* solicitudAlmacenadoFinal = obtenerSolicitudAlmacenadoFinal(message);
	solicitud_almacenado_final* solicitudAlmacenadoFinal = obtenerSolicitudAlmacenadoFinalMock(message);
	char* solicitudSerializado = serializarSolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
	uint32_t longitud = getLong_SolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
	int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_ALMACENADO_FINAL,solicitudSerializado,longitud);

}

void procesarResultadoAlmacenadoFinal(int nuevoSocket, uint32_t message_long, char* message){
	//TODO: fin
}

void procesarSolicitudArchivoMaster(int nuevoSocket, uint32_t message_long, char* message){
	//solicitud_transformacion* solicitudTransformacion = obtenerSolicitudTrasnformacion(message);
	solicitud_transformacion* solicitudTransformacion = obtenerSolicitudTrasnformacionMock(message);
	char* solicitudTransfSerializado = serializarSolicitudTransformacion(solicitudTransformacion);
	uint32_t longitud = getLong_SolicitudTransformacion(solicitudTransformacion);
	int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_TRANSFORMACION,solicitudTransfSerializado,longitud);

	/*Espero resultado de Transformacion*/

}


void procesarSolicitudMaster(nuevoSocket){
	int protocolo;
	recibirInt(nuevoSocket,&protocolo);
	switch(protocolo){
	case ENVIAR_ARCHIVO_TEXTO:
		printf("Se recibio instruccion para recibir archivo de texto\n");
		recibirArchivo(nuevoSocket);
		break;
	}
}


/*
 * recibe un string y devuelve una ruta generada
 * la ruta es la direccion /tmp/ y el
 * nombre del archivo es un simple contador que siempre aumenta
 * por cada vez que pase por la funcion
 */
char* generarRutaTemporal(){
	rutaGlobal++;
	char ruta[50];
	sprintf(ruta, "/tmp/%d", rutaGlobal);
	return ruta;
}


void inicializarConfigYama(){
	infoConfig = config_create("../config.txt");

	if(config_has_property(infoConfig,"IP_FILESYSTEM")){
		fsIP = config_get_string_value(infoConfig,"IP_FILESYSTEM");
		printf("IP del filesystem: %s\n", fsIP);
	}

	if(config_has_property(infoConfig,"PUERTO_FILESYSTEM")){
		fsPort = config_get_int_value(infoConfig,"PUERTO_FILESYSTEM");
		printf("Puerto del filsystem: %d\n", fsPort);
	}

	if(config_has_property(infoConfig,"RETARDO_PLANIFICACION")){
		retardoPlanificacion = config_get_int_value(infoConfig,"RETARDO_PLANIFICACION");
		printf("Tiempo de retardo (en milisegundos: %d\n", retardoPlanificacion);
	}
	if(config_has_property(infoConfig,"ALGORITMO_BALANCEO")){
		algoritmoBalanceo = config_get_string_value(infoConfig,"ALGORITMO_BALANCEO");
		printf("Algoritmo de balanceo seleccionado: %s\n", algoritmoBalanceo);
	}
	if(config_has_property(infoConfig,"DISP_BASE")){
		dispBase = config_get_int_value(infoConfig,"DISP_BASE");
		printf("Disponibilidad Base: %d\n", dispBase);
	}
}
