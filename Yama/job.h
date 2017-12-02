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

void crearJob(t_list* bloques, t_list* listaNodos, char* tipoAlgoritmo, int idMaster);
void generarEstados(t_list* unaPreplanificacion, t_job* job);
t_estado* crearEstadoTransformacion(t_planificacion* unNodoPlanificado);
t_estado* crearEstadoReduccionLocal(t_planificacion* unNodoPlanificado, char* rutaReduccionLocal);
t_estado* crearEstadoReduccionGlobal(t_planificacion* unNodoPlanificado);
t_estado* crearEstadoAlmacenadoFinal(t_planificacion* unNodoPlanificado);
t_list* crearEstadosReduccionesLocales(t_list* unaPreplanificacion);
char* obtenerIdNodo(t_planificacion* nodoPlanificado);
t_job* obtenerJob(int idJob, t_list* jobs);
int obtenerIdJob(int idMaster, t_list* jobs);
int obtenerPosicionJob(int idJob, t_list* jobs);
void enProcesoSiguienteEtapa(char* idWorker, int etapa, t_job* job);
t_list* falloWorker(int bloque, char* etapa, int idWorker, t_job* job);
void cambiarEtapaOk(int idWorker, char* etapa, int bloque, t_job* job);
void respuestaWorker(char* respuesta, int bloque, char* etapa, int idWorker, int idJob);

t_list* obtenerEInicializarNodosDeBloques(t_list* bloques);
t_nodo* inicializarNodo(t_bloque* bloque);
bool estaElNodo(char* id, t_list* nodos);
void setearNodos(t_list* nodos);
bool estaConectado(t_nodo* unNodo);
t_nodo* obtenerNodoConectado(t_nodo* unNodo);
void actualizar(t_nodo* nodo, t_nodo* nodoConectado);

#endif /* JOB_H_ */
