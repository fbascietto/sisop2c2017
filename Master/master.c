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
#include <commons/string.h>
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "interfaceMaster.h"
#include "../bibliotecas/fileCleaner.c"

void main(int args, char* argv[]) {

	vaciarArchivo("logMaster.txt");

	t_log_level level_INFO = LOG_LEVEL_INFO;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* master_info_log = log_create("logMaster.txt", "MASTER", 1, level_INFO);
	t_log* master_error_log = log_create("logMaster.txt", "MASTER", 1, level_ERROR);

	if(args != 5){

		log_error(master_error_log, "Cantidad de parametros incorrectos.");
		exit(0);

	}

	ruta_programa_transformador = argv[1];
	ruta_programa_reductor = argv[2];
	ruta_archivo_del_job = argv[3];
	ruta_archivo_final_fs = argv[4];

	char* info = string_from_format("El programa transformador que se utilizara para el job es: %s", ruta_programa_transformador);
	log_info(master_info_log, info);
	free(info);
	info = string_from_format("El programa reductor que se utilizara para el job es: %s", ruta_programa_reductor);
	log_info(master_info_log, info);
	free(info);
	info = string_from_format("La ruta del archivo que se procesara en el job es: %s", ruta_archivo_del_job);
	log_info(master_info_log, info);
	free(info);
	info = string_from_format("La ruta del archivo resultante del job es: %s", ruta_archivo_final_fs);
	log_info(master_info_log, info);
	free(info);


	t_config* infoConfig;
	char* yamaIP;
	char* nombreNodo;
	int yamaPort;

	activosReduccionLocal = 0;

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
	socketYama = conectarseA(yamaIP, yamaPort);
	while(socketYama == 0){
		socketYama = conectarseA(yamaIP, yamaPort);
		sleep(3);
	}
	gettimeofday(&t_ini, NULL);
	enviarInt(socketYama,PROCESO_MASTER);
	int len = strlen(ruta_archivo_del_job);
	uint32_t message_long = sizeof(char)*len;
	enviarMensajeSocketConLongitud(socketYama,ACCION_PROCESAR_ARCHIVO,ruta_archivo_del_job,len);
	recibirInt(socketYama,&idJob);
	while(1){
		Package* package = createPackage();
		printf("esperando mensaje de yama: %d\n",socketYama);
		int leidos = recieve_and_deserialize(package, socketYama);
		printf("codigo de mensaje: %d\n",	package->msgCode);
		switch(package->msgCode){
			case ACCION_REPLANIFICACION: ;
				uint32_t idJobReplanificacion = malloc(sizeof(uint32_t));
				int offset = 0;
				deserializarDato(&(idJobReplanificacion),package->message,sizeof(uint32_t),&offset);
				idJob = idJobReplanificacion;
				break;
			case ACCION_TERMINAR_JOB:
				close(socketYama);
				break;
			case ACCION_PROCESAR_TRANSFORMACION:
				procesarSolicitudTransformacion(socketYama, package->message_long, package->message);
				//enviarMensajeSocketConLongitud(socketConn,RESULTADO_TRANSFORMACION,archivoMensage,len);
				break;
			case ACCION_PROCESAR_REDUCCION_LOCAL:
				procesarSolicitudReduccionLocal(socketYama, package->message_long, package->message);
				//enviarMensajeSocketConLongitud(socketConn,RESULTADO_REDUCCION_LOCAL,archivoMensage,len);
				break;
			case ACCION_PROCESAR_REDUCCION_GLOBAL:
				procesarSolicitudReduccionGlobal(socketYama, package->message_long, package->message);
				//enviarMensajeSocketConLongitud(socketConn,RESULTADO_REDUCCION_GLOBAL,archivoMensage,len);
				break;
			case ACCION_PROCESAR_ALMACENADO_FINAL:
				procesarSolicitudAlmacenadoFinal(socketYama, package->message_long, package->message);
				//enviarMensajeSocketConLongitud(socketConn,RESULTADO_ALMACENADO_FINAL,archivoMensage,len);
				break;
		}
		if(package->msgCode ==ACCION_TERMINAR_JOB){
			break;
		}
	}
//	gettimeofday(&t_fin, NULL);
//	metrica();
//	log_destroy(master_info_log);
//	log_destroy(master_error_log);

}

void metrica(){
	t_log_level level_INFO = LOG_LEVEL_INFO;
	t_log* metricas_info_log = log_create("master-metricas.txt", "MASTER", 1, level_INFO);

	//Tiempo total de Ejecución del Job.
	double tiempoDeEjecucionTotal = timeval_diff(&t_fin, &t_ini);

	//Tiempo promedio de ejecución de cada etapa principal del Job (Transformación, Reducción y Reducción Local).
	double tiempoPromEtapasTransformacion = tiempoAcumEtapasTransformacion/cantidadEtapasTranformacion;
	double tiempoPromEtapasReduccionLocal = tiempoAcumEtapasReduccionLocal/cantidadEtapasReduccionLocal;
	double tiempoPromEtapasReduccionGlobal = tiempoAcumEtapasReduccionLocal/cantidadEtapasReduccionLocal;

	//Cantidad máxima de tareas de Transformación y Reducción Local ejecutadas de forma paralela.
	int cantidadMayorTransformacion = cantidadMayorTransformacion;
	int cantidadMayorReduccionLocal = cantidadMayorReduccionLocal;

	//Cantidad total de tareas realizadas en cada etapa principal del Job.
	int cantidadTareasTransformacion = cantidadEtapasTranformacion;
	int cantidadTareasReduccionLocal = cantidadEtapasReduccionLocal;
	int cantidadTareasReduccionGlobal = cantidadEtapasReduccionLocal;

	//Cantidad de fallos obtenidos en la realización de un Job.
	int fallosTotal = fallosEnTotal;

	char* info = string_from_format("Tiempo total de Ejecución del Job: %.16g", tiempoDeEjecucionTotal);
	log_info(metricas_info_log, info);
	free(info);

	info = string_from_format("Tiempo promedio de ejecución Transformación: %.16g", tiempoPromEtapasTransformacion);
	log_info(metricas_info_log, info);
	free(info);

	info = string_from_format("Tiempo promedio de ejecución Reducción: %.16g", tiempoPromEtapasReduccionLocal);
	log_info(metricas_info_log, info);
	free(info);

	info = string_from_format("Tiempo promedio de ejecución Reducción Local: %s", tiempoPromEtapasReduccionGlobal);
	log_info(metricas_info_log, info);
	free(info);

	info = string_from_format("Cantidad máxima de tareas de Transformación ejecutadas de forma paralela: %s", cantidadMayorTransformacion);
	log_info(metricas_info_log, info);
	free(info);

	info = string_from_format("Cantidad máxima de tareas de Reducción Local ejecutadas de forma paralela: %s", cantidadMayorReduccionLocal);
	log_info(metricas_info_log, info);
	free(info);

	info = string_from_format("Cantidad de fallos obtenidos en la realización de un Job: %s", fallosEnTotal);
	log_info(metricas_info_log, info);
	free(info);
}

