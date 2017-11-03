/*
 * prePlanificacion.h
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#ifndef PREPLANIFICACION_H_
#define PREPLANIFICACION_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <commons/collections/list.h>
#include "funcionesyama.h"
#include "../bibliotecas/estructuras.h"


int valorBaseTemporal;




t_list* prePlanificacion(t_list* bloques, int valorBase, t_list* listaNodos, char* tipoAlgoritmo);
t_list* replanificacion(t_list* listaNodos, int nodoFallado, t_list* bloques, int dispBase, char* tipoAlgoritmo);
void terminarJob(t_list* planificacion);

void agregarDisponibilidad(void* unNodo);
void agregarDisponibilidadNodos(void* nodosDisponibles, int valorBase);
void asignarDisponibilidades(t_list* nodos, char* tipoAlgoritmo);
int disponibilidad (t_nodo* nodo, char* tipoAlgoritmo);
t_nodo* nodoConMayorDisponibilidad(t_list* nodos, int* numNodo, char* tipoAlgoritmo);

void agregarNodoALaPlanificacion(t_bloque* bloque, t_nodo* nodo, t_list* planificacionNodos);
t_nodo* siguienteNodo(int* numNodo, t_list* nodos);

void agregarCargaDeTrabajo(t_planificacion* planificacionNodo);
t_nodo* nodoConMenorCargaTrabajo(t_list* nodos);
int pwl(t_nodo* nodo);
void actualizarWL(t_list* planificacionNodos);

bool estaElBloque(void* bloqueAVerificar);
bool tieneElBloque(t_nodo* nodo, t_bloque* bloque);
bool hayUnaCopiaDeCadaBloque(t_list* listaNodos, t_list* bloques);

void seleccionarNodoParaReduccionFinal(t_list* nodos, char* tipoAlgoritmo, t_list* planificacionNodos, int cantBloques);
void* quitarNodo(t_list* listaNodos, int indiceNodoFallado);
void* obtenerNodos(void* elemento);


//prueba de preplanificacion
void inicializarNodo(t_nodo* nodo, int id);
void asignarBloque(t_nodo* nodo, int numeroBloque, int bytesOcupados);
void ejemploPrePlanificacion();

#endif /* PREPLANIFICACION_H_ */
