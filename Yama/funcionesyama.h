/*
 * funcionesyama.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include <sys/socket.h>

#ifndef YAMA_FUNCIONESYAMA_H_
#define YAMA_FUNCIONESYAMA_H_

typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;

void *esperarConexionesYama(void *args);

#endif /* YAMA_FUNCIONESYAMA_H_ */
