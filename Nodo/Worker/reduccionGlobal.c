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

	t_list* elementos_de_RG = list_create();

	//preparacion de estructuras para preparar la reduccion global
	for(i=0; i<solicitudDeserializada->cantidad_item_programa_reduccion; i++){

		prepararEstructuras(elementos_de_RG, solicitudDeserializada->workers[i], i);

	}

	t_elemento* element_with_socket_to_add;

	//para ver si uno de los archivos de reduccion local reside en el worker encargado
	for(i=0; i<solicitudDeserializada->cantidad_item_programa_reduccion; i++){

		int nodoID; //solo lo pongo para que compile, la idea es que compare con el nombre del nodo
		//verificacion de si es worker encargado
		if(nodoID == solicitudDeserializada->workers[i].nodo_id){

			element_with_socket_to_add = list_get(elementos_de_RG, i);
			element_with_socket_to_add->socket = VALOR_SOCKET_WE;
			list_replace(elementos_de_RG, i, element_with_socket_to_add);

			leerYEnviarArchivoTemp(solicitudDeserializada->workers[i].archivo_temporal_reduccion_local, VALOR_SOCKET_WE);

		}else{

			//se debe pedir los registros mediante puerto/ip
			socket = conectarseA(solicitudDeserializada->workers[i].ip_worker,
					solicitudDeserializada->workers[i].puerto_worker);

			element_with_socket_to_add = list_get(elementos_de_RG, i);
			element_with_socket_to_add->socket = socket;
			list_replace(elementos_de_RG, i, element_with_socket_to_add);

			enviarInt(socket, PROCESO_WORKER);
			//Todo: enviar la ruta del archivo temporal
			enviarMensajeSocketConLongitud(socket, COMENZAR_REDUCCION_GLOBAL, NULL, 0);

		}

	}

	aparear(elementos_de_RG);

	free(element_with_socket_to_add);

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


int leerYEnviarArchivoTemp(char ruta_arch_temp[LENGTH_RUTA_ARCHIVO_TEMP], int socket){

	//Todo: verificacion usando el socket para ver si es el encargado u otro worker

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

	//	//Inicializacion del semaforo
	//
	//	/*pshared = 0 significa que el semaforo no se comparte entre proceso
	//	**pshared = 1 que si se comparte*/
	//	int pshared;
	//	int value;
	//
	//	//solo este proceso utiliza el semaforo => 0 de privado
	//	pshared = 0;
	//	//inicializo en 1 para que pueda leer un registro y enviar antes de bloquearse
	//	value = 1;
	//	retorno = sem_init(&sem, pshared, value);
	//	if(retorno != 0){
	//		log_error(worker_error_log, "No se pudo inicializar el semaforo");
	//		return -5;
	//	}

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

		//wait(sem);

		//leo de a un registro (una linea porque ya viene ordenado el archivo) para guardar en buffer y enviar
		fgets(buffer, longitud_archivo_temporal, f1);

		if(socket!= VALOR_SOCKET_WE){

			log_trace(worker_log, "Se envia al worker encargado un registro para la reduccion global");
			solicitud_recibir_palabra* respuesta;
			respuesta->fin_de_archivo = false;
			respuesta->palabra = buffer;
			char* serialized = serializarSolicitudRecibirPalabra(respuesta);
			enviarMensajeSocket(socket, ACCION_RECIBIR_PALABRA, serialized);

		}

		recibirSolicitudWorker(socket);


	}

	solicitud_recibir_palabra* respuesta_fin;
	respuesta_fin->fin_de_archivo = true;
	respuesta_fin->palabra = "";
	char* serialized_fin = serializarSolicitudRecibirPalabra(respuesta_fin);
	enviarMensajeSocket(socket, ARCHIVO_TERMINADO, serialized_fin);

	free(buffer);

	log_destroy(worker_log);
	log_destroy(worker_error_log);

	return 0;

}


void prepararEstructuras(t_list* elementos_para_RG, t_worker worker, int posicion){

	t_elemento* unElemento;

	unElemento->ultima_palabra = "";
	unElemento->worker = worker;
	unElemento->pedir = false;
	unElemento->fin = false;
	unElemento->posicion = posicion;

	list_add(elementos_para_RG, unElemento);


}

solicitud_recibir_palabra* recibirPalabra(int socket){

	solicitud_recibir_palabra* palabra;

	if(socket != VALOR_SOCKET_WE){

		enviarMensajeSocketConLongitud(socket, CONTINUAR_ENVIO, NULL, 0);
		palabra = recibirSolicitudWorker(socket);

	}else{

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
