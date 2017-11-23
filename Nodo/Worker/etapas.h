/*
 * etapas.h
 *
 *  Created on: 5/11/2017
 *      Author: utnso
 */

#include "../../bibliotecas/estructuras.h"
#include <commons/collections/list.h>
#include <semaphore.h>

#ifndef ETAPAS_H_
#define ETAPAS_H_

#define VALOR_SOCKET_WE -2

//semaforo para el recorrido de los archivos temporales de reduccion local para enviarle al worker encargado de a una linea
sem_t sem;

//estructura que define los elementos que maneja el worker encargado en la reduccion global
typedef struct{

	t_worker worker;	//worker al que representa este elemento
	bool pedir;		//booleano que determina si hay que pedirle el proximo elemento a este worker
	char* ultima_palabra;	//ultima palabra enviada por este worker al worker encargado
	bool fin; //booleano que determina si ya recorrio por completo su archivo temporal el worker
	int socket; 	//socket de conexion entre worker encargado y este worker
	int posicion;

} t_elemento;

typedef struct{

char* palabra;
bool fin_de_archivo;

} t_palabra;

//T
int transformacion(solicitud_programa_transformacion *, char *);
void responderSolicitudT(int, int);
//RL
int reduccionLocal(solicitud_programa_reduccion_local *);
void responderSolicitudRL(int, int);
//RG
int reduccionGlobal(solicitud_programa_reduccion_global *);
void responderSolicitudRG(int, int);
int leerYEnviarArchivoTemp(char[LENGTH_RUTA_ARCHIVO_TEMP], int);
t_palabra recibirPalabra();
void escribirEnArchivo(char *);
void aparear(t_list*);
void recibirArchivoTemp(solicitud_recibir_archivo_temp *);
void prepararEstructuras(t_list*, t_worker, int);
//void habilitarSemaforo();
//AF
void almacenamientoFinal(char*, int, char*);


#endif /* ETAPAS_H_ */
