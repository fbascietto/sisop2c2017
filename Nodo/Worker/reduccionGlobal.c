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
	string_append(ruta_archivo_apareo, solicitudDeserializada->archivo_temporal_resultante);
	string_append(ruta_archivo_apareo, "-Apareo");

	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	//para recorrer array
	int i;
	//para realizar conexion con otros workers
	int socket;

	//retorno de la funcion que persiste el programa de reduccion
	int retorno;

	//persisto el programa reductor
	retorno = persistirPrograma(solicitudDeserializada->programa_reduccion, solicitudDeserializada->programa);
	if(retorno != 0){
		return retorno;
	}

	t_list* lista_de_RG = list_create();

	//preparacion para aparear
	for(i=0; i<solicitudDeserializada->cantidad_workers; i++){

		prepararParaApareo(lista_de_RG, &(solicitudDeserializada->workers[i]), i);

	}

	retorno = aparear(lista_de_RG);
	if(retorno != 0){
		return retorno;
	}

	char* comando = string_from_format("cat \"%s\" | .\"/scripts/%s\" > \"%s\"", ruta_archivo_apareo,
			solicitudDeserializada->programa_reduccion, ruta_archivo_temp_final);

	retorno = system(comando);
	if(retorno == -1){
		log_error(worker_error_log, "No se pudo realizar la reduccion global");
		log_destroy(worker_error_log);
		free(comando);
		list_destroy_and_destroy_elements(lista_de_RG, free);
		return -4;
	}

	free(comando);
	free(ruta_archivo_apareo);

	list_destroy_and_destroy_elements(lista_de_RG, free);

	return 0;

}


void prepararParaApareo(t_list* elementos_para_RG, t_worker* worker, int posicion){

	t_elemento* unElemento = malloc(sizeof(t_elemento));

	unElemento->socket = conectarseA(worker->ip_worker, worker->puerto_worker);
	solicitud_leer_y_enviar_archivo_temp* solicitud = malloc(sizeof(solicitud_leer_y_enviar_archivo_temp));
	strcpy(solicitud->ruta_archivo_red_local_temp, worker->archivo_temporal_reduccion_local);
	char* serialized = serializar_solicitud_leer_y_enviar_archivo_temp(solicitud);
	enviarInt(unElemento->socket, PROCESO_WORKER);
	enviarMensajeSocket(unElemento->socket, COMENZAR_REDUCCION_GLOBAL, serialized);
	free(solicitud);

	unElemento->ultima_palabra = "";
	unElemento->worker = worker;
	unElemento->pedir = true;
	unElemento->fin = false;
	unElemento->posicion = posicion;

	list_add(elementos_para_RG, unElemento);


}

int leerYEnviarArchivoTemp(char* ruta_arch_temp, int socket){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	//fichero para recorrer el archivo de reduccion local
	FILE* f1;

	//buffer donde pongo cada registro (linea) que voy a enviar
	char* buffer;
	//entero donde almaceno longitud del archivo para reservar memoria en el malloc
	int longitud_archivo_temporal;
	int retorno;

	//estructura que se envia al worker encargado
	solicitud_recibir_palabra* respuesta;

	f1 = fopen(ruta_arch_temp, "r");
	if(f1 == NULL){
		log_error(worker_error_log, "No se pudo abrir el archivo temporal de reduccion local para recorrerlo");
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		return -3;
	}

	//me posiciono al final del archivo
	retorno = fseek(f1, 0, SEEK_END);
	if(retorno!=0){
		log_error(worker_error_log, "No se pudo posicional al final del archivo temporal");
		log_destroy(worker_log);
		log_destroy(worker_error_log);
		return -4;
	}
	//determino longitud del archivo
	longitud_archivo_temporal = ftell(f1);
	//me posiciono al principio del archivo para poder leer
	rewind(f1);

	char* serialized; //char* donde se pone la estructura serializada para enviar

	buffer = malloc(longitud_archivo_temporal);

	while(!feof(f1)){

		//leo de a un registro (una linea porque ya viene ordenado el archivo) para guardar en buffer y enviar
		fgets(buffer, longitud_archivo_temporal, f1);

		log_trace(worker_log, "Se envia al worker encargado un registro para la reduccion global");
		respuesta = realloc(respuesta, sizeof(solicitud_recibir_palabra));
		respuesta->fin_de_archivo = false;
		respuesta->palabra = buffer;
		serialized = serializarSolicitudRecibirPalabra(respuesta);
		enviarMensajeSocket(socket, ACCION_RECIBIR_PALABRA, serialized);

		recibirSolicitudWorker(socket);


	}

	log_trace(worker_log, "Archivo de reduccion local llego al eof");

	solicitud_recibir_palabra* respuesta_fin = malloc(sizeof(solicitud_recibir_palabra));
	respuesta_fin->fin_de_archivo = true;
	respuesta_fin->palabra = "";
	char* serialized_fin = serializarSolicitudRecibirPalabra(respuesta_fin);
	enviarMensajeSocket(socket, ACCION_RECIBIR_PALABRA, serialized_fin);

	free(serialized);
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
	enviarMensajeSocketConLongitud(socket, CONTINUAR_ENVIO, NULL, 0);

	return palabra;

}

int escribirEnArchivo(char* palabra_a_escribir){

	int retorno;

	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	FILE* f1;

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

	if(strcmp(cadena1, cadena2) < 0 || cadena2 == "") return true;
	return false;

}

bool termino(void* unElemento){

	t_elemento* elemento = (t_elemento*) unElemento;

	return elemento->fin;

}

bool algunoNoTermino(t_list* lista){

	bool resultado = false;
	t_elemento* elemento;
	int i;

	for(i=0; i < list_size(lista); i++){

		elemento = list_get(lista, i);
		resultado = resultado || !termino(elemento);

	}

	return resultado;

}

void procesarElemento(void* unElemento){

	t_elemento* elemento = (t_elemento*) unElemento;

	if(!elemento->fin && elemento->pedir){

		solicitud_recibir_palabra* respuesta = recibirPalabra(elemento->socket);
		elemento->fin = respuesta->fin_de_archivo;
		elemento->ultima_palabra = respuesta->palabra;
		elemento->pedir = false;

	}

	if(!elemento->fin && esMenor(elemento->ultima_palabra, palabraCandidata)){
		palabraCandidata = elemento->ultima_palabra;
		posicionCandidata = elemento->posicion;

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
		list_replace(lista, posicionCandidata, elegido);
		retorno = escribirEnArchivo(palabraCandidata);
		if(retorno != 0){
			log_error(worker_error_log, "El apareo no fue exitoso");
			return -3;
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
		log_error(worker_error_log, "Se envia a Master el error de creacion del programa de reduccion");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_ERROR_CREACION, NULL, 0);
		break;
	case -2:
		log_error(worker_error_log, "Se envia a Master el error de escritura del contenido del programa de reduccion");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_ERROR_ESCRITURA, NULL, 0);
		break;
	case -3:
		log_error(worker_error_log, "Se envia a Master el error en el apareo");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_ERROR_APAREO, NULL, 0);
		break;
	case -4:
		log_error(worker_error_log, "Se envia a Master el error en la llamada system para terminar la reduccion global");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_ERROR_SYSTEM, NULL, 0);
		break;
	case -10:
		log_error(worker_error_log, "Se envia a Master el error al dar permisos de ejecucion al programa de reduccion");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_ERROR_PERMISOS, NULL, 0);
		break;

	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);

}
