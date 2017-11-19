/*
 * etapas.h
 *
 *  Created on: 5/11/2017
 *      Author: utnso
 */

#include "../../bibliotecas/estructuras.h"
#include <semaphore.h>

#ifndef ETAPAS_H_
#define ETAPAS_H_

//semaforo para el recorrido de los archivos temporales de reduccion local para enviarle al worker encargado de a una linea
sem_t sem;

//estructura que define los elementos que maneja el worker encargado en la reduccion global
typedef struct{

	item_programa_reduccion_global* worker;	//worker al que representa este elemento
	bool pedir;		//booleano que determina si hay que pedirle el proximo elemento a este worker
	char* ultima_palabra;	//ultima palabra enviada por este worker al worker encargado
	bool fin; //booleano que determina si ya recorrio por completo su archivo temporal el worker

} t_elemento;

typedef struct{

char* palabraCandidata;

} t_palabra;

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
