/*
 * job.c
 *
 *  Created on: 1/11/2017
 *      Author: utnso
 */

#include "job.h"


int jobsID = 0;

/*
 * recibe una lista de bloques, una lista de nodos, un tipo de algoritmo y un id de master
 * y devuelve un nuevo job
 */
void crearJob(t_list* bloques, t_list* listaNodos, char* tipoAlgoritmo, int idMaster){
	int valorBase = dispBase;
	t_list* nuevaPrePlanificacion = prePlanificacion(bloques, valorBase, listaNodos, tipoAlgoritmo);
	t_job* nuevoJob = malloc (sizeof(t_job));

	nuevoJob->idMaster = idMaster;
	nuevoJob->idJob = jobsID;
	nuevoJob->planificacion = nuevaPrePlanificacion;
	generarEstados(nuevaPrePlanificacion, nuevoJob);

	jobsID++;

	jobGlobal = nuevoJob;

}

/**
 * recibe una lista de t_planificacion* y un job
 * y completa los
 * 		estadosTransformaciones
 * 		estadosReduccionesLocales
 * 		t_estado* reduccionGlobal
 * 	con elementos de formatos t_estado*
 *
 */
void generarEstados(t_list* unaPreplanificacion, t_job* job){
	int i;
	int tamanioPreplanificacion = list_size(unaPreplanificacion);

	job->estadosReduccionesLocales = crearEstadosReduccionesLocales(unaPreplanificacion);

	for(i=0; i<tamanioPreplanificacion; i++){
		t_planificacion* unNodoPlanificado = list_get(unaPreplanificacion, i);
		if(unNodoPlanificado->reduccionGlobal == 0){
			list_add(job->estadosTransformaciones, crearEstadoTransformacion(unNodoPlanificado));
		} else{
			job->reduccionGlobal = crearEstadoReduccionGlobal(unNodoPlanificado);
			//job->almacenadoFinal = crearEstadoAlmacenadoFinal(unNodoPlanificado);
		}
	}

}

t_estado* crearEstadoTransformacion(t_planificacion* unNodoPlanificado){
	t_estado* unEstado = malloc(sizeof(t_estado));

	unEstado->nodoPlanificado = unNodoPlanificado;
	strncpy(unEstado->archivoTemporal, generarRutaTemporal(), LENGTH_RUTA_ARCHIVO_TEMP);
	strncpy(unEstado->estado, "en proceso", LENGTH_ETAPA);
	return unEstado;
}

t_estado* crearEstadoReduccionLocal(t_planificacion* unNodoPlanificado, char* rutaReduccionLocal){
	t_estado* unEstado = malloc(sizeof(t_estado));

	unEstado->nodoPlanificado = unNodoPlanificado;
	strncpy(unEstado->archivoTemporal, rutaReduccionLocal, LENGTH_RUTA_ARCHIVO_TEMP);
	strncpy(unEstado->estado, "esperando transformacion", LENGTH_ETAPA);

	return unEstado;
}

t_estado* crearEstadoReduccionGlobal(t_planificacion* unNodoPlanificado){
	t_estado* unEstado = malloc(sizeof(t_estado));
	unEstado->nodoPlanificado = unNodoPlanificado;
	strncpy(unEstado->archivoTemporal, generarRutaTemporal(), LENGTH_RUTA_ARCHIVO_TEMP);
	strncpy(unEstado->estado, "esperando reducciones locales", LENGTH_ETAPA);

	return unEstado;
}

t_list* crearEstadosReduccionesLocales(t_list* unaPreplanificacion){

	t_planificacion* unNodoPlanificado;

	t_estado* nuevoEstado;

	t_list* idNodosTransformacion;
	t_list* rutasTemporales = list_create();
	t_list* estadosReduccionesLocales = list_create();

	int i, j;
	int tamanioPreplanificacion = list_size(unaPreplanificacion);


	idNodosTransformacion = list_map(unaPreplanificacion, obtenerIdNodoPlanificado);

	int cantidadNodos = list_size(idNodosTransformacion);

	for(i=0; i<cantidadNodos; i++){
		list_add(rutasTemporales, generarRutaTemporal());
	}

	for(i=0; i<tamanioPreplanificacion; i++){
		unNodoPlanificado = list_get(unaPreplanificacion, i);
		for(j=0; j<cantidadNodos; j++){
			if(strcmp(unNodoPlanificado->nodo->idNodo, list_get(idNodosTransformacion, j)) == 0){
				nuevoEstado = crearEstadoReduccionLocal(unNodoPlanificado, list_get(rutasTemporales, j));
				list_add(estadosReduccionesLocales, nuevoEstado);
			}
		}
	}

	return estadosReduccionesLocales;
}


