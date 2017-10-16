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

#define LENGTH_RUTA_ARCHIVO_TEMP 50
#define LENGTH_RUTA_PROGRAMA 200

/*Structs de comunicacion YAMA con MASTER*/

	//TRANSFORMACION

typedef struct item_transformacion {
	uint32_t nodo_id;		//nodo
	char ip_puerto_worker[20];		//IP y Puerto del Worker
	uint32_t bloque;		//bloque a aplicar programa de Transformacion
	uint32_t bytes_ocupados;		//Bytes Ocupados
	char archivo_temporal[50];		//Archivo temporal
} item_transformacion;

typedef struct solicitud_transformacion {
	item_transformacion* items_transformacion;		//array de item_transformacion
	uint32_t item_cantidad; //cantidad de items
} solicitud_transformacion;

	//REDUCCION LOCAL

typedef struct item_reduccion_local {
	uint32_t nodo_id;		//nodo
	char ip_puerto_worker[20];		//IP y Puerto del Worker
	char archivo_temporal_transformacion[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal transformacion
	char archivo_temporal_reduccion_local[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion local
} item_reduccion_local;

typedef struct solicitud_reduccion_local {
	item_reduccion_local* items_reduccion_local;	//array de item_reduccion_local
	uint32_t item_cantidad; //cantidad de items
} solicitud_reduccion_local;

	//REDUCCION GLOBAL

typedef struct item_reduccion_global {
	uint32_t nodo_id;		//nodo
	char ip_puerto_worker[20];		//IP y Puerto del Worker
	char archivo_temporal_reduccion_local[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion local
	char archivo_temporal_reduccion_global[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion global
	bool esEncargado;
} item_reduccion_global;

typedef struct solicitud_reduccion_global {
	item_reduccion_global* items_reduccion_global;		//array de item_reduccion_global
	uint32_t item_cantidad; //cantidad de items
} solicitud_reduccion_global;

	//ALMACENAMIENTO FINAL

typedef struct solicitud_almacenado_final {
	uint32_t nodo_id;		//nodo
	char ip_puerto_worker[20];		//IP y Puerto del Worker
	char archivo_temporal_reduccion_global[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion global
} solicitud_almacenado_final;

//-------------------------------------------------------------------------------------------------------------------


/*Structs de comunicacion MASTER con WORKER*/

	//TRANSFORMACION

typedef struct {
	char programa_transformacion[LENGTH_RUTA_PROGRAMA];
	uint32_t bloque;  //bloque a aplicar programa de Transformacion
	uint32_t bytes_ocupados;
	char archivo_temporal[LENGTH_RUTA_ARCHIVO_TEMP]; //ruta de archivo temporal
} solicitud_programa_transformacion;

	//REDUCCION LOCAL

typedef struct {
	char archivo_temp[LENGTH_RUTA_ARCHIVO_TEMP];
} archivo_temp;

typedef struct {
	char programa_reduccion[LENGTH_RUTA_PROGRAMA];
	archivo_temp* archivos_temporales;  //lista de archivos temporales
	uint32_t cantidad_archivos_temp;  //cantidad de archivos temporales en dicha lista
	char archivo_temporal_resultante[LENGTH_RUTA_ARCHIVO_TEMP];
} solicitud_programa_reduccion_local;

	//REDUCCION GLOBAL

typedef struct {
//definir este struct

} solicitud_programa_reduccion_global;


#endif /* ESTRUCTURAS_H_ */
