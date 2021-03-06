/*
 * funcionesnodo.h
 *
 *  Created on: 30/9/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "../../bibliotecas/protocolo.h"
#include "../../bibliotecas/serializacion.h"
#include <commons/config.h>
#include <sys/mman.h>
#include <fcntl.h>


#ifndef FUNCIONESNODO_H_
#define FUNCIONESNODO_H_

#define BUFBLOQ 1024

t_log* logNodo;

typedef struct {
  char nombre[10];
  int espacio_libre;
  int espacio_total;
  int puerto;
} t_nodo;

t_config* infoConfig;
char* fsIP;
int fsPort;
char* nombreNodo;
char* rutaNodo;
int socketConn;
int puerto;

t_nodo * inicializoDataBin(char* rutaBin, char* nombreNodo, int puerto);
void iniciarDataNode();

int esperarBloque(int socketConn,t_nodo* nodo, char* rutaNodo);
int leerBloque(int socketConn,t_nodo* nodo, char* rutaNodo);

void *serializarNodo(t_nodo* nodo);

#endif /* FUNCIONESNODO_H_ */
