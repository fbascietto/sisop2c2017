/*
 * funcionesyama.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <commons/config.h>
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

typedef struct{
	t_nodo* nodo;
	t_list* bloques;
}nodoConectado;


typedef struct {
	t_nodo* nodo;
	t_bloque* bloque;
	int reduccionGlobal;
} t_planificacion;

void *esperarConexionMaster(void *args);
void inicializarConfigYama();
void hacerPedidoDeTransformacionYRL();

solicitud_transformacion* obtenerSolicitudTrasnformacion(t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasTemporales);
solicitud_reduccion_local* obtenerSolicitudReduccionLocal(t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasTransformacionTemporales, char* rutaReduccion);
solicitud_reduccion_global* obtenerSolicitudReduccionGlobal(t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasRedLocalTemporales, char* rutaReduccionGlobal);
solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinal(t_nodo* nodoEncargado, int puerto_worker, char* ip_worker, char* rutaReduccionGlobal);


#endif /* YAMA_FUNCIONESYAMA_H_ */
