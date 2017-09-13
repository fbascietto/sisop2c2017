/*
 * master.c
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include "funcionesmaster.h"
#include <stdio.h>
#include <string.h>
#include <commons/config.h>

void main() {
	//leer config

	t_config* infoConfig;
	char* yamaIP;
	int yamaPort;

	infoConfig = config_create("/home/utnso/yama/tp-2017-2c-sapnu-puas/Master/config.txt");

	if(config_has_property(infoConfig,"YAMA_IP")){
		yamaIP = config_get_string_value(infoConfig,"YAMA_IP");
		printf("IP: %s\n", yamaIP);
	}

	if(config_has_property(infoConfig,"YAMA_PUERTO")){
			yamaPort = config_get_int_value(infoConfig,"YAMA_PUERTO");
			printf("Puerto: %d\n", yamaPort);
	}
}

