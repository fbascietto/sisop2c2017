/*
 * funcionesWorker.c
 *
 *  Created on: 4/10/2017
 *      Author: utnso
 */

#include "funcionesWorker.h"


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
	}

	if(config_has_property(infoConfig,"PUERTO_FILESYSTEM")){
		puerto_fs = config_get_int_value(infoConfig,"PUERTO_FILESYSTEM");

	}


}

int persistirPrograma(char* nombre, char* contenido){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	char* mensaje_de_error_log = string_new();

	int retorno;
	FILE* f1;
	//longitud del contenido del programa
	int longitud_contenido = strlen(contenido);
	//inicializo en -1 para que no pueda ser igual a la longitud excepto que complete correctamente el fwrite
	int escritos = -1;

	//creo ruta para crear el archivo
	char* ruta = string_from_format("scripts/%s", nombre);

	f1 = fopen(ruta, "r");
	if(f1 == NULL){
		//si no esta persistido el programa aun, lo persiste

		//creo el programa vacio en el servidor local con la ruta
		f1 = fopen(ruta, "w");
		if(f1==NULL){
			string_append_with_format(&mensaje_de_error_log, "No se pudo persistir %s", nombre);
			log_error(worker_error_log, mensaje_de_error_log);
			free(ruta);
			free(mensaje_de_error_log);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -1;
		}

		//le escribo el contenido con lo recibido por socket
		escritos = fwrite(contenido, 1, longitud_contenido, f1);
		if(escritos != longitud_contenido){
			string_append_with_format(&mensaje_de_error_log, "No se pudo escribir el contenido de %s", nombre);
			log_error(worker_error_log, mensaje_de_error_log);
			free(ruta);
			free(mensaje_de_error_log);
			fclose(f1);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -2;
		}

		char* mensaje_de_log1 = string_from_format("Programa %s persistido", nombre);
		log_trace(worker_log, mensaje_de_log1);
		free(mensaje_de_log1);
		fclose(f1);

		//puntero que va a tener la cadena de caracteres que se la pasa a la funcion system para dar permisos de ejecucion al script
		char* p = string_from_format("chmod 777 \"%s\"", ruta);
		free(ruta);
		//le doy permisos de ejecucion al script
		retorno = system(p);
		if(retorno == -1){
			sprintf(mensaje_de_error_log, "No se pudo dar los permisos de ejecucion a %s", nombre);
			log_error(worker_error_log, mensaje_de_error_log);
			free(mensaje_de_error_log);
			free(p);
			log_destroy(worker_log);
			log_destroy(worker_error_log);
			return -10;
		}

		if(retorno != -1){
			char* mensaje_de_log2 = string_from_format("Permisos de ejecucion dados a %s", nombre);
			log_trace(worker_log, mensaje_de_log2);
			free(mensaje_de_log2);
		}

		free(p);
		free(mensaje_de_error_log);
		log_destroy(worker_log);
		log_destroy(worker_error_log);

		return 0;
	}
	//si ya esta persistido, no hace nada
	fclose(f1);
	free(ruta);
	free(mensaje_de_error_log);
	log_destroy(worker_log);
	log_destroy(worker_error_log);
	return 0;

}

void *esperarConexionesMasterYWorker(void *args) {

	t_esperar_conexion* argumentos = (t_esperar_conexion*) args;

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	log_trace(worker_log, "Esperando conexiones de Master");

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------

	int cliente;
	pid_t pid;
	int nuevoSocket;

	while(1){

		nuevoSocket = -1;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,argumentos->socketEscucha);

		if (nuevoSocket != -1) {

//			Package* package = createPackage();
//			recieve_and_deserialize(package, nuevoSocket);
//			solicitud_programa_transformacion* solicitud = deserializarSolicitudProgramaTransformacion(package->message);
//			printf("%s", solicitud->archivo_temporal);

			pid = fork();
			if(pid == 0){
				//proceso hijo continua la solicitud
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

			}else{

				if(pid < 0){
					//cuando no pudo crear el hijo
					log_error(worker_error_log, "No se ha podido crear el proceso hijo para responder a la solicitud");

				}

			}
		}
	}
}

