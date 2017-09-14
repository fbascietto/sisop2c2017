/*
 * nodo.c
 *
 *  Created on: 14/9/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"

void main() {
	//leer config

	t_config* infoConfig;
	char* fsIP;
	int fsPort;
	char* nombreNodo;
	int socketConn;

	infoConfig = config_create("/home/utnso/yama/tp-2017-2c-sapnu-puas/Nodo/config.txt");

	if(config_has_property(infoConfig,"IP_FILESYSTEM")){
		fsIP = config_get_string_value(infoConfig,"IP_FILESYSTEM");
		printf("IP: %s\n", fsIP);
	}

	if(config_has_property(infoConfig,"PUERTO_FILESYSTEM")){
			fsPort = config_get_int_value(infoConfig,"PUERTO_FILESYSTEM");
			printf("Puerto: %d\n", fsPort);
	}

	if(config_has_property(infoConfig,"NOMBRE_NODO")){
			nombreNodo = config_get_string_value(infoConfig,"NOMBRE_NODO");
	}

	socketConn = conectarseA(fsIP, fsPort);
	enviarMensaje(socketConn, nombreNodo);



}
