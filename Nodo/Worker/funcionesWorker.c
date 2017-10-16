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
#include <sys/types.h>
#include "interfaceWorker.h"


//#define SIZE 1024   YAMA hace nombrado de archivos temp

void iniciarWorker(){

//--------------WORKER LEE ARCHIVO DE CONFIGURACION--------------------

	//numeroDeArchivoTemporal = 0;   //nombrado de archivos lo hace YAMA

	infoConfig = config_create("../config.txt");


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

int transformacion(solicitud_programa_transformacion* solicitudDeserializada){

	return 0;
}

int reduccionLocal(solicitud_programa_reduccion_local* solicitudDeserializada){

	return 0;
}

int reduccionGlobal(solicitud_programa_reduccion_global* solicitudDeserializada){

	return 0;
}

void *esperarConexionesMaster(void *args) {

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
	int exit_code;
	pid_t pid = fork();
	if(pid == 0){
		//proceso hijo continua con la solicitud
		switch(package->msgCode){
			case ACCION_TRANSFORMACION:
				; //empty statement. Es solucion a un error que genera el lenguaje C
				solicitud_programa_transformacion* solicitudTDeserializada =
							deserializarSolicitudProgramaTransformacion(package->message);
				exit_code = transformacion(solicitudTDeserializada);
				break;
			case ACCION_REDUCCION_LOCAL:
				; //empty statement. Es solucion a un error que genera el lenguaje C
				solicitud_programa_reduccion_local* solicitudRLDeserializada =
							deserializarSolicitudProgramaReduccionLocal(package->message);
				exit_code = reduccionLocal(solicitudRLDeserializada);
				break;
			case ACCION_REDUCCION_GLOBAL:
				; //empty statement. Es solucion a un error que genera el lenguaje C
				solicitud_programa_reduccion_global* solicitudRGDeserializada =
							deserializarSolicitudProgramaReduccionGlobal(package->message);
				exit_code = reduccionGlobal(solicitudRGDeserializada);
				break;
		}
		exit(0);
	}else{
		if(pid < 0){
			//cuando no pudo crear el hijo
			perror("No se ha podido crear el proceso hijo\n");
		}else{
			//lo que haria el padre si es que necesitamos que haga algo
		}

	}
}

