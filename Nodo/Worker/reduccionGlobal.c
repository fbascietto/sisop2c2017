/*
 * reduccionGlobal.c
 *
 *  Created on: 4/11/2017
 *      Author: utnso
 */

#include "etapas.h"
#include <commons/log.h>
#include <commons/string.h>
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"


int reduccionGlobal(solicitud_programa_reduccion_global* solicitudDeserializada){

	ruta_archivo_apareo = string_new();
	string_append(&ruta_archivo_apareo, "Apareo-");
	string_append(&ruta_archivo_apareo, basename(solicitudDeserializada->archivo_temporal_resultante));

	printf("Soy proceso: %d. La ruta del archivo de apareo despues de generarse es: %s\n", getpid(), ruta_archivo_apareo);

	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	//para recorrer array
	int i;

	//retorno de la funcion que persiste el programa de reduccion
	int retorno;

	t_list* lista_de_RG = list_create();

	//preparacion para aparear

	for(i=0; i<solicitudDeserializada->cantidad_workers; i++){

		prepararParaApareo(lista_de_RG, &(solicitudDeserializada->workers[i]));

	}
	retorno = aparear(lista_de_RG);
	//retorno = 0;
	if(retorno != 0){
		return retorno;
	}

	char* comando = string_from_format("cat \"%s\" | .\"/scripts/%s\" > \"%s\"", ruta_archivo_apareo,
			solicitudDeserializada->programa_reduccion, ruta_archivo_temp_final);

	vaciarArchivo(ruta_archivo_temp_final);

	retorno = system(comando);
	if(retorno == -1){
		log_error(worker_error_log, "No se pudo realizar la reduccion global");
		log_destroy(worker_error_log);
		free(comando);
		free(ruta_archivo_apareo);
		list_destroy_and_destroy_elements(lista_de_RG, free);
		return -4;
	}

	free(comando);
	free(ruta_archivo_apareo);

	list_destroy_and_destroy_elements(lista_de_RG, free);

	return 0;

}


void prepararParaApareo(t_list* elementos_para_RG, t_worker* worker){

	t_elemento* unElemento = malloc(sizeof(t_elemento));

	unElemento->socket = conectarseA(worker->ip_worker, worker->puerto_worker);

	solicitud_leer_y_enviar_archivo_temp* solicitud = malloc(sizeof(solicitud_leer_y_enviar_archivo_temp));
	strcpy(solicitud->ruta_archivo_red_local_temp, worker->archivo_temporal_reduccion_local);
	char* serialized = serializar_solicitud_leer_y_enviar_archivo_temp(solicitud);
	enviarInt(unElemento->socket, PROCESO_WORKER);
	enviarMensajeSocket(unElemento->socket, COMENZAR_REDUCCION_GLOBAL, serialized);

	free(solicitud);
	free(serialized);

	unElemento->worker = worker;
	unElemento->pedir = true;
	unElemento->fin = false;
	unElemento->ultima_palabra = malloc(LENGTH_PALABRA);
	unElemento->posicion = list_add(elementos_para_RG, unElemento);



}

