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

#include "funcionesyama.h"

void mainYama() {
	int socketEscucha;
	fd_set fdSocketsEscucha;
	FD_ZERO(&fdSocketsEscucha);


	socketEscucha= escuchar(4001);
	FD_SET(socketEscucha, &fdSocketsEscucha);

	pthread_t threadEsperaConexiones;

	t_esperar_conexion *esperarConexion;

	esperarConexion = malloc(sizeof(t_esperar_conexion));

	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;

	//Espero conexión de procesos master
	int er1 = pthread_create(&threadEsperaConexiones, NULL,
			esperarConexionesYama,
					(void*) esperarConexion);

	while(1);
}



