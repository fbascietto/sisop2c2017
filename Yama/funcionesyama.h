/*
 * funcionesyama.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "../Master/interface.h"

#ifndef YAMA_FUNCIONESYAMA_H_
#define YAMA_FUNCIONESYAMA_H_

typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;

typedef struct{
	int disponibilidad;
	t_list * bloques;
}t_nodo;

void *esperarConexionMaster(void *args);

t_list* prePlanificacionClock(int bloques, int dispBase, t_list* listaNodos);

void* agregarDisponibilidad(void* unNodo);
t_list* agregarDisponibilidadNodos(void* nodosDisponibles, int valorBase);
int siguienteNodo(int numNodo, t_list* nodos, t_nodo* nodo);
void agregarNodoALaPlanificacion(int numNodo, t_nodo* nodo,t_list* planificacionNodos);
bool estaElBloque(void* bloqueAVerificar);
bool tieneElBloque(void* nodo, int bloque);

#endif /* YAMA_FUNCIONESYAMA_H_ */
