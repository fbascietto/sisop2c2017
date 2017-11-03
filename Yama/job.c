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

	nuevoJob->estadosJob = generarEstados(nuevaPrePlanificacion);
	nuevoJob->idMaster = idMaster;
	nuevoJob->idJob = jobsID;
	nuevoJob->reduccionesLocales = 0;
	nuevoJob->transformaciones = 0;

	jobsID++;

	return nuevoJob;

}

t_list* generarEstados(t_list* unaPreplanificacion){
	int i;
	int tamanioPreplanificacion = list_size(unaPreplanificacion);
	t_list* estados = list_create();

	for(i=0; i<tamanioPreplanificacion; i++){
		t_planificacion* unNodoPlanificado = list_get(unaPreplanificacion, i);
		if(unNodoPlanificado->reduccionGlobal == 0){
			list_add(estados, crearEstadoTransformacion(unNodoPlanificado));
			list_add(estados, crearEstadoReduccionLocal(unNodoPlanificado));
		} else{
			list_add(estados, crearEstadoReduccionGlobal(unNodoPlanificado));
		}
	}

	return estados;

}

t_estado* crearEstadoTransformacion(t_planificacion* unNodoPlanificado){
	t_estado* unEstado = malloc(sizeof(LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_ESTADO + LENGTH_ETAPA));

	unEstado->nodoPlanificado = unNodoPlanificado;
	unEstado->archivoTemporal = generarRutaTemporal();
	unEstado->etapa = "transformacion";
	unEstado->estado = "en proceso";

	return unEstado;
}

t_estado* crearEstadoReduccionLocal(t_planificacion* unNodoPlanificado){
	t_estado* unEstado = malloc(sizeof(LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_ESTADO + LENGTH_ETAPA));

	unEstado->nodoPlanificado = unNodoPlanificado;
	unEstado->archivoTemporal = generarRutaTemporal();
	unEstado->etapa = "reduccion local";
	unEstado->estado = "esperando transformacion";

	return unEstado;
}

t_estado* crearEstadoReduccionGlobal(t_planificacion* unNodoPlanificado){
	t_estado* unEstado = malloc(sizeof(LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_ESTADO + LENGTH_ETAPA));
	unEstado->nodoPlanificado = unNodoPlanificado;
	unEstado->archivoTemporal = generarRutaTemporal();
	unEstado->etapa = "reduccion global";
	unEstado->estado = "esperando reducciones locales";

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

void enProcesoSiguienteEtapa(int idWorker, char* etapa, t_job* job){
	int i;
	int tamanioEstados = list_size(job->estadosJob);
	t_estado* unEstado;
	char* siguienteEtapa = proximaEtapa(etapa);
	if(siguienteEtapa != "job terminado"){
		for (i=0; i<tamanioEstados; i++){
			unEstado = list_get(job->estadosJob, i);
			if(unEstado->nodoPlanificado->nodo->idNodo == idWorker &&
					unEstado->etapa == siguienteEtapa){
				unEstado->estado = "en proceso";
				break;
			}
		}

	}
}

//todo
t_list* falloWorker(int bloque, char* etapa, int idWorker, t_job* job){
	cambiarEtapaFallo(idWorker, bloque, job);
	t_list* nodosJob = obtenerNodosJob(job);
	t_list* bloquesJob = obtenerBloquesJob(job);
	t_list* laReplanificacion = replanificacion(nodosJob, idWorker, bloquesJob, dispBase, algoritmoBalanceo);
	return laReplanificacion;
}

/**
 * todo
 * no esta terminado
 * falta chequear que en transformacion todos los
 * bloques hayan sido transformados
 * para pasar a la reduccion local a "en proceso"
 */
void cambiarEtapaOk(int idWorker, char* etapa, int bloque, t_job* job){
	int i;
	int tamanioEstados = list_size(job->estadosJob);
	t_estado* unEstado;

	for (i=0; i<tamanioEstados; i++){
		unEstado = list_get(job->estadosJob, i);
		if(unEstado->nodoPlanificado->nodo->idNodo == idWorker &&
				unEstado->nodoPlanificado->bloque->numeroBloque == bloque &&
				unEstado->etapa == etapa){
			unEstado->estado = "finalizado";
			enProcesoSiguienteEtapa(idWorker, etapa, job);
			break;
		}
	}
}


//todo
void respuestaWorker(char* respuesta, int bloque, char* etapa, int idWorker, int idJob){
	t_job* job = obtenerJob(idJob, jobsActivos);
	if (respuesta == "finalizado"){
		cambiarEtapaOK(idWorker, bloque, job);
	}else {
		falloWorker(idWorker, bloque, job);
	}
}
