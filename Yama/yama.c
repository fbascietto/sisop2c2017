/*
 * yama.c
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/sockets.c"
#include "funcionesyama.h"
#include "prePlanificacion.h"
#include "../bibliotecas/fileCleaner.c"

int main() {

	vaciarArchivo("logYama.txt");

	socketFS=0;
	nodosConectados=list_create();
	valorBaseTemporal=0;
	rutaGlobal=0;
	jobsActivos=list_create();
	jobsFinalizados=list_create();


	//todo
	struct sigaction sa;
	sa.sa_handler = recargarConfiguracion;
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGUSR1,&sa,0) < 0) // Setup signal
		log_trace(logYamaErrorImpreso,"sigaction failed");

	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	logYama = log_create("logYama.txt", "YAMA", 0, level);
	logYamaImpreso = log_create("logYama.txt", "YAMA", 1, level);
	logYamaError = log_create("logYama.txt", "YAMA", 0, level_ERROR);
	logYamaErrorImpreso = log_create("logYama.txt", "YAMA", 1, level_ERROR);

	//cargo config.txt
	inicializarConfigYama();

	socketFS = 0;
	int socketEscucha;
	int puerto = 5100;
	fd_set fdSocketsEscucha;
	FD_ZERO(&fdSocketsEscucha);
	socketEscucha= escuchar(puerto);
	FD_SET(socketEscucha, &fdSocketsEscucha);

	log_trace(logYamaImpreso, "pid de yama %d \nusarlo para enviar seniales", getpid());
	log_trace(logYamaImpreso, "conectando con el filesystem");

	int contadorfs = 1;
	socketFS = conectarseA(fsIP, fsPort);
	while(socketFS == 0){
		sleep(contadorfs);
		socketFS = conectarseA(fsIP, fsPort);
	}

	enviarInt(socketFS,PROCESO_YAMA);

	int estadoFS;
	recibirInt(socketFS, &estadoFS);

	int i = 1;
	while(estadoFS == FSYS_NO_ESTABLE){
		log_trace(logYamaErrorImpreso, "filesystem no estable, esperando a que se estabilice\nse intentara de nuevo en %d segundos", i);
		sleep(i);
		enviarInt(socketFS, PROCESO_YAMA);
		recibirInt(socketFS, &estadoFS);
		i++;
	}

	log_trace(logYamaImpreso, "filesystem estable");

	esperarConexion = malloc(sizeof(t_esperar_conexion));
	esperarConexion->fdSocketEscucha = fdSocketsEscucha;
	esperarConexion->socketEscucha = socketEscucha;

	esperarConexionMasterYFS((void*) esperarConexion);

	log_destroy(logYama);
	log_destroy(logYamaError);
	log_destroy(logYamaErrorImpreso);
	log_destroy(logYamaImpreso);

	return EXIT_SUCCESS;
}



