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

#define LENGTH_EXTRA_SPRINTF 100

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

//---------------ESPERA CONEXIONES-------------------------------


}

int persistirPrograma(char* nombre, char* contenido, char* etapa){

	FILE* f1;
	FILE* f2;
	//longitud del contenido del programa
	int longitud_contenido = strlen(contenido);
	//inicializo en -1 para que no pueda ser igual a la longitud excepto que complete correctamente el fwrite
	int escritos = -1;

	//creo ruta para crear el archivo TODO: SE DEBE CREAR /scripts PREVIAMENTE AL LEVANTAR LA VM "mkdir scripts"
	char* ruta;
	sprintf(ruta, "/scripts/%s", nombre);

	f1 = fopen(ruta, "r");
	if(f1 == NULL){
		//si no esta persistido el programa aun, lo persiste
		fclose(f1);
		char* mensaje_de_error = malloc(100);

		//creo el programa vacio en el servidor local con la ruta
		f2 = fopen(ruta, "w");
		free(ruta);
		if(f2==NULL){
			sprintf(mensaje_de_error, "No se pudo crear el programa de %s\n", etapa);
			perror(mensaje_de_error);
			free(mensaje_de_error);
			return -1;
			}

		//le escribo el contenido con lo recibido por socket
		escritos = fwrite(contenido, 1, longitud_contenido, f2);
		if(escritos != longitud_contenido){
			sprintf(mensaje_de_error, "No se pudo escribir el contenido del programa de %s\n", etapa);
			perror(mensaje_de_error);
			free(mensaje_de_error);
			return -2;
		}

		fclose(f2);
		free(mensaje_de_error);

		return 0;
	}
		//si ya esta persistido, no hace nada
		fclose(f1);
		printf("Ya estaba persistido\n");
		return 0;

}

void responderSolicitudT(int exit_code){

	switch(exit_code){

		case 0:
			//enviar OK
			break;
		case -1:
			//enviar ERROR de creacion de programa de transformacion
			break;
		case -2:
			//enviar ERROR de escritura de programa de transformacion
			break;
		case -3:
			//enviar ERROR de apertura de data.bin
			break;
		case -4:
			//enviar ERROR de lectura de data.bin
			break;

	}
}

void responderSolicitudRL(int exit_code){

	switch(exit_code){

		case 0:
			//enviar OK
			break;
		case -1:
			//enviar ERROR de creacion de programa de reduccion
			break;
		case -2:
			//enviar ERROR de escritura de programa de reduccion
			break;
		case -3:
			//enviar ERROR de apertura de archivo temporal
			break;
		case -4:
			//enviar ERROR de posicionamiento al final del archivo temporal
			break;
		case -5:
			//enviar ERROR de posicionamiento al principio del archivo temporal
			break;
		case -6:
			//enviar ERROR de lectura de archivo temporal
			break;
	}

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

	//fichero para leer el data.bin
	FILE* f1;
	int leidos;

	//retorno de la funcion que persiste el programa de transformacion
	int retorno;
	//etapa para pasarle a la funcion
	char* etapa = "transformacion";

	//persisto el programa transformador
	retorno = persistirPrograma(solicitudDeserializada->programa_transformacion, solicitudDeserializada->programa, etapa);
	if(retorno == -1 || retorno == -2){
		return retorno;
	}

	//abro el data.bin
	f1 = fopen (rutaNodo, "rb");
	if (f1==NULL)
	{
	   perror("No se pudo abrir data.bin\n");
	   return -3;
	}

	//me desplazo hasta el bloque que quiero leer
	fseek(f1, TAMANIO_BLOQUE*solicitudDeserializada->bloque, SEEK_SET);

	//buffer donde pongo datos que leo del bloque del data.bin
	char* buffer = malloc(solicitudDeserializada->bytes_ocupados + 1);

	//leer bloque de archivo
	leidos = fread(buffer, 1, solicitudDeserializada->bytes_ocupados, f1);
	if(leidos!= solicitudDeserializada->bytes_ocupados){
		perror("No se leyo correctamente el bloque");
		free(buffer);
		return -4;
	}

	fclose(f1);

	//puntero que va a tener la cadena de caracteres que se la pasa a la funcion system para dar permisos de ejecucion al script
	char* p = malloc(LENGTH_NOMBRE_PROGRAMA +LENGTH_EXTRA_SPRINTF);

	//le doy permisos de ejecucion al script
	sprintf(p, "chmod 777 \"/scripts/%s\"", solicitudDeserializada->programa_transformacion);
	system(p);
	free(p);

	//puntero que va a tener la cadena de caracteres que se le pasa a la funcion system para ejecutar el script
	char* s = malloc(solicitudDeserializada->bytes_ocupados + LENGTH_NOMBRE_PROGRAMA + LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_EXTRA_SPRINTF + 1);

	//meto en s lo que quiero pasarle a system para que ejecute el script
	sprintf(s, "echo %s | .\"/scripts/%s\" | sort > \"%s\"", buffer, solicitudDeserializada->programa_transformacion, solicitudDeserializada->archivo_temporal);
	system(s);
	free(buffer);
	free(s);

	return 0;

}