int leerYEnviarArchivoTemp(char* ruta_arch_temp, int socket){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	int enviados;

	//fichero para recorrer el archivo de reduccion local
	FILE* f1;

	//buffer donde pongo cada registro (linea) que voy a enviar
	char* buffer;
	//entero donde almaceno longitud del archivo para reservar memoria en el malloc
	int retorno;

	f1 = fopen(ruta_arch_temp, "r");
	if(f1 == NULL){
		log_error(worker_error_log, "No se pudo abrir el archivo temporal de reduccion local para recorrerlo");
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		return -3;
	}

	char* serialized; //char* donde se pone la estructura serializada para enviar

	buffer = malloc(LENGTH_PALABRA);

	//estructura que se envia al worker encargado
	solicitud_recibir_palabra* respuesta = malloc(sizeof(solicitud_recibir_palabra));

	while(1){

		//leo de a un registro (una linea porque ya viene ordenado el archivo) para guardar en buffer y enviar
		if((fgets(buffer, LENGTH_PALABRA, f1)) == NULL) break;

		log_trace(worker_log, "Se envia al worker encargado un registro para la reduccion global");
		respuesta->fin_de_archivo = false;
		strcpy(respuesta->palabra, buffer);

		printf("Soy proceso: %d. palabra a enviar en el socket: %d es: %s\n", getpid(), socket, respuesta->palabra);

		serialized = serializarSolicitudRecibirPalabra(respuesta);
		uint32_t total_size = getLong_SolicitudRecibirPalabra(respuesta);
		enviados = enviarMensajeSocketConLongitud(socket, ACCION_RECIBIR_PALABRA, serialized, total_size);

		free(serialized);

		int recibido;
		recibirInt(socket, &recibido);
		if(recibido != CONTINUAR_ENVIO){
			printf("Soy proceso: %d. Codigo de mensaje invalido en reduccion global: %d. Socket: %d\n", getpid(), recibido, socket);
		}


	}

	log_trace(worker_log, "Archivo de reduccion local llego al eof");

	solicitud_recibir_palabra* respuesta_fin = malloc(sizeof(solicitud_recibir_palabra));
	respuesta_fin->fin_de_archivo = true;
	strcpy(respuesta_fin->palabra, "");
	char* serialized_fin = serializarSolicitudRecibirPalabra(respuesta_fin);
	uint32_t total_size_fin = getLong_SolicitudRecibirPalabra(respuesta_fin);
	enviarMensajeSocketConLongitud(socket, ACCION_RECIBIR_PALABRA, serialized_fin, total_size_fin);

	free(serialized_fin);
	free(buffer);
	free(respuesta);
	free(respuesta_fin);

	fclose(f1);

	log_destroy(worker_log);
	log_destroy(worker_error_log);

	return 0;

}

solicitud_recibir_palabra* recibirPalabra(int socket){

	solicitud_recibir_palabra* palabra;
	palabra = recibirSolicitudWorker(socket);
	enviarInt(socket, CONTINUAR_ENVIO);

	return palabra;

}

int escribirEnArchivo(char* palabra_a_escribir){

	int retorno;

	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	FILE* f1;

	printf("Soy proceso: %d. La ruta del archivo de apareo a la hora de escribir es: %s\n", getpid(), ruta_archivo_apareo);

	f1 = fopen(ruta_archivo_apareo, "r+");
	if(f1 == NULL){
		f1 = fopen(ruta_archivo_apareo, "w");
		if(f1 == NULL){
			log_error(worker_error_log, "No se pudo abrir el archivo temporal de apareo para escritura");
			return -1;
		}
	}

	fseek(f1, 0, SEEK_END);

	retorno = fputs(palabra_a_escribir, f1);
	if(retorno == feof(f1)){
		log_error(worker_error_log, "No se pudo escribir la palabra en el archivo");
		fclose(f1);
		log_destroy(worker_error_log);
		return -2;
	}

	fclose(f1);

	log_destroy(worker_error_log);

	return 0;

}

bool esMenor(char* cadena1, char* cadena2){

	printf("Se compara %s con la palabra candidata: %s", cadena1, cadena2);

	if(strcmp(cadena1, cadena2) < 0 || !strcmp(cadena2, "")) return true;
	return false;

}

bool termino(t_elemento* elemento){

	return elemento->fin;

}

bool algunoNoTermino(t_list* lista){

	t_elemento* elemento;
	int i;

	for(i=0; i < list_size(lista); i++){

		elemento = list_get(lista, i);
		if(!termino(elemento)) return true;

	}

	return false;

}

