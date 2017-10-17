/*
 * master.c
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include "funcionesmaster.h"
#include "interface.h"
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"

#define PROCESO_MASTER 4

void main() {
	//testSerializarSolicitudTrasnformacion();
	//testSerializarItemTransformacion();
	//testSerializarSolicitudReduccionLocal();
	//testSerializarSolicitudReduccionGlobal();
	//testSerializarSolicitudAlmacenadoFinal();

	t_config* infoConfig;
	char* yamaIP;
	char* nombreNodo;
	int yamaPort;
	int socketConn;

	infoConfig = config_create("config.txt");

	if(config_has_property(infoConfig,"YAMA_IP")){
		yamaIP = config_get_string_value(infoConfig,"YAMA_IP");
		printf("IP: %s\n", yamaIP);
	}

	if(config_has_property(infoConfig,"YAMA_PUERTO")){
		yamaPort = config_get_int_value(infoConfig,"YAMA_PUERTO");
		printf("Puerto: %d\n", yamaPort);
	}
	if(config_has_property(infoConfig,"NOMBRE_MASTER")){
		nombreNodo = config_get_string_value(infoConfig,"NOMBRE_MASTER");
		// printf("Nombre: %s\n", nombreNodo);
		nombreNodo[strlen(nombreNodo)+1]='\0';
	}

	//TODO: crear un hilo para manejar esta comunicacion con Yama.
	socketConn = conectarseA(yamaIP, yamaPort);

	enviarInt(socketConn,PROCESO_MASTER);
	char* archivoMensage = "sent.txt";
	int len = strlen(archivoMensage);
	uint32_t message_long = sizeof(char)*len;
	enviarMensajeSocketConLongitud(socketConn,ACCION_PROCESAR_ARCHIVO,archivoMensage,len);
	while(1){
		Package* package = createPackage();
		printf("esperando mensaje de yama: %d\n",socketConn);
		int leidos = recieve_and_deserialize(package, socketConn);
		printf("codigo de mensaje: %d\n",	package->msgCode);
		switch(package->msgCode){
			case ACCION_PROCESAR_TRANSFORMACION:
				procesarSolicitudTransformacion(socketConn, package->message_long, package->message);
				enviarMensajeSocketConLongitud(socketConn,RESULTADO_TRANSFORMACION,archivoMensage,len);
				break;
			case ACCION_PROCESAR_REDUCCION_LOCAL:
				procesarSolicitudReduccionLocal(socketConn, package->message_long, package->message);
				enviarMensajeSocketConLongitud(socketConn,RESULTADO_REDUCCION_LOCAL,archivoMensage,len);
				break;
			case ACCION_PROCESAR_REDUCCION_GLOBAL:
				procesarSolicitudReduccionGlobal(socketConn, package->message_long, package->message);
				enviarMensajeSocketConLongitud(socketConn,RESULTADO_REDUCCION_GLOBAL,archivoMensage,len);
				break;
			case ACCION_PROCESAR_ALMACENADO_FINAL:
				procesarSolicitudAlmacenadoFinal(socketConn, package->message_long, package->message);
				enviarMensajeSocketConLongitud(socketConn,RESULTADO_ALMACENADO_FINAL,archivoMensage,len);
				break;
		}
	}
}

