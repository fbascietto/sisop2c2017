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
t_job* crearJob(t_list* bloques, t_list* listaNodos, char* tipoAlgoritmo, int idMaster){
	int valorBase = dispBase;
	t_list* nuevaPrePlanificacion = prePlanificacion(bloques, valorBase, listaNodos, tipoAlgoritmo);
	t_job* nuevoJob = malloc (sizeof(t_job));

	nuevoJob->idMaster = idMaster;
	nuevoJob->idJob = jobsID;
	generarEstados(nuevaPrePlanificacion, nuevoJob);

	jobsID++;

	return nuevoJob;

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

	for(i=0; i<tamanioPreplanificacion; i++){
		t_planificacion* unNodoPlanificado = list_get(unaPreplanificacion, i);
		if(unNodoPlanificado->reduccionGlobal == 0){
			list_add(job->estadosTransformaciones, crearEstadoTransformacion(unNodoPlanificado));
			list_add(job->estadosReduccionesLocales, crearEstadoReduccionLocal(unNodoPlanificado));
		} else{
			job->reduccionGlobal = crearEstadoReduccionGlobal(unNodoPlanificado);
		}
	}

}

t_estado* crearEstadoTransformacion(t_planificacion* unNodoPlanificado){
	t_estado* unEstado = malloc(sizeof(LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_ESTADO + LENGTH_ETAPA));

	unEstado->nodoPlanificado = unNodoPlanificado;
	strcpy(unEstado->archivoTemporal, generarRutaTemporal());
	strcpy(unEstado->etapa, "transformacion");
	strcpy(unEstado->estado, "en proceso");

	return unEstado;
}

t_estado* crearEstadoReduccionLocal(t_planificacion* unNodoPlanificado){
	t_estado* unEstado = malloc(sizeof(LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_ESTADO + LENGTH_ETAPA));

	unEstado->nodoPlanificado = unNodoPlanificado;
	strcpy(unEstado->archivoTemporal, generarRutaTemporal());
	strcpy(unEstado->etapa, "reduccion local");
	strcpy(unEstado->estado, "esperando transformacion");

	return unEstado;
}

t_estado* crearEstadoReduccionGlobal(t_planificacion* unNodoPlanificado){
	t_estado* unEstado = malloc(sizeof(LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_ESTADO + LENGTH_ETAPA));
	unEstado->nodoPlanificado = unNodoPlanificado;
	strcpy(unEstado->archivoTemporal, generarRutaTemporal());
	strcpy(unEstado->etapa, "reduccion global");
	strcpy(unEstado->estado, "esperando reducciones locales");

	return unEstado;
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

char* proximaEtapa(char* etapa){
	char* siguienteEtapa = malloc(LENGTH_ETAPA);
	if(strcmp(etapa, "transformacion") == 0){
		strcpy(siguienteEtapa, "reduccion local");
		return siguienteEtapa;
	} else{
		strcpy(siguienteEtapa, "reduccion global");
		return siguienteEtapa;
	}
}

void enProcesoSiguienteEtapa(char* idWorker, char* etapa, t_job* job){
	t_estado* unEstado;
	char* siguienteEtapa = proximaEtapa(etapa);

	if(strcmp(siguienteEtapa, "reduccion local")){
		int tamanioEstados = list_size(job->estadosReduccionesLocales);
		int i;
		for(i=0; i<tamanioEstados; i++){
			unEstado = list_get(job->estadosReduccionesLocales, i);
			if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idWorker) == 0){
				strcpy(unEstado->estado,"en proceso");
			}
		}
	}
	else if(strcmp(siguienteEtapa, "reduccion global") == 0){
		strcpy(unEstado->estado, "en proceso");
	} else{
		printf("no se pudo cambiar el estado de la siguiente etapa");
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
				strcpy(unEstado->estado, "finalizado");
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
				strcpy(unEstado->estado, "finalizado");
				enProcesoSiguienteEtapa(idWorker, etapa, job);
				return 1;
			}
		}
	} else if(strcmp(etapa, "reduccion global") == 0){
		strcpy(job->reduccionGlobal->estado, "finalizado");
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


/**
 * todo
 * recibe una planificacion
 * y restaura todos los valores de
 * las cargas de trabajo
 */
void terminarJob(t_job* job){
	printf("finalizando job, mostrando estadisticas (pueden repetirse los nodos)\n\n");

	int tamanioPlanificacion = list_size(job->estadosTransformaciones);
	int i;
	t_estado* unaPlanificacion;



	for(i=0; i<tamanioPlanificacion; i++){
		unaPlanificacion = list_get(job->estadosTransformaciones, i);
		unaPlanificacion->nodoPlanificado->nodo->cargaDeTrabajo--;
		if(unaPlanificacion->nodoPlanificado->reduccionGlobal == 0){
		}	else{
		}
		unaPlanificacion->nodoPlanificado->nodo->disponibilidad++;
	}
	unaPlanificacion = job->reduccionGlobal;
	unaPlanificacion->nodoPlanificado->nodo->cargaDeTrabajo -= unaPlanificacion->nodoPlanificado->reduccionGlobal;
	unaPlanificacion->nodoPlanificado->nodo->disponibilidad++;

	/**estadisticas**/

	printf("id del master: %d\nid del job: %d\n", job->idMaster, job->idJob);
	list_iterate(job->estadosTransformaciones,estadisticas);
	estadisticas(unaPlanificacion);

	/** fin estadisticas**/

	list_add(jobsFinalizados, job);
	printf("job finalizado exitosamente\n");
}

