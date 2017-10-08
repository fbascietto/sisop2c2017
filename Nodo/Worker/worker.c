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

	/*pthread_t threadProcesoWorker;

		int er1 = pthread_create(&threadProcesoWorker,NULL,iniciarWorker,NULL);
		pthread_join(threadProcesoWorker, NULL); */

	iniciarWorker();


}
