/*
 * yama.c
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/sockets.c"
#include "funcionesyama.h"
#include "prePlanificacion.h"

void main() {

	socketFS=0;
	nodosConectados=list_create();
	valorBaseTemporal=0;
	rutaGlobal=0;
	jobsActivos=list_create();
	jobsFinalizados=list_create();

	//cargo config.txt
	inicializarConfigYama();

	socketFS = 0;
	int socketEscucha;
	fd_set fdSocketsEscucha;
	FD_ZERO(&fdSocketsEscucha);
	socketEscucha= escuchar(5100);
	FD_SET(socketEscucha, &fdSocketsEscucha);

	printf("pid de yama %d \nusarlo para enviar seniales\n", getpid());



	esperarConexion = malloc(sizeof(t_esperar_conexion));

	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;



	socketFS = conectarseA(fsIP, fsPort);
//	while(socketFS == 0){
//		socketFS = conectarseA(fsIP, fsPort);
//		sleep(3);
//	}

	enviarInt(socketFS,PROCESO_YAMA);

		//Espero conexión de procesos master

	esperarConexionMasterYFS((void*) esperarConexion);

}



