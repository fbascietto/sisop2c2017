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

		t_esperar_conexion *esperarConexion;
		esperarConexion = malloc(sizeof(t_esperar_conexion));

		esperarConexionesMaster((void*)esperarConexion);

		}

void *esperarConexionesMaster(void *args){
	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;

	printf("Esperando conexiones de Master en Worker...\n");

	//Queda esperando conexiones de Master para atajar las solicitudes

	int nuevaConexion;
	int pid;

	while(1){
		nuevaConexion = esperarConexionesSocket(&argumentos->fdSocketEscucha, argumentos->socketEscucha);
		if (nuevaConexion != -1) {
				printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevaConexion);
				pid = fork();
				if(pid < 0){
					printf("No se pudo crear el proceso hijo\n");
				}else{
					if(pid == 0){
						//Aca va la solicitud que hace el proceso hijo
						exit(0);
						//exit porque mata el proceso hijo al terminar la solicitud
					}
				}

				}
	}
}


