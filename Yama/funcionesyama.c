#include "funcionesyama.h"
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
	return solicitudTransformacion;
}

solicitud_reduccion_local* obtenerSolicitudReduccionLocalMock(char* message){
	item_reduccion_local* item1 = crearItemReduccionLocal(1,"127.0.0.1:8080","/tmp/Master1-temp38","/tmp/Master1-temp38");
	item_reduccion_local* item2 = crearItemReduccionLocal(12,"127.23.0.1:0101","/tmp/Master1-temp39","/tmp/Master1-Worker1");
	item_reduccion_local* item3 = crearItemReduccionLocal(137,"187.0.56.1:9090","/tmp/Master1-temp44","/tmp/Master1-Worker2");

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
	item_reduccion_global* item1 = crearItemReduccionGlobal(1,"127.0.0.1:8080","/tmp/Master1-temp38","/tmp/Master1-temp38",true);
	item_reduccion_global* item2 = crearItemReduccionGlobal(12,"127.23.0.1:0101","/tmp/Master1-temp39","/tmp/Master1-Worker1",false);
	item_reduccion_global* item3 = crearItemReduccionGlobal(137,"187.0.56.1:9090","/tmp/Master1-temp44","/tmp/Master1-Worker2",false);

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
	strcpy(solicitud->ip_puerto_worker,"192.168.1.11:5555");
	return solicitud;
}

solicitud_transformacion* obtenerSolicitudTrasnformacion(char* message){
	//TODO: obtener desde la pre-planificacion
	return obtenerSolicitudTrasnformacionMock(message);
}

solicitud_reduccion_local* obtenerSolicitudReduccionLocal(char* message){
	//TODO: obtener desde la pre-planificacion
	return obtenerSolicitudReduccionLocalMock(message);
}

solicitud_reduccion_global* obtenerSolicitudReduccionGlobal(char* message){
	//TODO: obtener desde la pre-planificacion
	return obtenerSolicitudReduccionGlobalMock(message);
}

solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinal(char* message){
	//TODO: obtener desde la pre-planificacion
	return obtenerSolicitudAlmacenadoFinalMock(message);
}

void procesarResultadoTransformacion(int nuevoSocket, uint32_t message_long, char* message){
	solicitud_reduccion_local* solicitudTransformacion = obtenerSolicitudReduccionLocal(message);
	char* solicitudSerializado = serializarSolicitudReduccionLocal(solicitudTransformacion);
	uint32_t longitud = getLong_SolicitudReduccionLocal(solicitudTransformacion);
	int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_REDUCCION_LOCAL,solicitudSerializado,longitud);
}

void procesarResultadoReduccionLocal(int nuevoSocket, uint32_t message_long, char* message){
	solicitud_reduccion_local* solicitudReduccionGlobal = obtenerSolicitudReduccionGlobal(message);
	char* solicitudSerializado = serializarSolicitudReduccionGlobal(solicitudReduccionGlobal);
	uint32_t longitud = getLong_SolicitudReduccionGlobal(solicitudReduccionGlobal);
	int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_REDUCCION_GLOBAL,solicitudSerializado,longitud);

}

void procesarResultadoReduccionGlobal(int nuevoSocket, uint32_t message_long, char* message){
	solicitud_almacenado_final* solicitudAlmacenadoFinal = obtenerSolicitudAlmacenadoFinal(message);
	char* solicitudSerializado = serializarSolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
	uint32_t longitud = getLong_SolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
	int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_ALMACENADO_FINAL,solicitudSerializado,longitud);

}

void procesarResultadoAlmacenadoFinal(int nuevoSocket, uint32_t message_long, char* message){
	//TODO: fin
}

void procesarSolicitudArchivoMaster(int nuevoSocket, uint32_t message_long, char* message){
	solicitud_transformacion* solicitudTransformacion = obtenerSolicitudTrasnformacion(message);
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

void inicializarConfigYama(){
	infoConfig = config_create("../config.txt");
}
