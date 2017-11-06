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
	int socketEscucha;
	fd_set fdSocketsEscucha;
	FD_ZERO(&fdSocketsEscucha);


	socketEscucha= escuchar(4004);
	FD_SET(socketEscucha, &fdSocketsEscucha);


	t_esperar_conexion *esperarConexion;

/*
	hacerPedidoDeTransformacionYRL();
	printf("finaliza pedido transformacion y reduccion\n");
*/
	//cargo config.txt
	//inicializarConfigYama();

	ejemploPrePlanificacion();

	esperarConexion = malloc(sizeof(t_esperar_conexion));

	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;

	//Espero conexión de procesos master
	esperarConexionMaster((void*) esperarConexion);



}



