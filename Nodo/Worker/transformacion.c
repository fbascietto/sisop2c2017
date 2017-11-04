/*
 * transformacion.c
 *
 *  Created on: 4/11/2017
 *      Author: utnso
 */

#include "transformacion.h"

int transformacion(solicitud_programa_transformacion* solicitudDeserializada){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	//fichero para leer el data.bin
	FILE* f1;
	int leidos;

	//retorno de la funcion que persiste el programa de transformacion
	int retorno;

	//persisto el programa transformador
	retorno = persistirPrograma(solicitudDeserializada->programa_transformacion, solicitudDeserializada->programa);
	if(retorno == -1 || retorno == -2 || retorno == -10){
		return retorno;
	}

	//abro el data.bin
	f1 = fopen (rutaNodo, "rb");
	if (f1==NULL)
	{
		log_error(worker_error_log, "No se pudo abrir data.bin");
		return -3;
	}

	log_trace(worker_log, "Archivo data.bin abierto");

	//me desplazo hasta el bloque que quiero leer
	fseek(f1, TAMANIO_BLOQUE*solicitudDeserializada->bloque, SEEK_SET);

	//buffer donde pongo datos que leo del bloque del data.bin
	char* buffer = malloc(solicitudDeserializada->bytes_ocupados + 1);

	//leer bloque de archivo
	leidos = fread(buffer, 1, solicitudDeserializada->bytes_ocupados, f1);
	if(leidos!= solicitudDeserializada->bytes_ocupados){
		log_error(worker_error_log, "No se leyo correctamente el bloque");
		free(buffer);
		return -4;
	}

	log_trace(worker_log, "Bloque leido");

	fclose(f1);

	//puntero que va a tener la cadena de caracteres que se le pasa a la funcion system para ejecutar el script
	char* s = malloc(solicitudDeserializada->bytes_ocupados + LENGTH_NOMBRE_PROGRAMA + LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_EXTRA_SPRINTF + 1);

	//meto en s lo que quiero pasarle a system para que ejecute el script
	sprintf(s, "echo %s | .\"/scripts/%s\" | sort > \"%s\"", buffer, solicitudDeserializada->programa_transformacion, solicitudDeserializada->archivo_temporal);
	retorno = system(s);
	if(retorno == -1){
		log_error(worker_error_log, "No se pudo realizar la transformacion");
		return -10;
	}

	free(buffer);
	free(s);

	log_trace(worker_log, "Transformacion de bloque finalizada");

	return 0;

}

void responderSolicitudT(int socket, int exit_code){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	switch(exit_code){

	case 0:
		log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de transformacion de un bloque a Master");
		enviarMensajeSocketConLongitud(socket, TRANSFORMACION_OK, NULL, 0);
		break;
	case -1:
		//enviar ERROR de creacion de programa de transformacion
		break;
	case -2:
		//enviar ERROR de escritura de programa de transformacion
		break;
	case -3:
		//enviar ERROR de apertura de data.bin
		break;
	case -4:
		//enviar ERROR de lectura de data.bin
		break;
	case -10:
		//enviar ERROR de llamada system()
		break;
	case -5:
		log_error(worker_error_log, "Se envia aviso de error en etapa de transformacion de un bloque a Master");
		enviarMensajeSocketConLongitud(socket, TRANSFORMACION_ERROR, NULL, 0);
		break;

	}
}

