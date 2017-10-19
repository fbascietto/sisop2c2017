/*
 * nodo.c
 *
 *  Created on: 14/9/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include "../../bibliotecas/sockets.c"
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"
#include "funcionesWorker.h"


void main() {

	iniciarWorker();

	int socketEscucha;
		fd_set fdSocketsEscucha;
		FD_ZERO(&fdSocketsEscucha);


		socketEscucha= escuchar(4002);
		FD_SET(socketEscucha, &fdSocketsEscucha);


		t_esperar_conexion *esperarConexion;

		esperarConexion = malloc(sizeof(t_esperar_conexion));

		esperarConexion->fdSocketEscucha = fdSocketsEscucha;
		esperarConexion->socketEscucha = socketEscucha;

		while(1){

			esperarConexionesMaster((void*) esperarConexion);

		}

}
