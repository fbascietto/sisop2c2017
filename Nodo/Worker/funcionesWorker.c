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

#define SIZE 1024 //use el valor del ejemplo, pero aca iria la cantidad que lee el buffer...

void iniciarWorker(){

//--------------WORKER LEE ARCHIVO DE CONFIGURACION--------------------

	numeroDeArchivoTemporal = 0; /*para contar la cantidad de archivos temporales para cuando se crea el nombre de dichos
								archivos*/

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

	//esta funcion esta muy incompleta obviamente...solo esta la salida no estandar

	int master = 1; /*el numero de Master tendria que obtenerse de manera distinta dado
													que no es una variable local sino que depende de donde viene la
													conexion*/
	char* buffer = malloc(SIZE); //buffer que se utiliza para guardar lo leido y luego volcarlo en el archivo temporal
	char ruta[30];
	sprintf(ruta, "/tmp/Master%d-temp%d", master, numeroDeArchivoTemporal); /*creacion de la ruta de los archivos temporales
																de transformacion*/
	FILE* fd = fopen(ruta,"w");
	fputs(buffer,fd);
	fclose(fd);
	free(buffer); //
}

void responderSolicitud(){

	int status;
	pid_t pid = fork();
	if(pid < 0){
		perror("No se ha podido crear el proceso hijo\n");
	}else{
		if(pid == 0){
			//proceso hijo
			transformar();
			//realiza solicitud luego termina
			exit(0);
		}else{
			//proceso padre
			waitpid(pid,&status,0); /*matazombies, no creo que se pueda usar esto igual porque el proceso
									padre se queda esperando a que termine el hijo*/
		}
	}
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

				//de aca para abajo es lo agregado de mi solucion, borrar si usamos la solucion de Mariano

				numeroDeArchivoTemporal++;
				responderSolicitud();


			}

}


