/*
 * funcionesyama.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <stdbool.h>
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "../Master/interface.h"

#ifndef YAMA_FUNCIONESYAMA_H_
#define YAMA_FUNCIONESYAMA_H_

t_config* infoConfig;
int dispBase;
char* fsIP;
int fsPort;
int retardoPlanificacion; //en milisegundos
char* algoritmoBalanceo;

/*
 * nodosConectados son todos los nodos que se conectaron
 * al filesystem en el orden que les corresponde.
 * Si alguno nuevo se conecta en algun momento, le corresponde el ultimo lugar
 */
t_list* nodosConectados;
int valorBaseTemporal;
int miBloque;


typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;

typedef struct{
	int disponibilidad;
	unsigned long int cargaDeTrabajo;
	t_list * bloques;
}t_nodo;

void *esperarConexionMaster(void *args);

t_list* prePlanificacion(int bloques, int dispBase, t_list* listaNodos, char* tipoAlgoritmo);
t_list* replanificacion(t_list* listaNodos, int nodoFallado, int bloques, int dispBase, char* tipoAlgoritmo);

void agregarDisponibilidad(void* unNodo);
void agregarDisponibilidadNodos(void* nodosDisponibles, int valorBase);
void agregarCarga(t_list* planificacionNodos, int numNodo, int n);
int siguienteNodo(int numNodo, t_list* nodos, t_nodo* nodo);
void agregarNodoALaPlanificacion(int numNodo, t_nodo* nodo,t_list* planificacionNodos);
bool estaElBloque(void* bloqueAVerificar);
bool tieneElBloque(void* nodo, int bloque);

#endif /* YAMA_FUNCIONESYAMA_H_ */
