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
t_list* replanificacion(char* nodoFallado, t_list* bloques, int dispBase, char* tipoAlgoritmo);
void terminarJob(t_job* job);

void estadisticas(void* unaPlanif);
void loguear_nodos_asignados(t_list* planificacion);

char* generarRutaTemporal();
void* obtenerIdNodoPlanificado(void* nodoPlanificado);

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

void seleccionarNodoParaReduccionFinal(t_list* nodos, t_list* planificacionNodos);
void* quitarNodo(t_list* listaNodos, int indiceNodoFallado);
void* obtenerNodos(void* elemento);

//eliminar
void eliminarBloque(void* elemento);
void eliminarNodo(void* elemento);
void eliminarPlanificacion(void* nodoPlanificado);



//prueba de preplanificacion
void inicializarNodoTest(t_nodo* nodo, char* id);
void inicializarBloque(t_nodo* nodo, t_bloque* bloque, int numero_bloque);
void ejemploPrePlanificacion();

#endif /* PREPLANIFICACION_H_ */