void procesarElemento(void* unElemento){

	t_elemento* elemento = (t_elemento*) unElemento;

	printf("ultima palabra del elemento %s antes del chequeo %s ", elemento->worker->nodo_id, elemento->ultima_palabra);
	//verifica si hay que pedir palabra
	if(!elemento->fin && elemento->pedir){

		solicitud_recibir_palabra* respuesta = recibirPalabra(elemento->socket);
		elemento->fin = respuesta->fin_de_archivo;
		strcpy(elemento->ultima_palabra, respuesta->palabra);
		elemento->pedir = false;

		free(respuesta);

	}

	printf("palabra del elemento %s despues del chequeo %s ", elemento->worker->nodo_id, elemento->ultima_palabra);
	//compara con la palabra candidata
	if(!elemento->fin && esMenor(elemento->ultima_palabra, palabraCandidata)){
		palabraCandidata = elemento->ultima_palabra;
		posicionCandidata = elemento->posicion;
		printf("Se cambio la palabra candidata a: %s\n", palabraCandidata);

	}

}

int aparear(t_list* lista){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_error = LOG_LEVEL_ERROR;
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_error);
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);

	int retorno;

	t_elemento* elegido;

	//siempre y cuando haya algun elemento de la lista que falte terminar de recorrer el archivo
	while(algunoNoTermino(lista)){

		palabraCandidata = "";

		list_iterate(lista, procesarElemento);
		elegido = list_get(lista, posicionCandidata);
		elegido->pedir = true;
		retorno = escribirEnArchivo(palabraCandidata);
		if(retorno != 0){
			log_error(worker_error_log, "El apareo no fue exitoso");
			return -3;
		}

	}

	list_iterate(lista, liberarUltimaPalabra);

	return 0;

}

void liberarUltimaPalabra(void* arg){
	t_elemento* unElemento = (t_elemento*) arg;

	free(unElemento->ultima_palabra);
}

void responderSolicitudRG(int socket, int exit_code){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	printf("Soy proceso: %d. Numero socket: %d. Exit code: %d\n", getpid(), socket, exit_code);

	int enviados;

	switch(exit_code){

	case 0:
		log_trace(worker_log, "Se envia confirmacion de finalizacion de etapa de reduccion global a Master");
		enviados = enviarInt(socket, REDUCCION_GLOBAL_OK);
		printf("Soy proceso: %d. Bytes enviados a Master: %d\n", getpid(), enviados);
		break;
	case -1:
		log_error(worker_error_log, "Se envia a Master el error de creacion del programa de reduccion");
		enviados = enviarInt(socket, REDUCCION_GLOBAL_ERROR_CREACION);
		printf("Soy proceso: %d. Bytes enviados a Master: %d\n", getpid(), enviados);
		break;
	case -2:
		log_error(worker_error_log, "Se envia a Master el error de escritura del contenido del programa de reduccion");
		enviados = enviarInt(socket, REDUCCION_GLOBAL_ERROR_ESCRITURA);
		printf("Soy proceso: %d. Bytes enviados a Master: %d\n", getpid(), enviados);
		break;
	case -3:
		log_error(worker_error_log, "Se envia a Master el error en el apareo");
		enviados = enviarInt(socket, REDUCCION_GLOBAL_ERROR_APAREO);
		printf("Soy proceso: %d. Bytes enviados a Master: %d\n", getpid(), enviados);
		break;
	case -4:
		log_error(worker_error_log, "Se envia a Master el error en la llamada system para terminar la reduccion global");
		enviados = enviarInt(socket, REDUCCION_GLOBAL_ERROR_SYSTEM);
		printf("Soy proceso: %d. Bytes enviados a Master: %d\n", getpid(), enviados);
		break;
	case -10:
		log_error(worker_error_log, "Se envia a Master el error al dar permisos de ejecucion al programa de reduccion");
		enviados = enviarInt(socket, REDUCCION_GLOBAL_ERROR_PERMISOS);
		printf("Soy proceso: %d. Bytes enviados a Master: %d\n", getpid(), enviados);
		break;

	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);

}
