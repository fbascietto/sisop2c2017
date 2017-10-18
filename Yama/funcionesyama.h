/*
 * funcionesyama.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <commons/config.h>
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "../Master/interface.h"

#ifndef YAMA_FUNCIONESYAMA_H_
#define YAMA_FUNCIONESYAMA_H_

t_config* infoConfig;
int dispBase;
char* fsIP;
int fsPort;
int retardoPlanificacion; //en milisegundos
char* algoritmoBalanceo;



typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;




void *esperarConexionMaster(void *args);
void inicializarConfigYama();



#endif /* YAMA_FUNCIONESYAMA_H_ */
