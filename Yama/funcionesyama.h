/*
 * funcionesyama.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <commons/collections/list.h>

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
void prePlanificacionClock(int bloques, int dispBase, t_list* listaWorkers);
int siguienteWorker(int numNodo, t_list* nodos, t_nodo* nodo);
void agregarWorkerALaPlanificacion(int bloque, t_nodo* worker);

#endif /* YAMA_FUNCIONESYAMA_H_ */
