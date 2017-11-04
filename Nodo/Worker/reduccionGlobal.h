/*
 * reduccionGlobal.h
 *
 *  Created on: 4/11/2017
 *      Author: utnso
 */

#ifndef REDUCCIONGLOBAL_H_
#define REDUCCIONGLOBAL_H_

#include "funcionesWorker.h"

int reduccionGlobal(solicitud_programa_reduccion_global *);
void responderSolicitudRG(int, int);
void enviarArchivoTemp(solicitud_enviar_archivo_temp *);
void leerArchivoTemp(solicitud_leer_archivo_temp *);

#endif /* REDUCCIONGLOBAL_H_ */
