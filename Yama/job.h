/*
 * job.h
 *
 *  Created on: 3/11/2017
 *      Author: utnso
 */

#ifndef JOB_H_
#define JOB_H_

#include "../bibliotecas/estructuras.h"
#include "prePlanificacion.h"
#include "funcionesyama.h"

t_job* crearJob(t_list* bloques, t_list* listaNodos, char* tipoAlgoritmo, int idMaster);
void generarEstados(t_list* unaPreplanificacion, t_job* job);
t_estado* crearEstadoTransformacion(t_planificacion* unNodoPlanificado);
t_estado* crearEstadoReduccionLocal(t_planificacion* unNodoPlanificado);
t_estado* crearEstadoReduccionGlobal(t_planificacion* unNodoPlanificado);
t_job* obtenerJob(int idJob, t_list* jobs);
void enProcesoSiguienteEtapa(char* idWorker, char* etapa, t_job* job);
t_list* falloWorker(int bloque, char* etapa, int idWorker, t_job* job);
void cambiarEtapaOk(int idWorker, char* etapa, int bloque, t_job* job);
void respuestaWorker(char* respuesta, int bloque, char* etapa, int idWorker, int idJob);

#endif /* JOB_H_ */