t_job* obtenerJob(int idJob, t_list* jobs){
	int i;
	int tamanioJobsActivos = list_size(jobs);
	t_job* miJob;

	for (i=0; i<tamanioJobsActivos; i++){
		miJob = list_get(jobs, i);
		if(miJob->idJob == idJob){
			return miJob;
		}
	}

	return NULL;
}

int obtenerIdJob(int idMaster, t_list* jobs){
	int i;
	int tamanioJobs = list_size(jobs);
	t_job* unJob;
	for(i=0; i<tamanioJobs; i++){
		unJob = list_get(jobs, i);
		if(unJob->idMaster == idMaster){
			return unJob->idJob;
		}
	}
	return NULL;
}

char* proximaEtapa(int etapa){
	char* siguienteEtapa = malloc(LENGTH_ETAPA);
	if(etapa == RESULTADO_TRANSFORMACION){
		strncpy(siguienteEtapa, "reduccion local", LENGTH_REDUCCION_LOCAL);
		return siguienteEtapa;
	} else if(etapa == RESULTADO_REDUCCION_LOCAL){
		strncpy(siguienteEtapa, "reduccion global", LENGTH_REDUCCION_GLOBAL);
		return siguienteEtapa;
	} else{
		strncpy(siguienteEtapa, "almacenado final", LENGTH_ALMACENADO_FINAL);
		return siguienteEtapa;
	}
}

void enProcesoSiguienteEtapa(char* idWorker, int etapa, t_job* job){
	t_estado* unEstado;
	char* siguienteEtapa = proximaEtapa(etapa);

	if(strcmp(siguienteEtapa, "reduccion local") == 0){
		int tamanioEstados = list_size(job->estadosReduccionesLocales);
		int i;
		for(i=0; i<tamanioEstados; i++){
			unEstado = list_get(job->estadosReduccionesLocales, i);
			if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idWorker) == 0){
				strncpy(unEstado->estado, "en proceso", LENGTH_EN_PROCESO);
			}
		}
	}
	else if(strcmp(siguienteEtapa, "reduccion global") == 0){
		strncpy(unEstado->estado, "en proceso", LENGTH_EN_PROCESO);
	} else{
		strncpy(unEstado->estado, "en proceso", LENGTH_EN_PROCESO);
	}
}

//todo
//t_list* falloWorker(int bloque, char* etapa, int idWorker, t_job* job){
//	if(strcmp(etapa, "transformacion") == 0){
//		cambiarEtapaFallo(idWorker, bloque, job);
//		t_list* nodosJob = obtenerNodosJob(job);
//		t_list* bloquesJob = obtenerBloquesJob(job);
//		t_list* laReplanificacion = replanificacion(nodosJob, idWorker, bloquesJob, dispBase, algoritmoBalanceo);
//		return laReplanificacion;
//	} else{
//		terminarJob(job);
//		return NULL;
//	}
//}

bool chequearReduccionLocalNodo(char* idWorker, int bloque, t_job* job){
	int i;
	int tamanioTransformaciones = list_size(job->estadosTransformaciones);
	t_estado* unEstado;
	t_list* transformacionesNodo = list_create();
	for(i=0; i<tamanioTransformaciones; i++){
		unEstado = list_get(job->estadosTransformaciones, i);
		if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idWorker) == 0){
			list_add(transformacionesNodo, unEstado);
		}
	}
	for(i=0; i<list_size(transformacionesNodo); i++){
		unEstado = list_get(transformacionesNodo, i);
		if(strcmp(unEstado->estado, "finalizado") != 0){
			return false;
		}
	}
	return true;
}

/**
 *
 *	Si el nombre es incorrecto, devolver -1,
 *	caso contrario devolver 0 para transformacion
 *	si todos los nodos fueron transformados devuelve 1
 *	devolver 2 para reduccion local
 *	devolver 3 para reduccion global
 */
int cambiarEtapaOK(char* idWorker, char* etapa, int bloque, t_job* job){
	int i;
	int tamanioEstados;
	t_estado* unEstado;

	if(strcmp(etapa, "transformacion") == 0){
		tamanioEstados= list_size(job->estadosTransformaciones);
		for (i=0; i<tamanioEstados; i++){
			unEstado = list_get(job->estadosTransformaciones, i);
			if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idWorker) == 0 &&
					unEstado->nodoPlanificado->bloque->numeroBloque == bloque){
				strncpy(unEstado->estado, "finalizado", LENGTH_FINALIZADO);
				if(chequearReduccionLocalNodo(idWorker, bloque, job)){
					enProcesoSiguienteEtapa(idWorker, etapa, job);
					return 1;
				}else{
					return 0;
				}
			}
		}

	} else if(strcmp(etapa, "reduccion local") == 0){
		tamanioEstados= list_size(job->estadosReduccionesLocales);
		for (i=0; i<tamanioEstados; i++){
			unEstado = list_get(job->estadosReduccionesLocales, i);
			if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idWorker) == 0){
				strncpy(unEstado->estado, "finalizado", LENGTH_FINALIZADO);
				enProcesoSiguienteEtapa(idWorker, etapa, job);
				return 1;
			}
		}
	} else if(strcmp(etapa, "reduccion global") == 0){
		strncpy(job->reduccionGlobal->estado, "finalizado", LENGTH_FINALIZADO);
		return 2;
	}

	return -1;
}

