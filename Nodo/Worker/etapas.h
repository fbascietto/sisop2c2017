/*
 * etapas.h
 *
 *  Created on: 5/11/2017
 *      Author: utnso
 */

#include "../../bibliotecas/estructuras.h"

#ifndef ETAPAS_H_
#define ETAPAS_H_

//T
int transformacion(solicitud_programa_transformacion *, char *);
void responderSolicitudT(int, int);
//RL
int reduccionLocal(solicitud_programa_reduccion_local *);
void responderSolicitudRL(int, int);
//RG
int reduccionGlobal(solicitud_programa_reduccion_global *, int);
void responderSolicitudRG(int, int);
int leerYEnviarArchivoTemp(solicitud_leer_y_enviar_archivo_temp *, int);
void habilitarSemaforo();
void recibirArchivoTemp(solicitud_recibir_archivo_temp *);
//AF
void almacenamientoFinal(char*, int, char*);


#endif /* ETAPAS_H_ */
