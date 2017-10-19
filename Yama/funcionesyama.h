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
#include <stdint.h>

#ifndef YAMA_FUNCIONESYAMA_H_
#define YAMA_FUNCIONESYAMA_H_

typedef struct {
	int socketCliente;
} t_esperar_mensaje;

t_config* infoConfig;
int dispBase;
char* fsIP;
int fsPort;
int retardoPlanificacion; //en milisegundos
char* algoritmoBalanceo;
uint32_t nombreRutaTemporal;
uint32_t rutaGlobal;



typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;


typedef struct{
	int numeroBloque;
	int bytesOcupados;
}t_bloque;



void *esperarConexionMaster(void *args);
void inicializarConfigYama();



#endif /* YAMA_FUNCIONESYAMA_H_ */
