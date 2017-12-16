#include "funcionesyama.h"


void tablaDeEstadosJobsActivos(){
	list_iterate(jobsActivos, tablaDeEstados);
}

void tablaDeEstadosJobsFinalizados(){
	list_iterate(jobsFinalizados, tablaDeEstados);
}

void tablaDeEstados(void* unJob){
	t_job* job = (void*) unJob;
	log_trace(logYamaImpreso, "imprimiendo tabla de estados del job %d", job->idJob);
	imprimirTransformaciones(job->estadosTransformaciones, job->idJob, job->idMaster);
	imprimirReduccionesLocales(job->estadosReduccionesLocales, job->estadosTransformaciones, job->idJob, job->idMaster);
	logEstadoReduccionGlobal(job->idJob, job->idMaster, job->reduccionGlobal);
	logEstadoAlmacenadoFinal(job);
}


void imprimirTransformaciones(t_list* transformaciones, int idJob, int idMaster){
	int i;
	int tamanioTransformaciones = list_size(transformaciones);
	t_estado* unEstado;
	for(i=0; i<tamanioTransformaciones; i++){
		unEstado = list_get(transformaciones, i);
		logEstadoTransformacion(idJob, idMaster, unEstado);
	}
}


void imprimirReduccionesLocales(t_list* reducciones, t_list* transformaciones, int idJob, int idMaster){
	int i, j;
	int tamanioTransformaciones = list_size(transformaciones);
	int tamanioReducciones = list_size(reducciones);
	t_estado* estadoTransformacion;
	t_estado* estadoReduccion;

	for(i=0; i<tamanioReducciones; i++){
		estadoReduccion = list_get(reducciones, i);
		for(j=0; j<tamanioTransformaciones; j++){
			estadoTransformacion = list_get(transformaciones, j);
			if(strcmp(estadoReduccion->nodoPlanificado->nodo->idNodo, estadoTransformacion->nodoPlanificado->nodo->idNodo) == 0){
				logEstadoReduccionLocal(idJob, idMaster, estadoTransformacion, estadoReduccion);
			}
		}
	}
}

void logEstadoTransformacion(int idJob, int idMaster, t_estado* unEstado) {
	log_trace(logYamaImpreso, "job: %d, master: %d, nodo: %s, bloque: %d, etapa: %s, archivo temporal: %s, estado: %s",
			idJob,
			idMaster,
			unEstado->nodoPlanificado->nodo->idNodo,
			unEstado->nodoPlanificado->bloque->numero_bloque,
			"transformacion",
			unEstado->archivoTemporal, unEstado->estado);
}

void logEstadoReduccionLocal(int idJob, int idMaster, t_estado* estadoTransformacion, t_estado* estadoReduccionLocal) {
	log_trace(logYamaImpreso, "job: %d, master: %d, nodo: %s, bloque: %d, etapa: %s, archivo temporal: %s, estado: %s",
			idJob,
			idMaster,
			estadoReduccionLocal->nodoPlanificado->nodo->idNodo,
			estadoTransformacion->nodoPlanificado->bloque->numero_bloque,
			"reduccion local",
			estadoReduccionLocal->archivoTemporal,
			estadoReduccionLocal->estado);
}

void logEstadoReduccionGlobal(int idJob, int idMaster, t_estado* estadoReduccionGlobal){
	log_trace(logYamaImpreso, "job: %d, master: %d, nodo: %s, bloque: %s, etapa: %s, archivo temporal: %s, estado: %s",
			idJob,
			idMaster,
			estadoReduccionGlobal->nodoPlanificado->nodo->idNodo,
			"----",
			"reduccion global",
			estadoReduccionGlobal->archivoTemporal,
			estadoReduccionGlobal->estado);
}

void logEstadoAlmacenadoFinal(t_job* job){

	int idJob = job->idJob;
	int idMaster = job->idMaster;

	log_trace(logYamaImpreso, "job: %d, master: %d, nodo: %s, bloque: %s, etapa: %s, archivo temporal: %s, estado: %s",
			idJob,
			idMaster,
			job->reduccionGlobal->nodoPlanificado->nodo->idNodo,
			"----",
			"almacenado final",
			"----",
			job->estadoAlmacenado);
}


void enProcesoReduccionLocal(char* idNodo, t_job* job){
	t_estado* estadoRedLoc = obtenerEstadoRedLoc(job->estadosReduccionesLocales, idNodo);
	strcpy(estadoRedLoc->estado, "en proceso");
}

void enProcesoReduccionGlobal(t_job* job){
	t_estado* estadoRedLoc = job->reduccionGlobal;
	strcpy(estadoRedLoc->estado, "en proceso");
}

void enProcesoAlmacenadoFinal(t_job* job){
	strcpy(job->estadoAlmacenado, "en proceso");
}


/*
 * funcion muy generica para todos los estados
 *
 * En particular:
 * si se actualiza el estado de la reduccion global con ok
 * poner "esperando almacenado"
 * y si se pone de actualizar el estado de almacenado final a ok
 * modificar el estado de reduccion global a "job finalizado
 */
void actualizarEstado(char idNodo[NOMBRE_NODO], int numero_bloque, int etapa, int idJob, int resultado){
	t_job* job;
	t_estado* estado;

	job = obtenerJob(idJob, jobsActivos);

	switch(etapa){
	case RESULTADO_TRANSFORMACION:
		estado = obtenerEstadoTransformacion(job->estadosTransformaciones, idNodo, numero_bloque);
		if(resultado == TRANSFORMACION_OK){
			strcpy(estado->estado, "finalizado");
			logEstadoTransformacion(idJob, job->idMaster, estado);
		}else{
			strcpy(estado->estado, "error");
		}
		break;

	case RESULTADO_REDUCCION_LOCAL:
		estado = obtenerEstadoRedLoc(job->estadosReduccionesLocales, idNodo);

		if(resultado == REDUCCION_LOCAL_OK){
			strcpy(estado->estado, "finalizado");
			int j;
			for(j=0; j<list_size(job->estadosTransformaciones); j++){
				t_estado* estadoTransformacion = list_get(job->estadosTransformaciones, j);
				if(strcmp(estado->nodoPlanificado->nodo->idNodo, estadoTransformacion->nodoPlanificado->nodo->idNodo) == 0){
					logEstadoReduccionLocal(idJob, job->idMaster, estadoTransformacion, estado);
				}
			}
		}else{
			strcpy(estado->estado, "error");
		}
		break;

	case RESULTADO_REDUCCION_GLOBAL:
		estado = job->reduccionGlobal;
		if(resultado == REDUCCION_GLOBAL_OK){
			strcpy(estado->estado, "finalizado");
			logEstadoReduccionGlobal(idJob, job->idMaster, estado);
		}else{
			strcpy(estado->estado, "error");
		}
		break;
	case RESULTADO_ALMACENADO_FINAL:
		if(resultado == ALMACENADO_FINAL_OK){
			strcpy(job->estadoAlmacenado, "job exitoso");
			logEstadoAlmacenadoFinal(job);
		}else{
			strcpy(job->estadoAlmacenado, "error almacenado final");
		}
		break;
	}
}

