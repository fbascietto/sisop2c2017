#include "prePlanificacion.h"
#include "funcionesyama.h"

void agregarNodoALaPlanificacion(t_bloque* bloque, t_nodo* nodo, t_list* planificacionNodos){
	nodo->disponibilidad--;
	nodo->cargaDeTrabajoActual++;

	t_planificacion* planificacion = malloc(sizeof(t_planificacion));
	planificacion->nodo = nodo;
	planificacion->bloque = bloque;
	planificacion->reduccionGlobal = 0;
	list_add(planificacionNodos, planificacion);
}


/*
 * setea las disponibilidades de los nodos
 * segun el algoritmo especificado
 * y ademas resetea la carga de trabajo actual a 0
 */
void asignarDisponibilidades(t_list* nodos, char* tipoAlgoritmo){
	int i;
	int tamanioLista;
	t_nodo* nodo;

	tamanioLista = list_size(nodos);

	for(i=0; i<tamanioLista; i++){
		nodo = list_get(nodos, i);
		nodo->disponibilidad = disponibilidad(nodo, tipoAlgoritmo);
		nodo->cargaDeTrabajoActual = 0;
		printf("disponibilidad de %d: %d\n", nodo->idNodo, nodo->disponibilidad);
	}
}

/*
 * agrega el valor establecido en
 * valorBaseTemporal al nodo
 * establecido en agregarDisponibilidadNodos
 */
void agregarDisponibilidad(void* unNodo){
	t_nodo* nodo = (t_nodo*) unNodo;
	nodo -> disponibilidad += valorBaseTemporal;
}

void agregarCargaDeTrabajo(t_planificacion* planificacionNodo) {
	if (planificacionNodo->reduccionGlobal == 0) {
		planificacionNodo->nodo->cargaDeTrabajo++;
		planificacionNodo->nodo->cargaDeTrabajoHistorica++;
	} else {
		planificacionNodo->nodo->cargaDeTrabajo +=
				planificacionNodo->reduccionGlobal;
		planificacionNodo->nodo->cargaDeTrabajoActual +=
				planificacionNodo->reduccionGlobal;
		planificacionNodo->nodo->cargaDeTrabajoHistorica +=
				planificacionNodo->reduccionGlobal;
	}
}

