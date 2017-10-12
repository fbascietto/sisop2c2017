/*
 * funcionesWorker.h
 *
 *  Created on: 4/10/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "../../bibliotecas/protocolo.h"
#include <commons/config.h>

#ifndef FUNCIONESWORKER_H_
#define FUNCIONESWORKER_H_

typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;

t_config* infoConfig;
char* nombreNodo;
char* rutaNodo;
int puerto;
int numeroDeArchivoTemporal;

void iniciarWorker();
void *esperarConexionesMaster(void * args);
void responderSolicitud();
void transformar();

#endif /* FUNCIONESWORKER_H_ */

