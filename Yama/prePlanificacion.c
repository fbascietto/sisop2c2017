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
		printf("disponibilidad de %s: %d\n", nodo->idNodo, nodo->disponibilidad);
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
	t_list* nodos;
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
//	int * unBloque = NULL;
//	int i;
//	int cantBloques;
//
//	cantBloques = list_size(nodo->bloques);
//
//	for (i=0; i<cantBloques; i++){
//		unBloque = list_get(nodo->bloques, i);
//		if(*unBloque == bloque->numeroBloque){
//			return true;
//		}
//	}
//
//
//	return false;

		if(strcmp(nodo->idNodo, bloque->idNodo) == 0){
			return true;
		}else{
			return false;
		}
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


//todo
void loguear_nodos_asignados(t_list* planificacion){
	int i;
	int cantidadBloques = list_size(planificacion);

	t_planificacion* unaPlanificacion;

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* yama_log = log_create("logYama.txt", "YAMA", 0, level);



	for(i=0; i<cantidadBloques; i++){
		unaPlanificacion = list_get(planificacion, i);
		log_trace(yama_log, "bloque %d  asignado al nodo: %s",
				unaPlanificacion->bloque->numeroBloque, unaPlanificacion->nodo->idNodo);
		log_trace(yama_log, "con disponibilidad restante : %d  y reduccionGlobal (deberia estar en 0): %d\n",
				unaPlanificacion->nodo->disponibilidad, unaPlanificacion->reduccionGlobal);
	}

	free(yama_log);
}

void* obtenerIdNodoPlanificado(void* nodo){
	t_planificacion* nodoPlanificado = (t_planificacion*) nodo;
	char* idNodo = malloc(NOMBRE_NODO);
	strncpy(idNodo, nodoPlanificado->nodo->idNodo, NOMBRE_NODO);
	return idNodo;
}

/*
 * cada nodo distinto va a tener un solo archivo de reduccion local
 * con n archivos de transformacion
 * usando la lista de nodos (asumo que todos son distintos)
 * obtengo la cantidad de archivos resultantes voy a tener
 */
void seleccionarNodoParaReduccionFinal(t_list* nodos, t_list* planificacionNodos){

	int cantidadArchivosReduccionLocal;

	t_nodo* nodo;
	t_planificacion* planificacionRG = malloc(sizeof(t_planificacion));

	nodo = nodoConMenorCargaTrabajo(nodos);
	planificacionRG->nodo = nodo;
	planificacionRG->bloque = NULL;

	cantidadArchivosReduccionLocal = list_size(nodos);

	//mitad redondeada para arriba de cantidad de bloques a reducir para reduccion final
	if(cantidadArchivosReduccionLocal % 2 == 0){
		planificacionRG->reduccionGlobal = cantidadArchivosReduccionLocal/2;
	}else{
		planificacionRG->reduccionGlobal = (cantidadArchivosReduccionLocal/2) +1;
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

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* yama_log = log_create("logYama.txt", "YAMA", 1, level);
	t_log* yama_error_log = log_create("logYama.txt", "YAMA", 1, level_ERROR);

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



	log_destroy(yama_log);
	log_destroy(yama_error_log);

	seleccionarNodoParaReduccionFinal(listaNodos, planificacionNodos);
	actualizarWL(planificacionNodos);

	//loguear_nodos_asignados(planificacionNodos);

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
t_list* replanificacion(char* nodoFallado, t_list* bloques, int dispBase, char* tipoAlgoritmo){

//	t_nodo* nodoEliminado;
//
//	nodoEliminado = list_remove(listaNodos, nodoFallado);
//
//	printf("se ejecuta rePlanificacion porque fallo el nodo %s \n", nodoEliminado->idNodo);
//	printf("se eliminino el nodo %s\n", nodoEliminado->idNodo);
//	printf("nuevo tamanio de la lista: %d\n", list_size(listaNodos));
//
	t_list* listaNodos;
	listaNodos = obtenerEInicializarNodosDeBloques(bloques);

	t_nodo* nodoEliminado;
	//nodoEliminado = quitarNodoFallado(nodoFallado, listaNodos);

	if (hayUnaCopiaDeCadaBloque(listaNodos, bloques)){

		return prePlanificacion(bloques, dispBase, listaNodos, tipoAlgoritmo);

	} else{

		//pendiente: definir que devolver en caso de que no haya una copia de cada bloque
		return NULL;
	}


}

bool sePuedeReplanificar(t_list* bloques){
	t_list* nodos;
	nodos = obtenerNodosParticipantes(bloques);
	if(hayUnaCopiaDeCadaBloque(nodos, bloques)){
		return true;
	}else{
		return false;
	}
}


/*
 * quita el t_nodo*
 * que corresponde de la
 * lista nodosConectados
 */
void desconectarNodo(char* idNodo){
	int i;
	int cantidadNodos = list_size(nodosConectados);
	t_nodo* unNodo;

	for(i=0; i<cantidadNodos; i++){
		unNodo = list_get(nodosConectados, i);
		if(strcmp(unNodo->idNodo, idNodo)==0){
			unNodo = list_remove(nodosConectados, i);
			free(unNodo);
			break;
		}
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






