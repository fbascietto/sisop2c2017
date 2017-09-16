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
#include <errno.h>
#include <commons/log.h>
#include <commons/collections/list.h>


#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

typedef struct {
	int socketEscucha;
	fd_set fdSocketEscucha;
} t_esperar_conexion;

typedef struct arg_struct {
	int indice;
	t_list* lista;
    int padre;
} t_arg_consola;

typedef struct {
  int index;
  char* nombre;
  int padre;
} t_directory;

void *escucharConsola(void *args);
void *esperarConexiones(void *args);

t_list *inicializarDirectorios(t_list* folderList);
/* void listarDirectorios(t_list* folderList, int index);*/

#endif /* FUNCIONESFS_H_ */
