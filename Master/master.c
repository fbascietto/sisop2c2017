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

	socketConn = conectarseA(yamaIP, yamaPort);
	enviarInt(socketConn,PROCESO_MASTER);
	// enviarMensaje(socketConn, nombreNodo);
	envioArchivo(socketConn,"sent.txt");

}


