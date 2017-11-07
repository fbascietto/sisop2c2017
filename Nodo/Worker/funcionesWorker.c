/*
 * funcionesWorker.c
 *
 *  Created on: 4/10/2017
 *      Author: utnso
 */

#include "funcionesWorker.h"
#include "etapas.h"

void iniciarWorker(){

	//--------------WORKER LEE ARCHIVO DE CONFIGURACION--------------------


	infoConfig = config_create("../config.txt");


	if(config_has_property(infoConfig,"NOMBRE_NODO")){
		nombreNodo = config_get_string_value(infoConfig,"NOMBRE_NODO");
		nombreNodo[strlen(nombreNodo)+1]='\0';
	}

	if(config_has_property(infoConfig,"RUTA_DATABIN")){
		rutaNodo = config_get_string_value(infoConfig,"RUTA_DATABIN");
		rutaNodo[strlen(rutaNodo)+1]='\0';
	}

	if(config_has_property(infoConfig, "PUERTO_WORKER")){
		puerto = config_get_int_value(infoConfig, "PUERTO_WORKER");
	}

	if(config_has_property(infoConfig,"IP_FILESYSTEM")){
		IP_fs = config_get_string_value(infoConfig,"IP_FILESYSTEM");
		printf("IP: %s\n", IP_fs);
	}

	if(config_has_property(infoConfig,"PUERTO_FILESYSTEM")){
		puerto_fs = config_get_int_value(infoConfig,"PUERTO_FILESYSTEM");
		printf("Puerto: %d\n", puerto_fs);
	}

	//---------------ESPERA CONEXIONES-------------------------------


}

int persistirPrograma(char* nombre, char* contenido){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	char* mensaje_de_log = malloc(100);

	int retorno;
	FILE* f1;
	//longitud del contenido del programa
	int longitud_contenido = strlen(contenido);
	//inicializo en -1 para que no pueda ser igual a la longitud excepto que complete correctamente el fwrite
	int escritos = -1;

	//creo ruta para crear el archivo
	char* ruta = malloc(strlen(nombre) + LENGTH_EXTRA_SPRINTF);
	sprintf(ruta, "scripts/%s", nombre);

	f1 = fopen(ruta, "r");
	if(f1 == NULL){
		//si no esta persistido el programa aun, lo persiste

		//creo el programa vacio en el servidor local con la ruta
		f1 = fopen(ruta, "w");
		if(f1==NULL){
			sprintf(mensaje_de_log, "No se pudo persistir %s", nombre);
			log_error(worker_error_log, mensaje_de_log);
			free(ruta);
			free(mensaje_de_log);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -1;
		}

		//le escribo el contenido con lo recibido por socket
		escritos = fwrite(contenido, 1, longitud_contenido, f1);
		if(escritos != longitud_contenido){
			sprintf(mensaje_de_log, "No se pudo escribir el contenido de %s", nombre);
			log_error(worker_error_log, mensaje_de_log);
			free(ruta);
			free(mensaje_de_log);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -2;
		}

		sprintf(mensaje_de_log, "Programa %s persistido", nombre);
		log_trace(worker_log, mensaje_de_log);
		fclose(f1);

		//puntero que va a tener la cadena de caracteres que se la pasa a la funcion system para dar permisos de ejecucion al script
		char* p = malloc(strlen(ruta) + LENGTH_EXTRA_SPRINTF);

		//le doy permisos de ejecucion al script
		sprintf(p, "chmod 777 \"%s\"", ruta);
		free(ruta);
		retorno = system(p);
		if(retorno == -1){
			sprintf(mensaje_de_log, "No se pudo dar los permisos de ejecucion a %s", nombre);
			log_error(worker_error_log, mensaje_de_log);
			free(mensaje_de_log);
			free(ruta);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -10;
		}

		if(retorno != -1){
			sprintf(mensaje_de_log, "Permisos de ejecucion dados a %s", nombre);
			log_trace(worker_log, mensaje_de_log);
		}

		free(p);
		free(mensaje_de_log);
		log_destroy(worker_log);
		log_destroy(worker_error_log);

		return 0;
	}
	//si ya esta persistido, no hace nada
	fclose(f1);
	free(ruta);
	free(mensaje_de_log);
	log_destroy(worker_log);
	log_destroy(worker_error_log);
	return 0;

}

