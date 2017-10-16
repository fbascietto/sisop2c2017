#include "funcionesyama.h"

void agregarNodoALaPlanificacion(int numNodo, t_nodo* nodo, t_list* planificacionNodos){
	nodo->disponibilidad--;
	agregarCarga(planificacionNodos, numNodo, 1);
	list_add(planificacionNodos, &numNodo);
}

/*
 * recibe la lista de nodos,
 * la posicion del nodo al que se le quiere aumentar la carga
 * y le agrega n cargas de trabajo
 */
void agregarCarga(t_list* planificacionNodos, int numNodo, int n){
	t_nodo* nodo = list_get(planificacionNodos, numNodo);
	nodo->cargaDeTrabajo += n;
	free(nodo);
}

/*
 * agrega el valor establecido en
 * valorBaseTemporal
 * al nodo
 */
void agregarDisponibilidad(void* unNodo){
	t_nodo* nodo = (t_nodo*) unNodo;
	nodo -> disponibilidad += valorBaseTemporal;
}

/*
 * agrega para todos los nodos
 * el valor enviado en valorBase
 * a su disponibilidad
 */
void agregarDisponibilidadNodos(void* nodosSinDisponibilidad, int valorBase){
	t_list* nodos = list_create();
	nodos = (t_list*) nodosSinDisponibilidad;

	valorBaseTemporal = valorBase;

	list_iterate(nodos, agregarDisponibilidad);
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

/*
 * verifica que haya al menos una copia de cada bloque
 * en la lista de nodos que se le pase
 */
bool hayUnaCopiaDeCadaBloque(t_list* listaNodos, int bloques){
	int i;
	int j;
	int recorridoListaNodos = 0;
	int tamanioListaNodos;
	tamanioListaNodos = list_size(listaNodos);

	//si sale de ambos for, todos los bloques tienen al menos una copia en los nodos
	//i recorre cada bloque
	for (i=0; i<bloques;){

		//j recorre cada lugar de los nodos
		for (j=0; j < tamanioListaNodos; j++){

			if(tieneElBloque(list_get(listaNodos, j), i)){
						i++;
						j=0;
						recorridoListaNodos=1;
						break;
			}
			recorridoListaNodos++;
		}
		//si recorrio toda la lista, recorridoListaNodos sale con el tamanio de la lista
		if(recorridoListaNodos == tamanioListaNodos){
			return false;
		}
	}
	return true;
}

int disponibilidad (t_nodo* nodo, char* tipoAlgoritmo){
	if (strcmp(tipoAlgoritmo, "CLOCK") == 0){
		return nodo -> disponibilidad;
	}else{
		return (nodo -> disponibilidad) + pwl(nodo);
	}
}

int pwl(t_nodo* nodo){
	/*
	 * TBD: preguntar si WLmax es para todos los nodos que existan
	 * o si se debe calcular solo para los nodos que se pasaron
	 * para la preplanificacion
	 */
	return 0;
}


//todavia no restaura la disponibilidad que tenia previamente cada nodo
/**
 * Retorna una lista t_list
 * donde cada posicion indica el numero de bloque
 * y el numero contenido indica el numero de nodo
 */
t_list* prePlanificacion(int cantBloques, int valorBase, t_list* listaNodos, char* tipoAlgoritmo){
		char* algoritmo = tipoAlgoritmo;
		int bloque;
		int numNodo = 0;

		//recorren los nodos de la lista de nodos
		t_nodo* nodo = NULL;
		t_nodo* aux = NULL;

		/*
		 * esta lista va a tener el orden de los nodos (por el momento la lista tiene solo el numero correspondiente del nodo)
		 * para los bloques (la posicion de la lista)
		*/
		t_list* planificacionNodos = list_create();

		t_list* nodos = listaNodos;


		nodo = list_get(nodos, 0);

		for(bloque=0;bloque < cantBloques;){

		if(tieneElBloque(nodo, bloque)){

			if(disponibilidad(nodo, algoritmo) > 0){

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

					agregarDisponibilidadNodos(nodos, valorBase);

				}else{

					if(tieneElBloque(aux, bloque)){

						if(disponibilidad(aux, algoritmo) > 0){

							agregarNodoALaPlanificacion(numNodo, nodo, planificacionNodos);
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
		free(nodo);
		free(aux);
		list_destroy(nodos);
		return planificacionNodos;
}


void quitarNodo(t_list* listaNodos, int nodoFallado){
	list_remove(listaNodos, nodoFallado);
}

t_list* replanificacion(t_list* listaNodos, int nodoFallado, int bloques, int dispBase, char* tipoAlgoritmo){

	quitarNodo(listaNodos, nodoFallado);

	if (hayUnaCopiaDeCadaBloque(listaNodos, bloques)){
		return prePlanificacion(bloques, dispBase, listaNodos, tipoAlgoritmo);
	} else{
		//pendiente: definir que devolver en caso de que no haya una copia de cada bloque
		return NULL;
	}
}
