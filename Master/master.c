/*
 * master.c
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include "funcionesmaster.h"
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "interfaceMaster.h"


void main(int args, char* argv[]) {
	//testSerializarSolicitudTrasnformacion();
	//testSerializarItemTransformacion();
	//testSerializarSolicitudReduccionLocal();
	//testSerializarSolicitudReduccionGlobal();
	//testSerializarSolicitudAlmacenadoFinal();
	//testSerializarSolicitudReduccionGlobal();


	t_log_level level_INFO = LOG_LEVEL_INFO;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* worker_info_log = log_create("logMaster.txt", "MASTER", 1, level_INFO);
	t_log* worker_error_log = log_create("logMaster.txt", "MASTER", 1, level_ERROR);

	if(args != 5){

		log_error(worker_error_log, "Cantidad de parametros incorrectos.");
		exit(0);

	}

	ruta_programa_transformador = argv[1];
	ruta_programa_reductor = argv[2];
	ruta_archivo_del_job = argv[3];
	ruta_archivo_final_fs = argv[4];

	char* info = string_from_format("El programa transformador que se utilizara para el job es: %s", ruta_programa_transformador);
	log_info(worker_info_log, info);
	free(info);
	info = string_from_format("El programa reductor que se utilizara para el job es: %s", ruta_programa_reductor);
	log_info(worker_info_log, info);
	free(info);
	info = string_from_format("La ruta del archivo que se procesara en el job es: %s", ruta_archivo_del_job);
	log_info(worker_info_log, info);
	free(info);
	info = string_from_format("La ruta del archivo resultante del job es: %s", ruta_archivo_final_fs);
	log_info(worker_info_log, info);
	free(info);


	t_config* infoConfig;
	char* yamaIP;
	char* nombreNodo;
	int yamaPort;
	int socketConn;


	infoConfig = config_create("config.txt");

	if(config_has_property(infoConfig,"YAMA_IP")){
		yamaIP = config_get_string_value(infoConfig,"YAMA_IP");
		printf("IP: %s\n", yamaIP);
	}

	if(config_has_property(infoConfig,"YAMA_PUERTO")){
		yamaPort = config_get_int_value(infoConfig,"YAMA_PUERTO");
		printf("Puerto: %d\n", yamaPort);
	}
	if(config_has_property(infoConfig,"NOMBRE_MASTER")){
		nombreNodo = config_get_string_value(infoConfig,"NOMBRE_MASTER");
		// printf("Nombre: %s\n", nombreNodo);
		nombreNodo[strlen(nombreNodo)+1]='\0';
	}

	//TODO: crear un hilo para manejar esta comunicacion con Yama.
	socketConn = conectarseA(yamaIP, yamaPort);
	while(socketConn == 0){
		socketConn = conectarseA(yamaIP, yamaPort);
		sleep(3);
	}

	enviarInt(socketConn,PROCESO_MASTER);
	int len = strlen(ruta_archivo_del_job);
	uint32_t message_long = sizeof(char)*len;
	enviarMensajeSocketConLongitud(socketConn,ACCION_PROCESAR_ARCHIVO,ruta_archivo_del_job,len);
	while(1){
		sleep(1);
		Package* package = createPackage();
		printf("esperando mensaje de yama: %d\n",socketConn);
		int leidos = recieve_and_deserialize(package, socketConn);
		printf("codigo de mensaje: %d\n",	package->msgCode);
		switch(package->msgCode){
			case ACCION_PROCESAR_TRANSFORMACION:
				procesarSolicitudTransformacion(socketConn, package->message_long, package->message);
				//enviarMensajeSocketConLongitud(socketConn,RESULTADO_TRANSFORMACION,archivoMensage,len);
				break;
			case ACCION_PROCESAR_REDUCCION_LOCAL:
				procesarSolicitudReduccionLocal(socketConn, package->message_long, package->message);
				//enviarMensajeSocketConLongitud(socketConn,RESULTADO_REDUCCION_LOCAL,archivoMensage,len);
				break;
			case ACCION_PROCESAR_REDUCCION_GLOBAL:
				procesarSolicitudReduccionGlobal(socketConn, package->message_long, package->message);
				//enviarMensajeSocketConLongitud(socketConn,RESULTADO_REDUCCION_GLOBAL,archivoMensage,len);
				break;
			case ACCION_PROCESAR_ALMACENADO_FINAL:
				procesarSolicitudAlmacenadoFinal(socketConn, package->message_long, package->message);
				//enviarMensajeSocketConLongitud(socketConn,RESULTADO_ALMACENADO_FINAL,archivoMensage,len);
				break;
		}

	}

	log_destroy(worker_info_log);
	log_destroy(worker_error_log);

}