//todo
void respuestaWorker(char* respuesta, int bloque, char* etapa, int idWorker, int idJob){
	t_job* job = obtenerJob(idJob, jobsActivos);
	if (strcmp(respuesta, "finalizado") == 0){
		cambiarEtapaOK(idWorker, etapa, bloque, job);
	}else {
		//falloWorker(idWorker, etapa, bloque, job);
	}
}

t_list* obtenerBloques(t_job* job){
	int i;
	int cantidadTransformaciones = list_size(job->estadosTransformaciones);
	t_estado* unEstado;
	t_list* bloques = list_create();

	for(i=0; i<cantidadTransformaciones; i++){
		unEstado = list_get(job->estadosTransformaciones, i);
		list_add(bloques, unEstado->nodoPlanificado->bloque);
	}

	return bloques;
}

t_list* reducirBloques(t_list* bloques){
	int i;
	int j;
	int cantidadBloques = list_size(bloques);

	t_bloque* unBloque;
	t_bloque* otroBloque;

	t_list* bloquesReducidos = list_create();


	for(i=0; i<cantidadBloques; i++){
		unBloque = list_get(bloques, i);
		for(j=i+1; j<cantidadBloques; j++){
			otroBloque = list_get(bloques, j);
			if(unBloque->idBloque == otroBloque->idBloque){
				break;
			}
		}
		if(unBloque->idBloque != otroBloque->idBloque){
			list_add(bloquesReducidos, unBloque);
		}
	}

	list_add(bloquesReducidos, unBloque);

	return bloquesReducidos;
}

/*
 * la lista de bloques posee en cada bloque el id del nodo
 * que lo tiene en su data.bin
 */
t_list* obtenerNodosParticipantes(t_list* bloques){

	t_list* nodosDeLosBloques;
	t_bloque* unBloque;

	nodosDeLosBloques = obtenerEInicializarNodosDeBloques(bloques);
	setearNodos(nodosDeLosBloques);

	return nodosDeLosBloques;
}


t_list* obtenerEInicializarNodosDeBloques(t_list* bloques){

	int i;
	int cantidadBloques = list_size(bloques);

	t_bloque* unBloque = malloc(sizeof(t_bloque));
	t_list* listaNodos = list_create();
	t_nodo* nuevoNodo = malloc(sizeof(t_nodo));

	for(i=0; i<cantidadBloques; i++){
		unBloque = list_get(bloques, i);
		if(!estaElNodo((unBloque->idNodo), listaNodos)){
			nuevoNodo = inicializarNodo(unBloque);
			list_add(listaNodos, nuevoNodo);
		}
	}

	return listaNodos;
}

t_nodo* inicializarNodo(t_bloque* bloque){
	t_nodo* nodo = malloc(sizeof(t_nodo));
	nodo->bloquesAsignados = list_create();
	strncpy(nodo->idNodo, bloque->idNodo, NOMBRE_NODO);
	nodo->cargaDeTrabajo = 0;
	nodo->cargaDeTrabajoActual = 0;
	nodo->cargaDeTrabajoHistorica = 0;
	nodo->disponibilidad = 0;
	list_add(nodo->bloquesAsignados, bloque);
	return nodo;
}

bool estaElNodo(char* id, t_list* nodos){
	int i;
	int cantidadNodos = list_size(nodos);

	t_nodo* unNodo;

	for(i=0; i<cantidadNodos; i++){
		unNodo = list_get(nodos, i);
		if(strcmp(unNodo->idNodo, id) == 0){
			return true;
		}
	}
	return false;
}

void setearNodos(t_list* nodos){

	int i;
	int cantidadNodos = list_size(nodos);

	t_nodo* unNodo = malloc(sizeof(t_nodo));
	t_nodo* nodoConectado = malloc(sizeof(t_nodo));

	for(i=0; i<cantidadNodos; i++){
		unNodo = list_get(nodos, i);
		if(!estaConectado(unNodo)){
			list_add(nodosConectados, unNodo);
		}else{
			nodoConectado = obtenerNodoConectado(unNodo);
			actualizar(unNodo, nodoConectado);
		}
	}
}


