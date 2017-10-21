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
#include <commons/string.h>
#include <commons/bitarray.h>
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
  char nombre_nodo[10];
  int tamanio;
  int espacioLibre;
} t_nodo;

pthread_mutex_t mx_nodobin;

void *escucharConsola();
void *esperarConexiones(void *args);
void procesarSolicitudMaster(int nuevoSocket);
int recibirConexionDataNode(int nuevoSocket);
void actualizarNodosBin();
void deserializar_a_nodo(void* serializado, t_nodo *nodo);
t_nodo* getNodoPorNombre(char* nombre_nodo, t_list* listaABuscar);

/*Funciones de bitmap*/
t_bitarray* creaAbreBitmap(int tamNodo, char* nombreNodo[10]);
int findFreeBloque(int tamNodo, t_bitarray* t_fs_bitmap);
void escribirBitMap(int tamNodo, char* nombreNodo[10], t_bitarray* t_fs_bitmap);

/*Funciones de Filesys*/
void guardarArchivoLocalEnFS(char* path_archivo_origen, char* directorio_yamafs, t_list* folderList);
void traerArchivoDeFs(char* archivoABuscar, void* parametro, t_list* folderList);
int obtenerMD5Archivo(char * archivo);

int leerBloque(t_nodo * nodo, int bloque, int largo, unsigned char * buffer);
void recibirDatosBloque(t_nodo * nodo);
void escucharConexionNodo(void* socket);


/*Funciones de directorio.dat*/
t_list* inicializarDirectorios();
void listarDirectorios(t_list* folderList, t_directory* carpetaActual);
void ordenoDirectorios(t_list* folderList);
void crearDirectorio(t_list* folderList, t_directory* carpetaActual, char* nombre);
int identificaDirectorio(char* directorio_yamafs, t_list* folderList);
char* getRutaMetadata(char* ruta_archivo, t_list* folderList);
char* getNombreArchivo(char* path);

/*Misc*/
char* replace_char(char* str, char find, char replace);

#endif /* FUNCIONESFS_H_ */
