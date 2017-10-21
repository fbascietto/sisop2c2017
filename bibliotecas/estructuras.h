/*
 * estructuras.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <stdint.h>
#include <stdbool.h>

#define TAMANIO_BLOQUE 1048576 //1 MB
#define LENGTH_RUTA_ARCHIVO_TEMP 50
#define LENGTH_NOMBRE_PROGRAMA 200
#define LENGTH_IP 20

/*Structs de comunicacion YAMA con MASTER*/

	//TRANSFORMACION

typedef struct item_transformacion {
	uint32_t nodo_id;		//nodo
	char ip_worker[LENGTH_IP];		//IP y Puerto del Worker
	uint32_t puerto_worker;
	uint32_t bloque;		//bloque a aplicar programa de Transformacion
	uint32_t bytes_ocupados;		//Bytes Ocupados
	char archivo_temporal[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal
} item_transformacion;

typedef struct solicitud_transformacion {
	item_transformacion* items_transformacion;		//array de item_transformacion
	uint32_t item_cantidad; //cantidad de items
} solicitud_transformacion;

	//REDUCCION LOCAL

typedef struct item_reduccion_local {
	uint32_t nodo_id;		//nodo
	char ip_worker[LENGTH_IP];
	uint32_t puerto_worker;
	archivo_temp* archivos_temporales_transformacion;		//Archivo temporal transformacion
	uint32_t cantidad_archivos_temp;
	char archivo_temporal_reduccion_local[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion local
} item_reduccion_local;

typedef struct solicitud_reduccion_local {
	item_reduccion_local* items_reduccion_local;	//array de item_reduccion_local
	uint32_t item_cantidad; //cantidad de items
} solicitud_reduccion_local;

	//REDUCCION GLOBAL

typedef struct item_reduccion_global {
	uint32_t nodo_id;		//nodo
	char ip_worker[LENGTH_IP];
	uint32_t puerto_worker;
	char archivo_temporal_reduccion_local[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion local
} item_reduccion_global;

typedef struct solicitud_reduccion_global {
	item_reduccion_global* items_reduccion_global;		//array de item_reduccion_global
	uint32_t item_cantidad; //cantidad de items
	item_reduccion_global* encargado_reduccion_global;
	char archivo_temporal_reduccion_global[LENGTH_RUTA_ARCHIVO_TEMP];
} solicitud_reduccion_global;

	//ALMACENAMIENTO FINAL

typedef struct solicitud_almacenado_final {
	uint32_t nodo_id;		//nodo
	char ip_worker[LENGTH_IP];
	uint32_t puerto_worker;
	char archivo_temporal_reduccion_global[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion global
} solicitud_almacenado_final;

//-------------------------------------------------------------------------------------------------------------------


/*Structs de comunicacion MASTER con WORKER*/

	//TRANSFORMACION

typedef struct {
	char programa_transformacion[LENGTH_NOMBRE_PROGRAMA];
	char* programa; //contenido del programa
	uint32_t length_programa;
	uint32_t bloque;  //bloque a aplicar programa de Transformacion
	uint32_t bytes_ocupados;
	char archivo_temporal[LENGTH_RUTA_ARCHIVO_TEMP]; //ruta de archivo temporal
} solicitud_programa_transformacion;

	//REDUCCION LOCAL

typedef struct {
	char archivo_temp[LENGTH_RUTA_ARCHIVO_TEMP];
} archivo_temp;

typedef struct {
	char programa_reduccion[LENGTH_NOMBRE_PROGRAMA];
	char* programa; //contenido del programa
	uint32_t length_programa;
	archivo_temp* archivos_temporales;  //lista de archivos temporales
	uint32_t cantidad_archivos_temp;  //cantidad de archivos temporales en dicha lista
	char archivo_temporal_resultante[LENGTH_RUTA_ARCHIVO_TEMP];
} solicitud_programa_reduccion_local;

	//REDUCCION GLOBAL

typedef struct {
	char ip_worker[LENGTH_IP];
	uint32_t puerto_worker;
	char archivo_temp_red_local[LENGTH_RUTA_ARCHIVO_TEMP];
} item_programa_reduccion_global;

typedef struct {
	char programa_reduccion[LENGTH_NOMBRE_PROGRAMA];
	char* programa; //contenido del programa
	uint32_t length_programa;
	item_programa_reduccion_global* items_programa_reduccion_global; 		/*lista de procesos Worker con sus respectivos IP, puerto y ruta de archivo temporal de
																			reduccion local*/
	uint32_t cantidad_item_programa_reduccion; 		//cantidad de elementos en dicha lista
	char archivo_temporal_resultante[LENGTH_RUTA_ARCHIVO_TEMP];		//ruta del archivo temporal resultante de la reduccion global
} solicitud_programa_reduccion_global;

//-------------------------------------------------------------------------------------------------------------------


/*Structs de comunicacion WORKER con WORKER*/

typedef struct {
//TODO: definir struct, tal vez ni sea struct este tipo de comunicacion

} solicitud_enviar_archivo_temp;

typedef struct {
//TODO: definir struct, tal vez ni sea struct este tipo de comunicacion

} solicitud_leer_archivo_temp;

//-------------------------------------------------------------------------------------------------------------------


#endif /* ESTRUCTURAS_H_ */
