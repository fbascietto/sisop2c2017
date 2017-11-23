/*
 * funcionesWorker.h
 *
 *  Created on: 4/10/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "../../bibliotecas/protocolo.h"
#include "../../bibliotecas/sockets.h"
#include "interfaceWorkerMaster.h"
#include "interfaceWorkerWorker.h"
#include <commons/config.h>
#include <commons/string.h>
#include "etapas.h"


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
int puerto_fs;
char* IP_fs;

void iniciarWorker();
int persistirPrograma(char*, char*);
void *esperarConexionesMasterYWorker(void *);
void recibirSolicitudMaster(int);
void recibirSolicitudWorker(int);

#endif /* FUNCIONESWORKER_H_ */

