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

		//Dejo comentado la solucion de Mariano, descomentar si nos quedamos con esa

		/*pid_t pid;
		pid = fork();

		switch(pid)
			{
				case -1: // Si pid es -1 quiere decir que ha habido un error
					perror("No se ha podido crear el proceso hijo\n");
					break;

					case 0: // Cuando pid es cero quiere decir que es el proceso hijo*/
						esperarConexionesMaster((void*) esperarConexion);
						/*break;

					default: // Cuando es distinto de cero es el padre
						iniciarWorker();
						printf("worker iniciado");
						break;
			}*/
	}
}
