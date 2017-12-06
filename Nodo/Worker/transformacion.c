/*
 * transformacion.c
 *
 *  Created on: 4/11/2017
 *      Author: utnso
 */

#include "etapas.h"
#include <commons/log.h>
#include <commons/string.h>
#include <sys/mman.h>
#include "../../bibliotecas/sockets.c"
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"

int transformacion(solicitud_programa_transformacion solicitudDeserializada, char* rutaNodo){

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
	retorno = persistirPrograma(solicitudDeserializada.programa_transformacion, solicitudDeserializada.programa);
	if(retorno == -1 || retorno == -2 || retorno == -10){
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		return retorno;
	}

	//abro el data.bin
	f1 = fopen (rutaNodo, "rb");
	if (f1==NULL)
	{
		log_error(worker_error_log, "No se pudo abrir data.bin");
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		return -3;
	}

	log_trace(worker_log, "Archivo data.bin abierto");

	int fd = fileno(f1);

	struct stat filestat;
	if(fstat(fd, &filestat) < 0){

		log_error(worker_error_log, "Error fstat");
		return -3;

	}

	unsigned char* map =
			(unsigned char*) mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, sizeof(unsigned char)*solicitudDeserializada.bloque*TAMANIO_BLOQUE);

	//buffer donde pongo datos que leo del bloque del data.bin
	char* buffer = malloc(solicitudDeserializada.bytes_ocupados);
	int i;

	for(i = 0; i<solicitudDeserializada.bytes_ocupados; i++){

		buffer[i] = map[i];

	}

	munmap(map, filestat.st_size);

	log_trace(worker_log, "Bloque leido");

	fclose(f1);

	//puntero que va a tener la cadena de caracteres que se le pasa a la funcion system para ejecutar el script
	char* s = string_from_format("printf \"%s\" | .\"/scripts/%s\" | sort > \"%s\"", buffer,
			solicitudDeserializada.programa_transformacion, solicitudDeserializada.archivo_temporal);
	free(buffer);
	retorno = system(s);
	if(retorno == -1){
		log_error(worker_error_log, "No se pudo realizar la transformacion");
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		free(s);
		return -10;
	}

	free(s);

	log_trace(worker_log, "Transformacion de bloque finalizada");
	log_destroy(worker_log);
	log_destroy(worker_error_log);

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
		log_error(worker_error_log, "Se envia a Master el error de creacion del programa de transformacion");
		enviarMensajeSocketConLongitud(socket, TRANSFORMACION_ERROR_CREACION, NULL, 0);
		break;
	case -2:
		log_error(worker_error_log, "Se envia a Master el error de escritura del contenido del programa de transformacion");
		enviarMensajeSocketConLongitud(socket, TRANSFORMACION_ERROR_ESCRITURA, NULL, 0);
		break;
	case -3:
		log_error(worker_error_log, "Se envia a Master el error al acceder a los datos del archivo del data.bin");
		enviarMensajeSocketConLongitud(socket, FSTAT_ERROR, NULL, 0);
		break;
	case -10:
		log_error(worker_error_log, "Se envia a Master el error al dar permisos de ejecucion al programa de transformacion");
		enviarMensajeSocketConLongitud(socket, TRANSFORMACION_ERROR_PERMISOS, NULL, 0);
		break;

	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);

}

