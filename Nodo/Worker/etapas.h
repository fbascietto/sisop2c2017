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

//RG (var globales)
char ruta_archivo_temp_final[LENGTH_RUTA_ARCHIVO_TEMP];
char ruta_archivo_temp_red_local[LENGTH_RUTA_ARCHIVO_TEMP];
solicitud_recibir_palabra* ultima_palabra;
int offset_lectura;
char* palabraCandidata;
int posicionCandidata;


//estructura que define los elementos que maneja el worker encargado en la reduccion global
typedef struct{

	t_worker* worker;	//worker al que representa este elemento
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
void prepararParaApareo(t_list*, t_worker*, int, char*);
int recorrerArchivo(char[LENGTH_RUTA_ARCHIVO_TEMP]);
int leerYEnviarArchivoTemp(char[LENGTH_RUTA_ARCHIVO_TEMP], int);
bool esMenor(char*, char*);
bool termino(void*);
void procesarElemento(void*);
int aparear(t_list*);
solicitud_recibir_palabra* recibirPalabra(int);
int escribirEnArchivo(char *);
char* contenido_de_archivo(char[LENGTH_RUTA_ARCHIVO_TEMP]);
void responderSolicitudRG(int, int);

//AF
void almacenamientoFinal(char*, int, char*);


#endif /* ETAPAS_H_ */
