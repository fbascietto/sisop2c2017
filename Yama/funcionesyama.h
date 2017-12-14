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
#include "job.h"

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
uint32_t rutaGlobal;


t_log* logYama;
t_log* logYamaImpreso;
t_log* logYamaError;
t_log* logYamaErrorImpreso;

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

t_esperar_conexion *esperarConexion;


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
	uint32_t idJob;
	int idMaster;
	t_list* estadosTransformaciones;
	t_list* estadosReduccionesLocales;
	t_estado* reduccionGlobal;
	char estadoAlmacenado[LENGTH_ESTADO];
	t_list* planificacion;
}t_job;

t_job* jobGlobal;

void esperarConexionMasterYFS(void *args);
void inicializarConfigYama();
void cargarValoresPlanificacion();
void recargarConfiguracion(int signal);
void hacerPedidoDeTransformacionYRL();

solicitud_transformacion* obtenerSolicitudTrasnformacion(t_job* job);
item_reduccion_local* obtenerSolicitudReduccionLocal(t_job* job, char idNodo[NOMBRE_NODO]);
solicitud_reduccion_global* obtenerSolicitudReduccionGlobal(t_job* job);
solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinal(t_job* job);

t_list* procesarBloquesRecibidos(char* message, uint32_t* masterId);
char* serializarSolicitudJob(char* solicitudArchivo, uint32_t masterId, uint32_t* tamanioSerializado);
void adaptarBloques(t_bloques_enviados* bloquesRecibidos, t_list* bloques);
bool resultadoOk(char* resultado);
void actualizarEstado(char* idNodo, int numero_bloque, int etapa, int idJob, int resultado);
bool finalizoTransformacionesNodo(char* idNodo, int numero_bloque, int idJob);
bool finalizaronReduccionesLocalesNodos(t_job* job);
bool termino(void* elemento);
void procesarSolicitudMaster(int nuevoSocket);
t_job* crearNuevoJob(int idMaster, t_list* bloques, char* algoritmo);

/************ tabla de estados **********/
void tablaDeEstados(t_job* job);
void imprimirTransformaciones(t_list* transformaciones, int idJob, int idMaster);
void imprimirReduccionesLocales(t_list* reducciones, t_list* transformaciones, int idJob, int idMaster);
void logEstadoTransformacion(int idJob, int idMaster, t_estado* unEstado);
void logEstadoReduccionLocal(int idJob, int idMaster, t_estado* estadoTransformacion, t_estado* estadoReduccionLocal);
void logEstadoReduccionGlobal(int idJob, int idMaster, t_estado* estadoReduccionGlobal);
void logEstadoAlmacenadoFinal(t_job* job);

void enProcesoReduccionLocal(int idNodo, t_job* job);
void enProcesoReduccionGlobal(t_job* job);
void enProcesoAlmacenadoFinal(t_job* job);


#endif /* YAMA_FUNCIONESYAMA_H_ */
