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
#include <commons/config.h>
#include "interfaceWorker.h"

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

void iniciarWorker();
void *esperarConexionesMaster(void *);
void recibirSolicitudMaster(int);
int transformacion(solicitud_programa_transformacion *);
int reduccionLocal(solicitud_programa_reduccion_local *);
int reduccionGlobal(solicitud_programa_reduccion_global *);

#endif /* FUNCIONESWORKER_H_ */

