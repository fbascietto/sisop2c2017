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

	t_list* lista_de_RG = list_create();

	//preparacion para aparear
	for(i=0; i<solicitudDeserializada->cantidad_item_programa_reduccion; i++){

		prepararParaApareo(lista_de_RG, solicitudDeserializada->workers[i]);

	}

	aparear(lista_de_RG);

	char* contenido = contenido_de_archivo(ruta_archivo_temp_final);

	char* comando = string_from_format("printf \"%s\" | .\"/scripts/%s\" > \"%s\"", contenido,
			solicitudDeserializada->programa_reduccion, ruta_archivo_temp_final);

	system(comando);

	free(comando);
	list_destroy_and_destroy_elements(lista_de_RG, free);

	return 0;

}

int recorrerArchivo(char ruta_arch_temp[LENGTH_RUTA_ARCHIVO_TEMP]){

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

	buffer = malloc(longitud_archivo_temporal);

	while(!feof(f1)){

		//leo de a un registro (una linea porque ya viene ordenado el archivo) para guardar en buffer y enviar
		fgets(buffer, longitud_archivo_temporal, f1);

		ultima_palabra->fin_de_archivo = false;
		ultima_palabra->palabra = buffer;

		//todo: definir manera que se activa el ciclo nuevamente

	}

	log_trace(worker_log, "Archivo de reduccion local llego al eof");
	ultima_palabra->fin_de_archivo = true;
	ultima_palabra->palabra = "";

	free(buffer);

	log_destroy(worker_log);
	log_destroy(worker_error_log);

	return 0;

}


int leerYEnviarArchivoTemp(char ruta_arch_temp[LENGTH_RUTA_ARCHIVO_TEMP], int socket){

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

	buffer = malloc(longitud_archivo_temporal);

	while(!feof(f1)){

		//leo de a un registro (una linea porque ya viene ordenado el archivo) para guardar en buffer y enviar
		fgets(buffer, longitud_archivo_temporal, f1);

		log_trace(worker_log, "Se envia al worker encargado un registro para la reduccion global");
		solicitud_recibir_palabra* respuesta;
		respuesta->fin_de_archivo = false;
		respuesta->palabra = buffer;
		char* serialized = serializarSolicitudRecibirPalabra(respuesta);
		enviarMensajeSocket(socket, ACCION_RECIBIR_PALABRA, serialized);

		recibirSolicitudWorker(socket);


	}

	solicitud_recibir_palabra* respuesta_fin;
	respuesta_fin->fin_de_archivo = true;
	respuesta_fin->palabra = "";
	char* serialized_fin = serializarSolicitudRecibirPalabra(respuesta_fin);
	enviarMensajeSocket(socket, ACCION_RECIBIR_PALABRA, serialized_fin);

	free(buffer);

	log_destroy(worker_log);
	log_destroy(worker_error_log);

	return 0;

}


void prepararParaApareo(t_list* elementos_para_RG, t_worker worker){

	t_elemento* unElemento = malloc(sizeof(t_elemento));

	char* ip = malloc(LENGTH_IP);
	strcpy(ip, worker.ip_worker);

	//todo: verificar nombre
	if(worker.nodo_id == 0){
		unElemento->socket = VALOR_SOCKET_WE;
		pid_t pid = fork();
		if(pid == 0){
			recorrerArchivo(worker.archivo_temporal_reduccion_local);
		}
	}else{
		unElemento->socket = conectarseA(ip, worker.puerto_worker);
		enviarMensajeSocketConLongitud(unElemento->socket, COMENZAR_REDUCCION_GLOBAL, NULL, 0);
	}

	unElemento->ultima_palabra = "";
	unElemento->worker = worker;
	unElemento->pedir = true;
	unElemento->fin = false;

	list_add(elementos_para_RG, unElemento);



}

solicitud_recibir_palabra* recibirPalabra(int socket){

	solicitud_recibir_palabra* palabra;

	if(socket != VALOR_SOCKET_WE){

		palabra = recibirSolicitudWorker(socket);
		enviarMensajeSocketConLongitud(socket, CONTINUAR_ENVIO, NULL, 0);

	}else{

		palabra = ultima_palabra;
		habilitarSemaforo();

	}

	return palabra;

}

void escribirEnArchivo(char* palabra_a_escribir){

	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	FILE* f1;

	f1 = fopen(ruta_archivo_temp_final, "r+");
	if(f1 == NULL){
		f1 = fopen(ruta_archivo_temp_final, "w");
		if(f1 == NULL){
			log_error(worker_error_log, "No se pudo abrir el archivo de reduccion global para escritura");
		}
	}

	fseek(f1, 0, SEEK_END);

	fputs(palabra_a_escribir, f1);

	fclose(f1);

	log_destroy(worker_error_log);

}

bool esMenor(char* cadena1, char* cadena2){

	return cadena1 < cadena2;

}

void aparear(t_list* lista){

	char* palabraCandidata;

	bool termino(t_elemento* elemento){

		return elemento->fin;

	}

	void procesarElemento(t_elemento* elemento){

		//si aun no termino y hay que pedir
		if(!elemento->fin && elemento->pedir){

			solicitud_recibir_palabra* respuesta = recibirPalabra(elemento->socket);
			elemento->fin = respuesta->fin_de_archivo; //
			elemento->ultima_palabra = respuesta->palabra;
			elemento->pedir = false;

		}

		if(!elemento->fin && esMenor(elemento->ultima_palabra, palabraCandidata)){
			palabraCandidata = elemento->ultima_palabra;
			elemento->pedir = true;
		}

	}

	while(list_any_satisfy(lista, !termino)){

		list_iterate(lista, procesarElemento);
		escribirEnArchivo(palabraCandidata);

	}


}


void habilitarSemaforo(){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* sem_log = log_create("logWorker.txt", "WORKER", 1, level);

	log_trace(sem_log, "Se habilita el semaforo para enviar el siguiente registro");
	signal(sem);

}

char* contenido_de_archivo(char ruta[LENGTH_RUTA_ARCHIVO_TEMP]){

	FILE* f1 = fopen(ruta, "r");
	if(f1 == NULL){
		char* error = "";
		return error;
	}

	fseek(f1, 0, SEEK_END);
	int longitud = ftell(f1);
	rewind(f1);

	char* buffer = malloc(longitud);

	fread(buffer, 1, longitud, f1);

	fclose(f1);

	return buffer;

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
		//enviar ERROR de creacion de programa de reduccion
		break;
	case -2:
		//enviar ERROR de escritura de programa de reduccion
		break;
	case -10:
		//enviar ERROR de llamada system() al darle permisos al script
		break;
	case -9:
		log_error(worker_error_log, "Se envia aviso de error en etapa de reduccion global a Master");
		enviarMensajeSocketConLongitud(socket, REDUCCION_GLOBAL_ERROR, NULL, 0);
		break;
	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);

}
