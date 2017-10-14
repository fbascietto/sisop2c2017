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

void transformar(){

	//Nombrado de archivos lo hace Yama

	/*int master = 1;
	char* buffer = malloc(SIZE);   buffer que se utiliza para guardar lo leido y luego volcarlo en el archivo temporal
	char ruta[30];
	sprintf(ruta, "/tmp/Master%d-temp%d", master, numeroDeArchivoTemporal); creacion de la ruta de archivos temp
	FILE* fd = fopen(ruta,"w");
	fputs(buffer,fd);
	fclose(fd);
	free(buffer); */
}

void responderSolicitud(){

	//el forkeo va a ir en el switch dentro de recibirSolicitudMaster, no en esta funcion

	int status;
	pid_t pid = fork();
	if(pid < 0){
		perror("No se ha podido crear el proceso hijo\n");
	}else{
		if(pid == 0){
			//proceso hijo
			//realiza solicitud luego termina
			exit(0);
		}
	}
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

}


