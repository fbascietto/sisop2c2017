/*
 * funcionesyama.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "../bibliotecas/estructuras.h"
#include "interfaceMaster.h"

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

int socketFS;
uint32_t idMaster;
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
	uint32_t numeroBloque;
	uint32_t bytesOcupados;
	char idNodo[NOMBRE_NODO];
	char ip[LENGTH_IP];
	uint32_t puerto;
	uint32_t idBloque;
}t_bloque;



typedef struct{
	char idNodo[NOMBRE_NODO];
	int disponibilidad;
	t_list* bloquesAsignados;
	uint32_t cargaDeTrabajo;
	uint32_t cargaDeTrabajoHistorica;
	uint32_t cargaDeTrabajoActual;
	char ipWorker[LENGTH_IP];
	uint32_t puerto;
}t_nodo;


typedef struct {
	t_nodo* nodo;
	t_bloque* bloque;
	uint32_t reduccionGlobal;
} t_planificacion;


typedef struct{
	t_planificacion* nodoPlanificado;
	char archivoTemporal[LENGTH_RUTA_ARCHIVO_TEMP];
	char estado[LENGTH_ESTADO];
} t_estado;

typedef struct{
	int idJob;
	int idMaster;
	t_list* estadosTransformaciones;
	t_list* estadosReduccionesLocales;
	t_estado* reduccionGlobal;
	t_list* planificacion;
}t_job;

void *esperarConexionMasterYFS(void *args);
void inicializarConfigYama();
void cargarValoresPlanificacion();
void recargarConfiguracion(int signal);
void hacerPedidoDeTransformacionYRL();

solicitud_transformacion* obtenerSolicitudTrasnformacion(t_job* job);
solicitud_reduccion_local* obtenerSolicitudReduccionLocal(t_job* job);
solicitud_reduccion_global* obtenerSolicitudReduccionGlobal(t_job* job);
solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinal(t_job* job);

t_list* procesarBloquesRecibidos(char* message, uint32_t* masterId);
char* serializarSolicitudJob(char* solicitudArchivo, uint32_t masterId, uint32_t* tamanioSerializado);
void adaptarBloques(t_bloques_enviados* bloquesRecibidos, t_list* bloques);
bool resultadoOk(char* resultado);
void actualizarEstado(char* idNodo, int numeroBloque, int etapa, int idJob, int resultado);
bool finalizoTransformacionesNodo(char* idNodo, int numeroBloque, int idJob);
bool finalizaronReduccionesLocalesNodos(t_job* job);
bool termino(void* elemento);
void procesarSolicitudMaster(int nuevoSocket);



#endif /* YAMA_FUNCIONESYAMA_H_ */
