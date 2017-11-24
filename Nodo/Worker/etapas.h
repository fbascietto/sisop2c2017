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

//ruta del archivo temporal de reduccion global
char ruta_archivo_temp_final[LENGTH_RUTA_ARCHIVO_TEMP];

solicitud_recibir_palabra* ultima_palabra;

//semaforo para el recorrido del archivo que se encuentra en el worker encargado
sem_t sem;

//estructura que define los elementos que maneja el worker encargado en la reduccion global
typedef struct{

	t_worker worker;	//worker al que representa este elemento
	bool pedir;		//booleano que determina si hay que pedirle el proximo elemento a este worker
	char* ultima_palabra;	//ultima palabra enviada por este worker al worker encargado
	bool fin; //booleano que determina si ya recorrio por completo su archivo temporal el worker
	int socket; 	//socket de conexion entre worker encargado y este worker
	int posicion; //posicion en la lista

} t_elemento;


//T
int transformacion(solicitud_programa_transformacion *, char *);
void responderSolicitudT(int, int);

//RL
int reduccionLocal(solicitud_programa_reduccion_local *);
void responderSolicitudRL(int, int);

//RG
int reduccionGlobal(solicitud_programa_reduccion_global *, char*);
int recorrerArchivo(char[LENGTH_RUTA_ARCHIVO_TEMP]);
int leerYEnviarArchivoTemp(char[LENGTH_RUTA_ARCHIVO_TEMP], int);
solicitud_recibir_palabra* recibirPalabra(int);
void escribirEnArchivo(char *);
bool esMenor(char*, char*);
void aparear(t_list*);
void prepararParaApareo(t_list*, t_worker, int, char*);
void habilitarSemaforo();
char* contenido_de_archivo(char[LENGTH_RUTA_ARCHIVO_TEMP]);
void responderSolicitudRG(int, int);

//AF
void almacenamientoFinal(char*, int, char*);


#endif /* ETAPAS_H_ */