void *esperarConexionesMasterYWorker(void *args) {

	t_esperar_conexion* argumentos = (t_esperar_conexion*) args;

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);

	log_trace(worker_log, "Esperando conexiones de Master");

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------


	while(1){
		int nuevoSocket = -1;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			int cliente;
			recibirInt(nuevoSocket,&cliente);
			switch(cliente){
			case PROCESO_MASTER:
				log_trace(worker_log, "Nueva conexion de Master recibida!");
				recibirSolicitudMaster(nuevoSocket);
				break;
			case PROCESO_WORKER:
				log_trace(worker_log, "Nueva conexion de un Worker recibida!");
				recibirSolicitudWorker(nuevoSocket);
				break;
			}
		}
	}
}

void recibirSolicitudMaster(int nuevoSocket){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);

	int exit_code;
	//pid para usarlo luego en el fork
	pid_t pid;
	//	if(pid == 0){
	//		//proceso hijo continua con la solicitud
	switch(package->msgCode){

	case ACCION_TRANSFORMACION:
		; //empty statement. Es solucion a un error que genera el lenguaje C
		log_trace(worker_log, "Solicitud de transformacion recibida");
		pid = fork();
		if(pid == 0){
			log_trace(worker_log, "Comienzo de transformacion");
			solicitud_programa_transformacion* solicitudTDeserializada =
					deserializarSolicitudProgramaTransformacion(package->message);
			exit_code = transformacion(solicitudTDeserializada, rutaNodo);
			responderSolicitudT(nuevoSocket, exit_code);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			exit(0);
		}else{
			if(pid < 0){
				//cuando no pudo crear el hijo
				log_error(worker_error_log, "No se ha podido crear el proceso hijo");
			}
		}
		break;

	case ACCION_REDUCCION_LOCAL:
		; //empty statement. Es solucion a un error que genera el lenguaje C
		log_trace(worker_log, "Solicitud de reduccion local recibida");
		pid = fork();
		if(pid == 0){
			log_trace(worker_log, "Comienzo de reduccion local");
			solicitud_programa_reduccion_local* solicitudRLDeserializada =
					deserializarSolicitudProgramaReduccionLocal(package->message);
			exit_code = reduccionLocal(solicitudRLDeserializada);
			responderSolicitudRL(nuevoSocket, exit_code);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			exit(0);
		}else{
			if(pid < 0){
				//cuando no pudo crear el hijo
				log_error(worker_error_log, "No se ha podido crear el proceso hijo");
			}
		}
		break;

	case ACCION_REDUCCION_GLOBAL:
		; //empty statement. Es solucion a un error que genera el lenguaje C
		log_trace(worker_log, "Solicitud de reduccion global recibida");
		log_trace(worker_log, "Comienzo de reduccion global");
		solicitud_programa_reduccion_global* solicitudRGDeserializada =
				deserializarSolicitudProgramaReduccionGlobal(package->message);
		ruta_archivo_temp_final = solicitudRGDeserializada->archivo_temporal_resultante;
		exit_code = reduccionGlobal(solicitudRGDeserializada, puerto);
		responderSolicitudRG(nuevoSocket, exit_code);
		break;

	case ACCION_ALMACENAMIENTO_FINAL:
		; //empty statement. Es solucion a un error que genera el lenguaje C
		log_trace(worker_log, "Solicitud de almacenamiento final recibida");
		almacenamientoFinal(IP_fs, puerto_fs, ruta_archivo_temp_final);
		break;

	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);

}

void recibirSolicitudWorker(int nuevoSocket){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);
	int exit_code;
	switch(package->msgCode){
	case ACCION_ENVIAR_ARCHIVO_TEMP_DE_RL:
		; //empty statement. Es solucion a un error que genera el lenguaje C
		solicitud_enviar_archivo_temp* solicitudEATDeserializada =
				deserializarSolicitudEnviarArchivoTemp(package->message);
		enviarArchivoTemp(solicitudEATDeserializada);
		break;
	case ACCION_LEER_ARCHIVO_TEMP_DE_RL:
		; //empty statement. Es solucion a un error que genera el lenguaje C
		solicitud_leer_archivo_temp* solicitudLATDeserializada =
				deserializarSolicitudLeerArchivoTemp(package->message);
		leerArchivoTemp(solicitudLATDeserializada);
		break;

	log_destroy(worker_log);
	log_destroy(worker_error_log);

	}
}
