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
int persistirPrograma(char*, char*, char*);
void *esperarConexionesMaster(void *);
void recibirSolicitudMaster(int);
void recibirSolicitudWorker(int);
int transformacion(solicitud_programa_transformacion *);
void responderSolicitudT(int);
int reduccionLocal(solicitud_programa_reduccion_local *);
void responderSolicitudRL(int);
int reduccionGlobal(solicitud_programa_reduccion_global *);
void responderSolicitudRG(int);
void enviarArchivoTemp(solicitud_enviar_archivo_temp *);
void leerArchivoTemp(solicitud_leer_archivo_temp *);

#endif /* FUNCIONESWORKER_H_ */