bool estaConectado(t_nodo* unNodo){
	return estaElNodo(unNodo->idNodo, nodosConectados);
}

t_nodo* obtenerNodoConectado(t_nodo* unNodo){
	int i;
	int cantidadNodosConectados = list_size(nodosConectados);

	t_nodo* unNodoConectado;

	for(i=0; i<cantidadNodosConectados; i++){
		unNodoConectado = list_get(nodosConectados, i);
		if(strcmp(unNodoConectado->idNodo, unNodo->idNodo) == 0){
			list_add_all(unNodoConectado->bloquesAsignados, unNodo->bloquesAsignados);
			return unNodoConectado;
		}
	}
	return NULL;
}


void actualizar(t_nodo* nodo, t_nodo* nodoConectado){
	nodo->cargaDeTrabajo = nodoConectado->cargaDeTrabajo;
	nodo->cargaDeTrabajoActual = nodoConectado->cargaDeTrabajoActual;
	nodo->cargaDeTrabajoHistorica = nodoConectado->cargaDeTrabajoHistorica;
	nodo->disponibilidad = nodoConectado->disponibilidad;
	strncpy(nodo->idNodo, nodoConectado->idNodo, NOMBRE_NODO);
	strncpy(nodo->ipWorker, nodoConectado->ipWorker, LENGTH_IP);
	nodo->puerto = nodoConectado->puerto;
}



void actualizarValores(t_planificacion* unaPlanificacion, t_list* nodos, int cantidad){
	int i;
	int cantidadNodos = list_size(nodos);
	t_nodo* unNodo;

	for(i=0; i<cantidadNodos; i++){
		unNodo = list_get(nodos, i);
		if(strcmp(unaPlanificacion->nodo->idNodo, unNodo->idNodo)==0){
			unNodo->cargaDeTrabajo -= cantidad;

			if(unaPlanificacion->reduccionGlobal == 0){
				unNodo->disponibilidad ++;
			}
		}
	}
}

/*
 * restaura en la lista de los nodos conectados
 * las cargas de trabajo y las disponibilidades
 * excepto que se trate de una red Global
 * cuyo caso restaura solo la carga de trabajo
 */
void restaurarValoresJob(t_job* job){
	int tamanioPlanificacion = list_size(job->estadosTransformaciones);
	int i;
	t_estado* unaPlanificacion;

	for(i=0; i<tamanioPlanificacion; i++){
		unaPlanificacion = list_get(job->estadosTransformaciones, i);

		unaPlanificacion->nodoPlanificado->nodo->cargaDeTrabajo--;
		unaPlanificacion->nodoPlanificado->nodo->disponibilidad++;

		actualizarValores(unaPlanificacion, nodosConectados, 1);
	}
	unaPlanificacion = job->reduccionGlobal;
	actualizarValores(unaPlanificacion, nodosConectados, unaPlanificacion->nodoPlanificado->reduccionGlobal);
}

int obtenerPosicionJob(int idJob, t_list* jobs){
	int i;
	int cantidadJobs = list_size(jobs);
	t_job* unJob;

	for(i=0; i<cantidadJobs; i++){
		unJob = list_get(jobs, i);

		if(unJob->idJob == idJob){
			return i;
		}
	}

	return NULL;
}

/*
 * dado el id del job
 * quita un job de jobsActivos
 * y lo agrega a jobsFinalizados
 */
t_job* moverJobAFinalizados(int idJob){

	int posicion = obtenerPosicionJob(idJob, jobsActivos);
	t_job* job;

	job = list_remove(jobsActivos, posicion);

	list_add(jobsFinalizados, job);

	return job;
}


/**
 * todo
 * recibe un id de job
 * y restaura todos los valores de
 * las cargas de trabajo y disponibilidades
 * ademas mueve el job a la lista de jobs finalizados
 * de la lista de jobs activos
 * y devuelve el job fallado
 */
t_job* terminarJob(int idJob){


	//	printf("finalizando job, mostrando estadisticas (pueden repetirse los nodos)\n\n");


	// todo
	//	/**estadisticas**/
	//	printf("id del master: %d\nid del job: %d\n", job->idMaster, job->idJob);
	//	list_iterate(job->estadosTransformaciones,estadisticas);
	//	estadisticas(unaPlanificacion);
	//
	//	/** fin estadisticas**/

	t_job* job;

	job = moverJobAFinalizados(idJob);

	restaurarValoresJob(job);

	printf("job finalizado exitosamente\n");
	return job;
}

