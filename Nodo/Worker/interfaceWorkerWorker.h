/*
 * interfaceWorkerWorker.h
 *
 *  Created on: 23/11/2017
 *      Author: utnso
 */

#include "interface.h"

#ifndef INTERFACEWORKERWORKER_H_
#define INTERFACEWORKERWORKER_H_

uint32_t getLong_SolicitudRecibirPalabra(solicitud_recibir_palabra*);
char* serializarSolicitudRecibirPalabra(solicitud_recibir_palabra*);

solicitud_leer_y_enviar_archivo_temp* deserializarSolicitudLeerYEnviarArchivoTemp(char* serialized);
solicitud_recibir_palabra* deserializarSolicitudRecibirPalabra(char* serialized);

#endif /* INTERFACEWORKERWORKER_H_ */
