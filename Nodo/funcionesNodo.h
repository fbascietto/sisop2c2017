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
#include "../bibliotecas/protocolo.h"
#include <commons/config.h>

#ifndef FUNCIONESNODO_H_
#define FUNCIONESNODO_H_

t_config* infoConfig;
char* fsIP;
int fsPort;
char* nombreNodo;
int socketConn;


void iniciarDataNode();



#endif /* FUNCIONESNODO_H_ */
