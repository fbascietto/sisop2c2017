/*
 * funcionesyama.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <commons/config.h>
#include <string.h>
#include <commons/collections/list.h>
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "../Master/interface.h"
#include <stdint.h>

#ifndef YAMA_FUNCIONESYAMA_H_
#define YAMA_FUNCIONESYAMA_H_

typedef struct {
	int socketCliente;
} t_esperar_mensaje;

t_config* infoConfig;
int dispBase;
char* fsIP;
int fsPort;
int retardoPlanificacion; //en milisegundos
char* algoritmoBalanceo;
uint32_t nombreRutaTemporal;
uint32_t rutaGlobal;


/*
 * nodosConectados son todos los nodos que se conectaron
 * cada nodo sera de la forma t_nodo
 * Si alguno nuevo se conecta en algun momento, le corresponde el ultimo lugar
 */
t_list* nodosConectados;

/**
 * jobs son todos los jobs hechos
 * dentro cada elemento sera un t_job*
 * queda a definir si al terminar el job debe eliminarse
 * de la lista por un tema de espacio
 */
t_list* jobsActivos;

t_list* jobsFinalizados;


typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;


typedef struct{
	int numeroBloque;
	int bytesOcupados;
}t_bloque;


typedef struct{
	int idNodo;
	int disponibilidad;
	uint32_t cargaDeTrabajo;
	uint32_t cargaDeTrabajoHistorica;
	uint32_t cargaDeTrabajoActual;
	t_list * bloques;
	char ipWorker[20];
	int puerto;
}t_nodo;


typedef struct {
	t_nodo* nodo;
	t_bloque* bloque;
	int reduccionGlobal;
} t_planificacion;


typedef struct{
	t_planificacion* nodoPlanificado;
	char* archivoTemporal;
	char* estado;
	char* etapa;
} t_estado;

typedef struct{
	t_list* estadosJob;
	int idJob;
	int idMaster;
	int transformaciones;
	int reduccionesLocales;
}t_job;

void *esperarConexionMaster(void *args);
void inicializarConfigYama();
void hacerPedidoDeTransformacionYRL();

solicitud_transformacion* obtenerSolicitudTrasnformacion(t_job* job);
solicitud_reduccion_local* obtenerSolicitudReduccionLocal(t_job* job);
solicitud_reduccion_global* obtenerSolicitudReduccionGlobal(t_job* job);
solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinal(t_job* job);


#endif /* YAMA_FUNCIONESYAMA_H_ */
