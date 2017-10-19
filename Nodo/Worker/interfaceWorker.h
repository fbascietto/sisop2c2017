/*
 * interfaceWorker.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "../../bibliotecas/estructuras.h"

#ifndef INTERFACEWORKER_H_
#define INTERFACEWORKER_H_

/*Funciones de serializacion para comunicacion Master con Worker*/

solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char*);
solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char*);
solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char*);

/*Funciones de serializacion para comunicacion Worker con Worker*/

solicitud_enviar_archivo_temp* deserializarSolicitudEnviarArchivoTemp(char*);
solicitud_leer_archivo_temp* deserializarSolicitudLeerArchivoTemp(char*);

#endif /* INTERFACEWORKER_H_ */
