#include "prePlanificacion.h"
#include <stdio.h>

//algunos ejemplos de inicializacion


/*
 * tener en cuenta que no es
 * responsabilidad de yama chequear
 * que esten todos los bloques
 * por lo que se bugea si falta un bloque
 * entre todos los nodos de la lista
 *
 * solo es responsabilidad hacer este
 * chequeo cuando se pide replanificar()
 */


/*
 * para probar otras variaciones
 * modificar asignarBloque de cada
 * nodo.
 *
 * para agregar mas bloques
 * modificar la variable cantidadBloques
 * y que coincida con las cantidad de bloques
 * distintos asignados
 *
 * por ahora no se puede poner un valor arbitrario para
 * cada bloque blX, sino que debe arrancar del 0 en adelante, queda pendiente
 */
void ejemploPrePlanificacion(){

		int i;

		//bloques
		int bl0=0;
		int bl1=1;
		int bl2=2;
		int bl3=3;
		int bl4=4;
		int bl5=5;

		int cantidadBloques = 6;
		int valorBase = 1;
		char* tipoAlgoritmo = "CLOCK";


		t_list* listaNodos = list_create();
		t_list* planificacion = list_create();
		t_planificacion* unaPlanificacion;
		int ejemploNodoFallado = 2;

		//inicializacion nodo0
		//le agrego aparte un valor a cargaDeTrabajo
		//para que quede igual que en el ejemplo del PPT
		t_nodo* nodo0 = malloc(sizeof(t_nodo));
		inicializarNodo(nodo0, "nodo0");
		asignarBloque(nodo0, &bl0);
		asignarBloque(nodo0, &bl4);
		asignarBloque(nodo0, &bl5);
		nodo0->cargaDeTrabajo++;
		list_add(listaNodos, nodo0);
		printf("nodo %s agregado a la lista\n", nodo0->nombreNodo);

		//inicializacion nodo1
		t_nodo* nodo1 = malloc(sizeof(t_nodo));
		inicializarNodo(nodo1, "nodo1");
		asignarBloque(nodo1, &bl1);
		asignarBloque(nodo1, &bl2);
		asignarBloque(nodo1, &bl3);
		asignarBloque(nodo1, &bl5);
		nodo1->cargaDeTrabajo++;
		nodo1->cargaDeTrabajoActual++;
		list_add(listaNodos, nodo1);
		printf("nodo %s agregado a la lista\n", nodo1->nombreNodo);

		//inicializacion nodo2
		t_nodo* nodo2 = malloc(sizeof(t_nodo));
		inicializarNodo(nodo2, "nodo2");
		asignarBloque(nodo2, &bl0);
		asignarBloque(nodo2, &bl1);
		asignarBloque(nodo2, &bl2);
		asignarBloque(nodo2, &bl3);
		asignarBloque(nodo2, &bl4);
		nodo2->cargaDeTrabajo++;
		nodo2->cargaDeTrabajoActual++;
		list_add(listaNodos, nodo2);
		printf("nodo %s agregado a la lista\n", nodo2->nombreNodo);

		planificacion = prePlanificacion(cantidadBloques, valorBase, listaNodos, tipoAlgoritmo);

		printf("ejecucion de preplanificacion exitosa\n\n");
		for(i=0; i<cantidadBloques; i++){
			unaPlanificacion = list_get(planificacion, i);
			printf("bloque %d  asignado al nodo: %s  ",
					unaPlanificacion->bloque, unaPlanificacion->nodo->nombreNodo);
			printf("con disponibilidad restante : %d  y reduccionGlobal (deberia estar en 0): %d \n",
					unaPlanificacion->nodo->disponibilidad, unaPlanificacion->reduccionGlobal);
		}

		unaPlanificacion = list_get(planificacion, i);

		printf("nodo seleccionado para reduccion global: %s \n", unaPlanificacion->nodo->nombreNodo);
		printf("con carga de trabajo para la reduccion global: %d\n", unaPlanificacion->reduccionGlobal);
		printf("tamanio lista planificacion %d + 1 (nodo reduccion global): %d\n", cantidadBloques, list_size(planificacion));


		t_list* nuevaReplanificacion;
		nuevaReplanificacion = replanificacion(listaNodos, ejemploNodoFallado, cantidadBloques, valorBase, tipoAlgoritmo);


		if(nuevaReplanificacion==NULL){
				terminarJob(planificacion);
		} else{
			printf("ejecucion de replanificacion exitosa\n\n");
			printf("tamanio de la lista de nodos: %d\n", list_size(listaNodos));
					for(i=0; i<cantidadBloques; i++){
						unaPlanificacion = list_get(nuevaReplanificacion, i);
						printf("bloque %d asignado al nodo: %s ",
								unaPlanificacion->bloque, unaPlanificacion->nodo->nombreNodo);
						printf("con disponibilidad: %d y reduccionGlobal: %d \n",
								unaPlanificacion->nodo->disponibilidad, unaPlanificacion->reduccionGlobal);
					}

			unaPlanificacion = list_get(nuevaReplanificacion, i);

			printf("nodo seleccionado para reduccion global: %s \n", unaPlanificacion->nodo->nombreNodo);
			printf("con carga de trabajo para la reduccion global: %d\n", unaPlanificacion->reduccionGlobal);
			printf("tamanio lista replanificacion %d + 1 (nodo reduccion global): %d\n",
						cantidadBloques, list_size(nuevaReplanificacion));

		}

}


void inicializarNodo(t_nodo* nodo, char* nombre){
	t_list* bloquesNodo = list_create();
	nodo->nombreNodo = nombre;
	nodo->bloques = bloquesNodo;
	nodo->cargaDeTrabajo = 0;
	nodo->cargaDeTrabajoActual = 0;
	nodo->cargaDeTrabajoHistorica = 0;
	nodo->disponibilidad = 0;
}

void asignarBloque(t_nodo* nodo, int* bloque){
	list_add(nodo->bloques, bloque);
	printf("se agrega bloque %d al nodo %s\n", *bloque, nodo->nombreNodo);
}



