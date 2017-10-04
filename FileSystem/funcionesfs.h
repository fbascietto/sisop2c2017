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
#include "../bibliotecas/protocolo.h"


#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

char* nodos_file;
t_list * nodos;

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

typedef struct {
  int socket_nodo;
  char* nombre_nodo[20];
  int tamanio;
  int espacioLibre;
} t_nodo;

pthread_mutex_t mx_nodobin;

void *escucharConsola(void *args);
void *esperarConexiones(void *args);
void procesarSolicitudMaster(int nuevoSocket);
int recibirConexionDataNode(int nuevoSocket);
void actualizarNodosBin();
void crearBitmap(int tamNodo, char* nombreNodo[20]);
void guardarArchivoLocalEnFS(char* path_archivo_origen, char* directorio_yamafs);


t_list *inicializarDirectorios(t_list* folderList);
/* void listarDirectorios(t_list* folderList, int index);*/

#endif /* FUNCIONESFS_H_ */
