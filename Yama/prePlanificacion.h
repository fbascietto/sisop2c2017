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


/*
 * nodosConectados son todos los nodos que se conectaron
 * al filesystem en el orden que les corresponde.
 * Si alguno nuevo se conecta en algun momento, le corresponde el ultimo lugar
 */
t_list* nodosConectados;
int valorBaseTemporal;
int miBloque;


typedef struct{
	char* nombreNodo;
	int disponibilidad;
	uint32_t cargaDeTrabajo;
	uint32_t cargaDeTrabajoHistorica;
	uint32_t cargaDeTrabajoActual;
	t_list * bloques;
}t_nodo;

typedef struct {
	t_nodo* nodo;
	int bloque;
	int reduccionGlobal;
} t_planificacion;



t_list* prePlanificacion(int bloques, int dispBase, t_list* listaNodos, char* tipoAlgoritmo);
t_list* replanificacion(t_list* listaNodos, int nodoFallado, int bloques, int dispBase, char* tipoAlgoritmo);
void terminarJob(t_list* planificacion);

void agregarDisponibilidad(void* unNodo);
void agregarDisponibilidadNodos(void* nodosDisponibles, int valorBase);
void asignarDisponibilidades(t_list* nodos, char* tipoAlgoritmo);
int disponibilidad (t_nodo* nodo, char* tipoAlgoritmo);
t_nodo* nodoConMayorDisponibilidad(t_list* nodos, int* numNodo, char* tipoAlgoritmo);

void agregarNodoALaPlanificacion(int numNodo, t_nodo* nodo,t_list* planificacionNodos);
t_nodo* siguienteNodo(int* numNodo, t_list* nodos);

void agregarCargaDeTrabajo(t_planificacion* planificacionNodo);
t_nodo* nodoConMenorCargaTrabajo(t_list* nodos);
int pwl(t_nodo* nodo);
void actualizarWL(t_list* planificacionNodos);

bool estaElBloque(void* bloqueAVerificar);
bool tieneElBloque(t_nodo* nodo, int bloque);
bool hayUnaCopiaDeCadaBloque(t_list* listaNodos, int bloques);

void seleccionarNodoParaReduccionFinal(t_list* nodos, char* tipoAlgoritmo, t_list* planificacionNodos, int cantBloques);
void* quitarNodo(t_list* listaNodos, int indiceNodoFallado);
void* obtenerNodos(void* elemento);


//prueba de preplanificacion
void inicializarNodo(t_nodo* nodo, char* nombre);
void asignarBloque(t_nodo* nodo, int* bloque);
void ejemploPrePlanificacion();

#endif /* PREPLANIFICACION_H_ */
