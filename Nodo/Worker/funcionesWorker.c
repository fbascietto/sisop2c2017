/*
 * funcionesWorker.c
 *
 *  Created on: 4/10/2017
 *      Author: utnso
 */

#include "funcionesWorker.h"
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <sys/types.h>

void iniciarWorker(){

//--------------WORKER LEE ARCHIVO DE CONFIGURACION--------------------

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

//---------------ESPERA CONEXIONES-------------------------------



		}

void *esperarConexionesMaster(void * args){

	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;


		// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------//
			printf("Esperando conexiones de Master en Worker...\n");

			int nuevoSocket;
			nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,
					argumentos->socketEscucha);

			if (nuevoSocket != -1) {
				printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);

			}


}


