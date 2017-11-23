/*
 * interfaceWorker.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "../bibliotecas/estructuras.h"
#include "interface.h"

#ifndef INTERFACEWORKER_H_
#define INTERFACEWORKER_H_

/*Funciones de serializacion para comunicacion Master con Worker*/

uint32_t getLong_SolicitudProgramaTransformacion(solicitud_programa_transformacion* solicitud);
char* serializarSolicitudProgramaTransformacion(solicitud_programa_transformacion* solicitud);
solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char* serialized);


uint32_t getLong_SolicitudProgramaReduccionLocal(solicitud_programa_reduccion_local* solicitud);


char* serializarSolicitudProgramaReduccionLocal(solicitud_programa_reduccion_local* solicitud);

solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char* serialized);

uint32_t getLong_SolicitudProgramaReduccionGlobal(solicitud_programa_reduccion_global* solicitud);

char* serializarSolicitudProgramaReduccionGlobal(solicitud_programa_reduccion_global* solicitud);
solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char* serialized);


#endif /* INTERFACEWORKER_H_ */
