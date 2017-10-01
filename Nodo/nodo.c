/*
 * nodo.c
 *
 *  Created on: 14/9/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "funcionesNodo.h"


void main() {

	pthread_t threadProcesoDataNode;

	int er1 = pthread_create(&threadProcesoDataNode,NULL,iniciarDataNode,NULL);

	pthread_join(threadProcesoDataNode, NULL);


}
