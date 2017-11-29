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
	socketFS = 0;
	int socketEscucha;
	fd_set fdSocketsEscucha;
	FD_ZERO(&fdSocketsEscucha);

	printf("pid de yama %d \nusarlo para enviar seniales\n", getpid());
	if (signal(SIGUSR1, recargarConfiguracion) == SIG_ERR)
		printf("\nerror agarrando la senial SIGUSR1\n");

	socketEscucha= escuchar(5500);
	FD_SET(socketEscucha, &fdSocketsEscucha);


	t_esperar_conexion *esperarConexion;

	/*
	hacerPedidoDeTransformacionYRL();
	printf("finaliza pedido transformacion y reduccion\n");
	 */
	//cargo config.txt
	inicializarConfigYama();


	socketFS = conectarseA(fsIP, fsPort);
//	while(socketFS == 0){
//		socketFS = conectarseA(fsIP, fsPort);
//		sleep(3);
//	}

	enviarInt(socketFS,PROCESO_YAMA);


	esperarConexion = malloc(sizeof(t_esperar_conexion));

	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;

	while(1){
		//Espero conexión de procesos master
		esperarConexionMasterYFS((void*) esperarConexion);
	}

}



