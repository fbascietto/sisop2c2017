/*
 * funcionesWorker.c
 *
 *  Created on: 4/10/2017
 *      Author: utnso
 */

#include "funcionesWorker.h"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

void iniciarWorker(){

		infoConfig = config_create("config.txt");


			if(config_has_property(infoConfig,"NOMBRE_NODO")){
					nombreNodo = config_get_string_value(infoConfig,"NOMBRE_NODO");
					nombreNodo[strlen(nombreNodo)+1]='\0';
			}

			if(config_has_property(infoConfig,"NOMBRE_NODO")){
					rutaNodo = config_get_string_value(infoConfig,"RUTA_DATABIN");
					rutaNodo[strlen(rutaNodo)+1]='\0';
			}

			if(config_has_property(infoConfig, "PUERTO_WORKER")){
					puerto = config_get_int_value(infoConfig, "PUERTO_WORKER");
			}

		}
