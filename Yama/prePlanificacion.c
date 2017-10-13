#include "funcionesyama.h"

//al haber una variable compartida en los procesos se va a requerir de un semaforo aca
int miBloque = 0;
int base;



void agregarNodoALaPlanificacion(int numNodo, t_nodo* nodo, t_list* planificacionNodos){
	nodo->disponibilidad--;
	list_add(planificacionNodos, &numNodo);
}


void* agregarDisponibilidad(void* unNodo){
	t_nodo* nodo = (t_nodo*) unNodo;

	//el valor de base se cambia en la preplanificacion porque map admite funciones de un solo parametro
	nodo -> disponibilidad += base;
	return nodo;
}

t_list* agregarDisponibilidadNodos(void* nodosDisponibles, int valorBase){
	t_list* nodos = list_create();
	nodos = (t_list*) nodosDisponibles;

	base = valorBase;

	t_list* nodosMapeados;
	nodosMapeados = list_map(nodos, agregarDisponibilidad);
	return nodosMapeados;
}

int siguienteNodo(int numNodo, t_list* nodos, t_nodo* nodo) {
	//si no llego al fin de la lista de nodos
	if (numNodo != list_size(nodos)) {
		numNodo++;
		nodo = list_get(nodos, numNodo);
	} else {
		numNodo = 0;
		nodo = list_get(nodos, numNodo);
	}
	return numNodo;
}


bool estaElBloque(void* bloqueAVerificar){
	int * bloque = (int *) bloqueAVerificar;

	//el numero de bloque se cambia en la funcion anterior porque solo admite un valor el filter
	return *bloque == miBloque;
}


bool tieneElBloque(void* nodo, int bloque){
	miBloque = bloque;
	t_nodo* bloques = (t_nodo*) nodo;
	if (list_any_satisfy(bloques->bloques, estaElBloque)){
			return true;
		}else{
			return false;
		}
}


//todavia no restaura la disponibilidad que tenia previamente cada nodo
t_list* prePlanificacionClock(int bloques, int dispBase, t_list* listaNodos){
		int bloque = 0;
		int numNodo = 0;
		int cantBloques = bloques;
		int valorBase = dispBase;

		//recorren los nodos de la lista de nodos
		t_nodo* nodo;
		t_nodo* aux;

		//esta lista va a tener el orden de los nodos (como enteros) para los bloques (la posicion de la lista)
		t_list* planificacionNodos;

		t_list* nodos = listaNodos;


		nodo = list_get(nodos, 0);

		for(;bloque < cantBloques;){

		if(tieneElBloque(nodo, bloque)){

			if((nodo -> disponibilidad) > 0){

				agregarNodoALaPlanificacion(numNodo, nodo,planificacionNodos);
				numNodo = siguienteNodo(numNodo, nodos, nodo);
				bloque++;

			} else{

				nodo -> disponibilidad = valorBase;
				numNodo = siguienteNodo(numNodo, nodos, nodo);
				bloque++;

			}

		} else{

			int auxNodo = numNodo;
			while(1){

				auxNodo = siguienteNodo(auxNodo, nodos, aux);

				if(aux == nodo){

					nodos = agregarDisponibilidadNodos(nodos, valorBase);

				}else{

					if(tieneElBloque(aux, bloque)){

						if((aux->  disponibilidad) > 0){

							agregarNodoALaPlanificacion(numNodo, nodo,planificacionNodos);
							numNodo = siguienteNodo(numNodo, nodos, aux);
							bloque++;
							break;

						} else{

							auxNodo = siguienteNodo(auxNodo, nodos, aux);

						}

					} else{

						auxNodo = siguienteNodo(auxNodo, nodos, aux);

					}
				}
			}
		}
	}
		return planificacionNodos;
}
