/*
 * funcionesfs.h
 *
 *  Created on: 3/9/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;

void *escucharConsola();
void *esperarConexiones(void *args);
void *recibirArchivo(void *args);

#endif /* FUNCIONESFS_H_ */
