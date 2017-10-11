#include "funcionesyama.h"



void deserializarDato(void* dato, char* buffer, int size, int* offset){
	memcpy(dato,buffer + *offset,size);
	*offset += size;
}

void *esperarConexionMaster(void *args) {

	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;

	printf("Esperando conexiones en Yama...\n");

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------


		int nuevoSocket;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,
				argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);

		}

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
					case PROCESO_MASTER:
						recibirSolicitudMaster(nuevoSocket);
				}


			}
		}
}

void recibirSolicitudMaster(int nuevoSocket){
	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);
	printf("codigo de mensaje: %d\n",	package->msgCode);
	switch(package->msgCode){
		case ACCION_PROCESAR_ARCHIVO:
			procesarSolicitudArchivoMaster(nuevoSocket, package->message_long, package->message);
	}
}

void procesarSolicitudArchivoMaster(int nuevoSocket, uint32_t message_long, char* message){
	//en este caso recibo solo un string como mensaje, pero en caso de recibir un objeto serializado, hay que deserializarlo antes.
	printf("mensaje recibido: %s\n",message);
	printf("longitudmensaje: %d\n",message_long);
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









