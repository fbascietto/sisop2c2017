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
#include "../bibliotecas/estructuras.h"


#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

#define ROUNDUP(x,y) ((x - 1) / y + 1)
// Redondear hacia arriba
#define CANTIDAD_BLOQUES_ARCHIVO(FILE_SIZE, BLOCK_SIZE) ((FILE_SIZE > 0) ? ROUNDUP(FILE_SIZE, BLOCK_SIZE) : 1)

char* nodos_file;
char* archivos_file;
t_list * nodos;
int cantNodos;
int estable;

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
  int bloquesLibres;
} t_nodo;

typedef struct {
	int bloque;
	char* Copia0;
	char* Copia1;
	int tamanio_bloque;
} t_bloque;

pthread_mutex_t mx_nodobin;

void *escucharConsola();
void *esperarConexiones(void *args);
void procesarSolicitudMaster(int nuevoSocket);
int recibirConexionDataNode(int nuevoSocket);
void levantarNodos(int clean);
void actualizarNodosBin();
void imprimeNodosBin();
int traeBloquesLibres();
void creoListaNodosDesdeNodosBin();
void deserializar_a_nodo(void* serializado, t_nodo *nodo);
t_nodo* getNodoPorNombre(char* nombre_nodo, t_list* listaABuscar);
t_list* getNodosMenosCargados(t_list* listaABuscar);

/*Funciones de bitmap*/
t_bitarray* creaAbreBitmap(int tamNodo, char nombreNodo[10]);
t_bitarray *crearBitmapVacio(int tamNodo);
t_bitarray *leerBitmap(FILE* bitmap_file, int tamNodo);
int findFreeBloque(int tamNodo, t_bitarray* t_fs_bitmap);
bool escribirBitMap(int tamNodo, char nombreNodo[10], t_bitarray* t_fs_bitmap);
int cuentaBloquesLibre(int tamNodo, t_bitarray* t_fs_bitmap);
t_bitarray *limpiar_bitmap(int tamNodo, char* nomNodo[10], t_bitarray* bitmap);
void destruir_bitmap(t_bitarray* bitmap);

/*Funciones de Filesys*/
void guardarArchivoLocalEnFS(char* path_archivo_origen, char* directorio_yamafs, t_list* folderList);
void guardarArchivoLocalDeTextoEnFS(char* path_archivo_origen, char* directorio_yamafs, t_list* folderList);
int traerArchivoDeFs(char* archivoABuscar, char* directorio, t_list* folderList, int md5flag);
void copioBloque(char* archivoABuscar, char* nodoDestino, t_list* folderList);

void removerArchivo(char* archivoABuscar, char* parametro, t_list* folderList);
void moverArchivo(char* archivoABuscar, char* destino);
void renombrarArchivo(char* archivoABuscar, char* nombreNuevo, t_list* folderList);

int obtenerMD5Archivo(char * archivo, t_list* folderList);
void imprimeMetadata(char* rutaEnYamafs, t_list* folderList);
int chequeoEstadoFS();
void * cls();

int escribirBloque(int socketnodo, int bloque, void * buffer, int largoAMandar);
int leerBloque(t_nodo * nodo, int bloque, int largo, unsigned char * buffer);
FILE * crearMetadata(char * destino, char* directorio_yamafs, t_list* folderList, char* tipo, int tamanio);
t_list * obtener_lista_metadata(char * ruta_metadata);
void recibirDatosBloque(t_nodo * nodo);
void actualizoArchivosDat(char* ruta_metadata, int flag);
int buscoEnArchivosDat(char* ruta_metadata);
int estaEstable();

int procesarSolicitudWorker(int nuevoSocket);

/*Funciones de directorio.dat*/
t_list* inicializarDirectorios();
void listarDirectorios(t_list* folderList, t_directory* carpetaActual);
void ordenoDirectorios(t_list* folderList);
void crearDirectorio(t_list* folderList, t_directory* carpetaActual, char* nombre);
int identificaDirectorio(char* directorio_yamafs, t_list* folderList);
char* getRutaMetadata(char* ruta_archivo, t_list* folderList, int carpeta);
char* getNombreArchivo(char* path);
int obtenerDirectorioFaltante(t_list* folderList);
t_directory * cambiarAdirectorio(char* nombre, t_directory* carpetaActual, t_list* folderList);
t_directory * cambiarAdirectorioConChequeo(char* nombre, t_directory* carpetaActual, t_list* folderList);
t_nodo* getDirectorioPorNombre(char* carpeta, t_list* folderList);
void actualizarDirectorioDat(t_list* folderList);

/*Funciones de serializacion y envio y recepcion de mensajes con Yama*/
void serializarDato(char* buffer, void* dato, int size_to_send, int* offset);
void deserializarDato(void* dato, char* buffer, int size, int* offset);
char* serializar_un_bloque(t_bloque_serializado* unBloque);
char* serialize_blocks(t_bloque_serializado** bloques, uint32_t item_cantidad);
uint32_t getLongitud_bloques(uint32_t item_cantidad);
uint32_t longitudBloques(t_bloques_enviados* bloques);
char* serializar_bloques(t_bloques_enviados* bloques, uint32_t* id_master, uint32_t* longitud);
int serializar_y_enviar_yama(t_bloques_enviados* bloques, uint32_t id_master, int socketYama);


/*Misc*/
char* replace_char(char* str, char find, char replace);

/*Deprecated*/
void escucharConexionNodo(void* socket);

#endif /* FUNCIONESFS_H_ */