void recibirSolicitudMaster(int nuevoSocket){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);

	solicitud_programa_transformacion* solicitudTDeserializada;
	solicitud_programa_reduccion_local* solicitudRLDeserializada;
	solicitud_programa_reduccion_global* solicitudRGDeserializada;
	solicitud_realizar_almacenamiento_final* solicitudAFDeserializada;


	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);

	int exit_code;

	switch(package->msgCode){

	case ACCION_TRANSFORMACION:
		log_trace(worker_log, "Solicitud de transformacion recibida");
		log_trace(worker_log, "Comienzo de transformacion");
		solicitudTDeserializada = deserializarSolicitudProgramaTransformacion(package->message);
		char* contenido_programa = malloc(solicitudTDeserializada->length_programa + 1);
		strcpy(contenido_programa, solicitudTDeserializada->programa);
		solicitud_programa_transformacion solicitudT;
		strcpy(solicitudT.archivo_temporal, solicitudTDeserializada->archivo_temporal);
		strcpy(solicitudT.programa_transformacion, solicitudTDeserializada->programa_transformacion);
		solicitudT.programa = contenido_programa;
		solicitudT.bloque = solicitudTDeserializada->bloque;
		solicitudT.bytes_ocupados = solicitudTDeserializada->bytes_ocupados;
		solicitudT.length_programa = solicitudTDeserializada->length_programa;
//		t_log_level slevel = LOG_LEVEL_INFO;
//		t_log* struct_log = log_create("structlog.txt", "WORKER", 0, slevel);
//		char* log = string_from_format("\nruta temporal: %s\nnombre script: %s\ncontenido programa: %s\nnro bloque: %d\nbytes ocupados: %d\nlongitud programa: %d", solicitudT.archivo_temporal, solicitudT.programa_transformacion, solicitudT.programa, solicitudT.bloque, solicitudT.bytes_ocupados, solicitudT.length_programa);
//		log_info(struct_log, log);
		exit_code = transformacion(solicitudT, rutaNodo);
		responderSolicitudT(nuevoSocket, exit_code);
		log_destroy(worker_log);
		free(solicitudTDeserializada);
		free(contenido_programa);
		exit(0);
		break;

	case ACCION_REDUCCION_LOCAL:
		log_trace(worker_log, "Solicitud de reduccion local recibida");
		log_trace(worker_log, "Comienzo de reduccion local");
		solicitudRLDeserializada = deserializarSolicitudProgramaReduccionLocal(package->message);
		exit_code = reduccionLocal(solicitudRLDeserializada);
		responderSolicitudRL(nuevoSocket, exit_code);
		log_destroy(worker_log);
		free(solicitudRLDeserializada);
		exit(0);
		break;

	case ACCION_REDUCCION_GLOBAL:
		log_trace(worker_log, "Solicitud de reduccion global recibida");
		log_trace(worker_log, "Comienzo de reduccion global");
		solicitudRGDeserializada = deserializarSolicitudProgramaReduccionGlobal(package->message);
		strcpy(ruta_archivo_temp_final, solicitudRGDeserializada->archivo_temporal_resultante);
		exit_code = reduccionGlobal(solicitudRGDeserializada);
		responderSolicitudRG(nuevoSocket, exit_code);
		log_destroy(worker_log);
		free(solicitudRGDeserializada);
		exit(0);
		break;

	case ACCION_ALMACENAMIENTO_FINAL:
		log_trace(worker_log, "Solicitud de almacenamiento final recibida");
		solicitudAFDeserializada = deserializarSolicitudRealizarAlmacenadoFinal(package->message);
		almacenamientoFinal(IP_fs, puerto_fs, solicitudAFDeserializada);
		responderSolicitudAlmacenadoFinal(nuevoSocket);
		log_destroy(worker_log);
		free(solicitudAFDeserializada);
		exit(0);
		break;

	}

	log_destroy(worker_log);

}

solicitud_recibir_palabra* recibirSolicitudWorker(int nuevoSocket){

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_log = log_create("logWorker.txt", "WORKER", 1, level);
	t_log* worker_error_log = log_create("logWorker.txt", "WORKER", 1, level_ERROR);

	solicitud_recibir_palabra* palabra;
	solicitud_leer_y_enviar_archivo_temp* solicitudLYEATDeserializada;

	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);

	int exit_code;

	switch(package->msgCode){

	case COMENZAR_REDUCCION_GLOBAL:
		solicitudLYEATDeserializada = deserializarSolicitudLeerYEnviarArchivoTemp(package->message);
		exit_code = leerYEnviarArchivoTemp(solicitudLYEATDeserializada->ruta_archivo_red_local_temp, nuevoSocket);
		break;
	case ACCION_RECIBIR_PALABRA:
		palabra = deserializarSolicitudRecibirPalabra(package->message);
		break;
	case CONTINUAR_ENVIO:
		break;

	}

	log_destroy(worker_log);
	log_destroy(worker_error_log);

	return palabra;
}
