/*
 * funcionesWorker.c
 *
 *  Created on: 4/10/2017
 *      Author: utnso
 */

#include "funcionesWorker.h"
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"
#include <commons/log.h>
#include <commons/config.h>
#include <sys/types.h>
#include "interfaceWorker.h"



void iniciarWorker(){

//--------------WORKER LEE ARCHIVO DE CONFIGURACION--------------------

	//numeroDeArchivoTemporal = 0;   //nombrado de archivos lo hace YAMA

	infoConfig = config_create("../config.txt");


		if(config_has_property(infoConfig,"NOMBRE_NODO")){
			nombreNodo = config_get_string_value(infoConfig,"NOMBRE_NODO");
			nombreNodo[strlen(nombreNodo)+1]='\0';
		}

		if(config_has_property(infoConfig,"NOMBRE_NODO")){
			rutaNodo = config_get_string_value(infoConfig,"RUTA_DATABIN");
			rutaNodo[strlen(rutaNodo)+1]='\0';
		}

		if(config_has_property(infoConfig, "PUERTO_WORKER")){
			puerto = config_get_int_value(infoConfig, "PUERTO_WORKER");
		}

//---------------ESPERA CONEXIONES-------------------------------



		}

void responderSolicitudT(int exit_code){

	if(exit_code == 0){
		//enviar OK
	}else{
		if(exit_code == -1){
			//enviar ERROR
		}
	}
}

void responderSolicitudRL(int exit_code){

	if(exit_code == 0){
		//enviar OK
	}else{
		if(exit_code == -1){
			//enviar ERROR
		}
	}
}

void responderSolicitudRG(int exit_code){

	if(exit_code == 0){
		//enviar OK
	}else{
		if(exit_code == -1){
			//enviar ERROR
		}
	}
}

int transformacion(solicitud_programa_transformacion* solicitudDeserializada){

	FILE *f1;
	char* buffer = malloc(solicitudDeserializada->bytes_ocupados);
	int leidos;
	char* s;

	//abro el data.bin
	f1 = fopen ("../data.bin", "rb");
	if (f1==NULL)
	{
	   perror("No se pudo abrir data.bin");
	   return -1;
	}

	//me desplazo hasta el bloque que quiero leer
	fseek(f1, 1048576*solicitudDeserializada->bloque, SEEK_SET);

	//leer bloque de archivo
	leidos = fread(buffer, 1, solicitudDeserializada->bytes_ocupados, f1);
	if(leidos!= solicitudDeserializada->bytes_ocupados){
		perror("No se leyo correctamente el bloque");
		return -2;
	}

	fclose(f1);

	//meto en el system lo que quiero que ejecute el script
	//esto funciona asumiendo que el script esta en la maquina del worker, falta pasarlo de archivo a ruta
	sprintf(s, "echo %s | .%s | sort > %s", buffer, solicitudDeserializada->programa_transformacion, solicitudDeserializada->archivo_temporal);
	system(s);
	free(buffer);

	return 0;
}

int reduccionLocal(solicitud_programa_reduccion_local* solicitudDeserializada){

	int i;

	for(i=0; i<solicitudDeserializada->cantidad_archivos_temp; i++){



	}

	return 0;
}

int reduccionGlobal(solicitud_programa_reduccion_global* solicitudDeserializada){

	int i;

	for(i=0; i<solicitudDeserializada->cantidad_item_programa_reduccion; i++){



	}

	return 0;
}

void enviarArchivoTemp(solicitud_enviar_archivo_temp* solicitudDeserializada){

}

void leerArchivoTemp(solicitud_leer_archivo_temp* solicitudDeserializada){

}

void *esperarConexionesMaster(void *args) {

	t_esperar_conexion* argumentos = (t_esperar_conexion*) args;

	printf("Esperando conexiones en Worker...\n");

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------


		while(1){
			int nuevoSocket = -1;

			nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,argumentos->socketEscucha);

			if (nuevoSocket != -1) {
				//log_trace(logSockets,"Nuevo Socket!");
				printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);
				int cliente;
				recibirInt(nuevoSocket,&cliente);
				switch(cliente){
					case PROCESO_MASTER:
						recibirSolicitudMaster(nuevoSocket);
						break;
					case PROCESO_WORKER:
						recibirSolicitudWorker(nuevoSocket);
						break;
				}
			}
		}
}

void recibirSolicitudMaster(int nuevoSocket){
	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);
	printf("codigo de mensaje: %d\n",	package->msgCode);
	int exit_code;
	pid_t pid = fork();
	if(pid == 0){
		//proceso hijo continua con la solicitud
		switch(package->msgCode){
			case ACCION_TRANSFORMACION:
				; //empty statement. Es solucion a un error que genera el lenguaje C
				solicitud_programa_transformacion* solicitudTDeserializada =
							deserializarSolicitudProgramaTransformacion(package->message);
				exit_code = transformacion(solicitudTDeserializada);
				responderSolicitudT(exit_code);
				break;
			case ACCION_REDUCCION_LOCAL:
				; //empty statement. Es solucion a un error que genera el lenguaje C
				solicitud_programa_reduccion_local* solicitudRLDeserializada =
							deserializarSolicitudProgramaReduccionLocal(package->message);
				exit_code = reduccionLocal(solicitudRLDeserializada);
				responderSolicitudRL(exit_code);
				break;
			case ACCION_REDUCCION_GLOBAL:
				; //empty statement. Es solucion a un error que genera el lenguaje C
				solicitud_programa_reduccion_global* solicitudRGDeserializada =
							deserializarSolicitudProgramaReduccionGlobal(package->message);
				exit_code = reduccionGlobal(solicitudRGDeserializada);
				responderSolicitudRG(exit_code);
				break;
		}
		exit(0);
	}else{
		if(pid < 0){
			//cuando no pudo crear el hijo
			perror("No se ha podido crear el proceso hijo\n");
		}else{
			//lo que haria el padre si es que necesitamos que haga algo
		}

	}
}

void recibirSolicitudWorker(int nuevoSocket){
	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);
	printf("codigo de mensaje: %d\n", package->msgCode);
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


	}
}
