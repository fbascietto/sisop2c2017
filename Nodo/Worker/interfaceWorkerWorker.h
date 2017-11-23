/*
 * interfaceWorkerWorker.h
 *
 *  Created on: 23/11/2017
 *      Author: utnso
 */

#include "../../bibliotecas/estructuras.h"

#ifndef INTERFACEWORKERWORKER_H_
#define INTERFACEWORKERWORKER_H_

solicitud_leer_y_enviar_archivo_temp* deserializarSolicitudLeerYEnviarArchivoTemp(char* message);
solicitud_recibir_palabra* deserializarSolicitudRecibirPalabra(char* message);

#endif /* INTERFACEWORKERWORKER_H_ */
