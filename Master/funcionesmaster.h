/*
 * funcionesmaster.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <libgen.h>
#include <stdint.h>

#ifndef MASTER_FUNCIONESMASTER_H_
#define MASTER_FUNCIONESMASTER_H_

char* ruta_programa_transformador;
char* ruta_programa_reductor;
char* ruta_archivo_del_job;
char* ruta_archivo_final_fs;
char* ruta_archivo_log;
int socketYama;
struct timeval t_ini, t_fin;

int activosTransformacion;
int cantidadEtapasTranformacion;
int cantidadEtapasReduccionLocal;
int activosReduccionLocal;
int cantidadEtapasReduccionGlobal;

double tiempoAcumEtapasTransformacion;
double tiempoAcumEtapasReduccionLocal;
double tiempoAcumEtapasReduccionGlobal;

int fallosEnTotal;

int cantidadMayorTransformacion;
int cantidadMayorReduccionLocal;

double timeval_diff(struct timeval *a, struct timeval *b);

uint32_t idJob;//todo: global

#endif /* MASTER_FUNCIONESMASTER_H_ */