void actualizarWL(t_list* planificacionNodos){
	int i;
	int tamanio = list_size(planificacionNodos);
	t_planificacion* planificacionNodo = NULL;

	for(i=0; i<tamanio; i++){
		planificacionNodo = list_get(planificacionNodos, i);
		agregarCargaDeTrabajo(planificacionNodo);
	}
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

/*
 * dada una posicion
 * y una lista de nodos
 * 		retorna el siguiente nodo
 * 		y actualiza el numero de la lista al siguiente
 */
t_nodo* siguienteNodo(int* numNodo, t_list* nodos) {

	t_nodo* nodo;
	//si no llego al fin de la lista de nodos
	if ((*numNodo) != (list_size(nodos) - 1)) {
		(*numNodo)++;
		nodo = list_get(nodos, *numNodo);
	} else {
		(*numNodo) = 0;
		nodo = list_get(nodos, *numNodo);
	}

	return nodo;
}


bool tieneElBloque(t_nodo* nodo, t_bloque* bloque){
	int * unBloque = NULL;
	int i;
	int cantBloques;

	cantBloques = list_size(nodo->bloques);

	for (i=0; i<cantBloques; i++){
		unBloque = list_get(nodo->bloques, i);
		if(*unBloque == bloque->numeroBloque){
			return true;
		}
	}


	return false;
}

/*
 * verifica que haya al menos una copia de cada bloque
 * en la lista de nodos que se pase
 */
bool hayUnaCopiaDeCadaBloque(t_list* listaNodos, t_list* bloques){
	int i;
	int j;
	int cantBloques = list_size(bloques);
	t_bloque* bloque;
	int recorridoListaNodos = 0;
	int tamanioListaNodos;
	tamanioListaNodos = list_size(listaNodos);

	//si sale de ambos for, todos los bloques tienen al menos una copia en los nodos
	//i recorre cada bloque
	for (i=0; i<cantBloques;){
		bloque = list_get(bloques, i);
		//j recorre cada lugar de los nodos
		for (j=0; j < tamanioListaNodos; j++){

			if(tieneElBloque(list_get(listaNodos, j), bloque)){
						i++;
						j=0;
						recorridoListaNodos=0;
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
		return valorBaseTemporal;
	}else{
		return valorBaseTemporal + pwl(nodo);
	}
}


int pwl(t_nodo* nodo){
	return nodo->cargaDeTrabajo - nodo->cargaDeTrabajoActual;
}


t_nodo* nodoConMayorDisponibilidad(t_list* nodos, int* numNodo, char* tipoAlgoritmo){
	int tamanioLista = list_size(nodos);
	int i;
	int disponibilidadNodo;
	t_nodo* nodo;
	t_nodo* nodoConMasDisponibilidad = list_get(nodos, 0);
	unsigned long int mayorDisponibilidad = 0;

	for(i=0; i < tamanioLista; i++){
		nodo = list_get(nodos, i);
		disponibilidadNodo = nodo->disponibilidad;
		if (mayorDisponibilidad < disponibilidadNodo){
			mayorDisponibilidad = disponibilidadNodo;
			nodoConMasDisponibilidad = nodo;
			*numNodo = i;

		} else
			if (mayorDisponibilidad == disponibilidadNodo){
				if (nodoConMasDisponibilidad->cargaDeTrabajoHistorica > nodo->cargaDeTrabajoHistorica){
					nodoConMasDisponibilidad = nodo;
					*numNodo = i;
				}
			}
	}

	return nodoConMasDisponibilidad;
}

t_nodo* nodoConMenorCargaTrabajo(t_list* nodos){
	int tamanioLista = list_size(nodos);
	int i;
	int cargaDeTrabajoNodo;
	t_nodo* nodo;
	t_nodo* nodoConMenosCargaTrabajo = list_get(nodos, 0);
	unsigned long int menorCargaDeTrabajo = 999999;

	for(i=0; i < tamanioLista; i++){
			nodo = list_get(nodos, i);
			cargaDeTrabajoNodo = nodo->cargaDeTrabajoActual;
			if(menorCargaDeTrabajo > cargaDeTrabajoNodo){
				menorCargaDeTrabajo = cargaDeTrabajoNodo;
				nodoConMenosCargaTrabajo = nodo;
			}
	}
	return nodoConMenosCargaTrabajo;
}


void seleccionarNodoParaReduccionFinal(t_list* nodos, char* tipoAlgoritmo, t_list* planificacionNodos, int cantBloques){
	t_nodo* nodo;
	nodo = nodoConMenorCargaTrabajo(nodos);
	t_planificacion* planificacionRG = malloc(sizeof(t_planificacion));
	planificacionRG->nodo = nodo;

	//mitad redondeada para arriba de cantidad de bloques a reducir para reduccion final
	if(cantBloques % 2 == 0){
		planificacionRG->reduccionGlobal = cantBloques/2;
	}else{
		planificacionRG->reduccionGlobal = (cantBloques/2) +1;
	}
	list_add(planificacionNodos,planificacionRG);
}



/**
 * Retorna una lista t_list
 * con structs t_planificacion
 * todos los elementos tienen el valor reduccionGlobal en 0 salvo
 * el bloque seleccionado para reduccion global
 */
t_list* prePlanificacion(t_list* bloques, int valorBase, t_list* listaNodos, char* tipoAlgoritmo){
		int contadorBloques;
		t_bloque* bloque;
		int numNodo = 0;
		int cantBloques = list_size(bloques);

		//recorren los nodos de la lista de nodos
		t_nodo* nodo = NULL;
		t_nodo* aux = NULL;

		t_list* planificacionNodos = list_create();

		valorBaseTemporal = valorBase;
		asignarDisponibilidades(listaNodos, tipoAlgoritmo);

		nodo = nodoConMayorDisponibilidad(listaNodos, &numNodo, tipoAlgoritmo);

		for(contadorBloques=0;contadorBloques < cantBloques;){

		bloque = list_get(bloques, contadorBloques);

		if(tieneElBloque(nodo, bloque)){

			if(nodo->disponibilidad > 0){

				agregarNodoALaPlanificacion(bloque, nodo, planificacionNodos);
				nodo = siguienteNodo(&numNodo, listaNodos);
				contadorBloques++;

			} else{

				nodo -> disponibilidad = valorBase;
				nodo = siguienteNodo(&numNodo, listaNodos);

			}

		} else{

			int auxNodo = numNodo;
			aux = siguienteNodo(&auxNodo, listaNodos);
			while(1){


				if(auxNodo == numNodo){

					agregarDisponibilidadNodos(listaNodos, valorBase);
					aux = siguienteNodo(&auxNodo, listaNodos);

				}else{

					if(tieneElBloque(aux, bloque)){

						if(aux->disponibilidad > 0){

							agregarNodoALaPlanificacion(bloque, aux, planificacionNodos);
							contadorBloques++;
							break;

						} else{

							aux = siguienteNodo(&auxNodo, listaNodos);

						}

					} else{

						aux = siguienteNodo(&auxNodo, listaNodos);

					}
				}
			}
		}
	}

		seleccionarNodoParaReduccionFinal(listaNodos, tipoAlgoritmo, planificacionNodos, cantBloques);
		actualizarWL(planificacionNodos);
		return planificacionNodos;
}

/*
 * dada la lista de preplanificacion de nodos
 * el tipo de algoritmo usado
 * la disponibilidad base usada previamente
 * bloques que va a utilizar (estan tambien en la lista de planificacion de nodos)
 * y el nodo que fallo
 *
 * replanifica en base al nodo fallado
 *
 * si no hay una copia de cada bloque
 * devuelve NULL
 */
t_list* replanificacion(t_list* listaNodos, int nodoFallado, t_list* bloques, int dispBase, char* tipoAlgoritmo){

	t_nodo* nodoEliminado;

	nodoEliminado = list_remove(listaNodos, nodoFallado);

	printf("se ejecuta rePlanificacion porque fallo el nodo %d \n", nodoEliminado->idNodo);
	printf("se eliminino el nodo %d\n", nodoEliminado->idNodo);
	printf("nuevo tamanio de la lista: %d\n", list_size(listaNodos));

	if (hayUnaCopiaDeCadaBloque(listaNodos, bloques)){

		return prePlanificacion(bloques, dispBase, listaNodos, tipoAlgoritmo);

	} else{

		//pendiente: definir que devolver en caso de que no haya una copia de cada bloque
		return NULL;
	}
}



void estadisticas(void* unaPlanif){
	t_estado* unaPlanificacion = (t_estado*) unaPlanif;
	if(unaPlanificacion->nodoPlanificado->reduccionGlobal == 0){
		printf("bloque: %d nodo: %d cargaDeTrabajoActual: %d \n",
			unaPlanificacion->nodoPlanificado->bloque->numeroBloque,
			unaPlanificacion->nodoPlanificado->nodo->idNodo,
			unaPlanificacion->nodoPlanificado->nodo->cargaDeTrabajoActual);
	}else{
		printf("nodo de reduccion final: %d con una cargaDeTrabajoActual: %d \n",
			unaPlanificacion->nodoPlanificado->nodo->idNodo,
			unaPlanificacion->nodoPlanificado->nodo->cargaDeTrabajoActual);
	}

}

bool quitarReduccionLocal(void* estado){
	t_estado* unEstado = (t_estado*) estado;
	if (strcmp(unEstado->etapa, "reduccion local") == 0){
		return false;
	}else{
		return true;
	}
}

/**
 * todo
 * recibe una planificacion
 * y restaura todos los valores de
 * las cargas de trabajo
 */
void terminarJob(t_job* job){
	printf("finalizando job, mostrando estadisticas (pueden repetirse los nodos)\n\n");
	t_list* estados = list_filter(job->estadosJob, quitarReduccionLocal);

	int tamanioPlanificacion = list_size(estados);
	int i;
	t_estado* unaPlanificacion;



	for(i=0; i<tamanioPlanificacion; i++){
		unaPlanificacion = list_get(estados, i);
		if(unaPlanificacion->nodoPlanificado->reduccionGlobal == 0){
			unaPlanificacion->nodoPlanificado->nodo->cargaDeTrabajo--;
		}	else{
			unaPlanificacion->nodoPlanificado->nodo->cargaDeTrabajo -= unaPlanificacion->nodoPlanificado->reduccionGlobal;
		}
		unaPlanificacion->nodoPlanificado->nodo->disponibilidad++;
	}
	list_iterate(estados,estadisticas);
	list_destroy(estados);
	free(job);
	printf("job finalizado exitosamente\n");
}



