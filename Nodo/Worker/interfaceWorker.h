/*
 * interfaceWorker.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "../../bibliotecas/estructuras.h"

#ifndef INTERFACEWORKER_H_
#define INTERFACEWORKER_H_

solicitud_programa_transformacion* deserializarSolicitudProgramaTransformacion(char*);
solicitud_programa_reduccion_local* deserializarSolicitudProgramaReduccionLocal(char*);
solicitud_programa_reduccion_global* deserializarSolicitudProgramaReduccionGlobal(char*);


#endif /* INTERFACEWORKER_H_ */