int reduccionLocal(solicitud_programa_reduccion_local* solicitudDeserializada){

	//entero para iterar array
	int i;
	//entero para depositar longitud de archivos temporales y leerlos completos
	int longitud_archivo_temporal;
	//para guardar el contenido de 1 archivo
	char* buffer;
	//variable que uso para trackear cuanta memoria asignada tiene previamente el buffer_total
	int memoria_asignada = sizeof(char);
	FILE* f1;
	int leidos;

	//retorno de la funcion que persiste el programa de reduccion
	int retorno;
	//etapa para pasarle a la funcion
	char* etapa = "reduccion_local";

	//persisto el programa reductor
	retorno = persistirPrograma(solicitudDeserializada->programa_reduccion, solicitudDeserializada->programa, etapa);
	if(retorno == -1 || retorno == -2){
		return retorno;
		}

	//para guardar el contenido de todos los archivos
	char* buffer_total = malloc(memoria_asignada);

	for(i=0; i<solicitudDeserializada->cantidad_archivos_temp; i++){

		//abro el archivo temporal de transformacion
		f1 = fopen(solicitudDeserializada->archivos_temporales[i].archivo_temp, "r");
		if (f1==NULL){
		   perror("No se pudo abrir el archivo temporal\n");
		   free(buffer_total);
		   return -3;
		}


		//me posiciono al final del archivo
		retorno = fseek(f1, 0, SEEK_END);
		if(retorno!=0){
			perror("No se pudo posicionar al final del archivo temporal\n");
			free(buffer_total);
			return -4;
		}
		//determino longitud del archivo
		longitud_archivo_temporal = ftell(f1);
		//me posiciono al principio del archivo para poder leer
		retorno = fseek(f1, 0, SEEK_SET);
		if(retorno!=0){
			perror("No se pudo posicionar al principio del archivo temporal\n");
			free(buffer_total);
			return -5;
		}

		buffer = malloc(longitud_archivo_temporal + 1);
		//leo archivo y lo pongo en el buffer
		leidos = fread(buffer, 1, longitud_archivo_temporal, f1);
		if(leidos != longitud_archivo_temporal){
			perror("No se leyo correctamente el archivo temporal\n");
			free(buffer);
			free(buffer_total);
			return -6;
		}
		//modifico valor de memoria asignada para que tambien tenga la longitud del archivo
		memoria_asignada = memoria_asignada + longitud_archivo_temporal;
		buffer_total = realloc(buffer, memoria_asignada);
		strcat(buffer_total, buffer);

		fclose(f1);
	}

	//puntero que va a tener la cadena de caracteres que se la pasa a la funcion system para dar permisos de ejecucion al script
	char* p = malloc(solicitudDeserializada->programa_reduccion + LENGTH_EXTRA_SPRINTF);

	//le doy permisos de ejecucion al script
	sprintf(p, "chmod 777 \"/scripts/%s\"", solicitudDeserializada->programa_reduccion);
	system(p);
	free(p);

	//puntero que va a tener la cadena de caracteres que se le pasa a la funcion system para ejecutar el script
	char* s = malloc(strlen(buffer_total) + LENGTH_NOMBRE_PROGRAMA + LENGTH_RUTA_ARCHIVO_TEMP + LENGTH_EXTRA_SPRINTF + 1);

	sprintf(s, "echo %s | sort | .\"/scripts/%s\" > \"%s\"", buffer_total, solicitudDeserializada->programa_reduccion, solicitudDeserializada->archivo_temporal_resultante);
	system(s);

	free(s);
	free(buffer);
	free(buffer_total);

	return 0;
}

int reduccionGlobal(solicitud_programa_reduccion_global* solicitudDeserializada){

	int i;

	//retorno de la funcion que persiste el programa de reduccion
	int retorno;
	//etapa para pasarle a la funcion
	char* etapa = "reduccion_global";

	//persisto el programa reductor
	retorno = persistirPrograma(solicitudDeserializada->programa_reduccion, solicitudDeserializada->programa, etapa);
	if(retorno == -1 || retorno == -2){
		return retorno;
	}


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
