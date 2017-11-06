/*
 * reduccionGlobal.c
 *
 *  Created on: 4/11/2017
 *      Author: utnso
 */

#include "etapas.h"
#include <commons/log.h>
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"

int reduccionGlobal(solicitud_programa_reduccion_global* solicitudDeserializada, int puerto){

	//para recorrer array
	int i;
	//para realizar conexion con otros workers
	int socket;

	//retorno de la funcion que persiste el programa de reduccion
	int retorno;

	//persisto el programa reductor
	retorno = persistirPrograma(solicitudDeserializada->programa_reduccion, solicitudDeserializada->programa);
	if(retorno == -1 || retorno == -2 || retorno == -10){
		return retorno;
	}

	//para ver si uno de los archivos de reduccion local reside en el worker encargado
	for(i=0; i<solicitudDeserializada->cantidad_item_programa_reduccion; i++){

		if(puerto == solicitudDeserializada->items_programa_reduccion_global[i].puerto_worker){

			//se utiliza un archivo temporal local

		}else{

			//se debe pedir los registros mediante puerto/ip
			socket = conectarseA(solicitudDeserializada->items_programa_reduccion_global[i].ip_worker,
									solicitudDeserializada->items_programa_reduccion_global[i].puerto_worker);
			enviarInt(socket, PROCESO_WORKER);
			enviarMensajeSocketConLongitud(socket, ACCION_ENVIAR_ARCHIVO_TEMP_DE_RL, NULL, 0);

		}

	}

	return 0;
}

void responderSolicitudRG(int socket, int exit_code){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	switch(exit_code){

	case 0:
		log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de reduccion global a Master");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_OK, NULL, 0);
		break;
	case -1:
		log_error(worker_error_log, "Se envia aviso de error en etapa de reduccion global a Master");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_ERROR, NULL, 0);
		break;
	}

}


void enviarArchivoTemp(solicitud_enviar_archivo_temp* solicitudDeserializada){

}

void leerArchivoTemp(solicitud_leer_archivo_temp* solicitudDeserializada){

}

