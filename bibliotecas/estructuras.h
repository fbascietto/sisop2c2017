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
#define LENGTH_NOMBRE_PROGRAMA 50
#define LENGTH_IP 20
#define LENGTH_ESTADO 30
#define LENGTH_ETAPA 30

/* Formatos YAMA */
#define NOMBRE_NODO 10

/* Tamaños de strings para strncpy */
#define LENGTH_TRANSFORMACION 14
#define LENGTH_REDUCCION_LOCAL 16
#define LENGTH_REDUCCION_GLOBAL 17
#define LENGTH_ALMACENADO_FINAL 17
#define LENGTH_EN_PROCESO 11
#define LENGTH_FINALIZADO 11


/*Structs de comunicacion YAMA con MASTER*/

//TRANSFORMACION

typedef struct {
	char nodo_id[NOMBRE_NODO];		//nodo
	char ip_worker[LENGTH_IP];		//IP y Puerto del Worker
	uint32_t puerto_worker;
	uint32_t bloque;		//bloque a aplicar programa de Transformacion
	uint32_t bytes_ocupados;		//Bytes Ocupados
	char archivo_temporal[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal
} item_transformacion;

typedef struct {
	item_transformacion* items_transformacion;		//array de item_transformacion
	uint32_t item_cantidad; //cantidad de items
} solicitud_transformacion;

//REDUCCION LOCAL

typedef struct {
	char archivo_temp[LENGTH_RUTA_ARCHIVO_TEMP];
} archivo_temp;

typedef struct item_reduccion_local {
	char nodo_id[NOMBRE_NODO];		//nodo
	char ip_worker[LENGTH_IP];
	uint32_t puerto_worker;
	archivo_temp* archivos_temporales_transformacion;		//archivos temporales de transformacion
	uint32_t cantidad_archivos_temp;
	char archivo_temporal_reduccion_local[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion local
} item_reduccion_local;

typedef struct solicitud_reduccion_local {
	item_reduccion_local* items_reduccion_local;	//array de item_reduccion_local
	uint32_t item_cantidad; //cantidad de items
} solicitud_reduccion_local;

//REDUCCION GLOBAL

typedef struct worker {
	char nodo_id[NOMBRE_NODO];		//nodo
	char ip_worker[LENGTH_IP];
	uint32_t puerto_worker;
	char archivo_temporal_reduccion_local[LENGTH_RUTA_ARCHIVO_TEMP];		//Archivo temporal reduccion local
} t_worker;

typedef struct solicitud_reduccion_global {
	t_worker* workers;		//array de worker
	uint32_t item_cantidad; //cantidad de items
	t_worker* encargado_worker;
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
	char programa_reduccion[LENGTH_NOMBRE_PROGRAMA];
	char* programa; //contenido del programa
	uint32_t length_programa;
	archivo_temp* archivos_temporales;  //lista de archivos temporales
	uint32_t cantidad_archivos_temp;  //cantidad de archivos temporales en dicha lista
	char archivo_temporal_resultante[LENGTH_RUTA_ARCHIVO_TEMP];
} solicitud_programa_reduccion_local;

//REDUCCION GLOBAL

typedef struct {
	char programa_reduccion[LENGTH_NOMBRE_PROGRAMA];
	char* programa; //contenido del programa
	uint32_t length_programa;
	t_worker* workers; 		/*lista de procesos Worker con sus respectivos IP, puerto y ruta de archivo temporal de
																			reduccion local*/
	uint32_t cantidad_workers; 		//cantidad de elementos en dicha lista
	char archivo_temporal_resultante[LENGTH_RUTA_ARCHIVO_TEMP];		//ruta del archivo temporal resultante de la reduccion global
} solicitud_programa_reduccion_global;


//ALMACENAMIENTO FINAL

typedef struct {

	char ruta_archivo_temporal_resultante_reduccion_global[LENGTH_RUTA_ARCHIVO_TEMP];
	char ruta_archivo_final_fs[LENGTH_RUTA_ARCHIVO_TEMP];

} solicitud_realizar_almacenamiento_final;

//-------------------------------------------------------------------------------------------------------------------


/*Structs de comunicacion WORKER con WORKER*/

typedef struct {
	char ruta_archivo_red_local_temp[LENGTH_RUTA_ARCHIVO_TEMP];  //ruta del archivo de RL del worker para recorrer y enviar de a un registro a la vez

} solicitud_leer_y_enviar_archivo_temp;

typedef struct {
	char* palabra;
	bool fin_de_archivo;

} solicitud_recibir_palabra;

//-------------------------------------------------------------------------------------------------------------------
/*Structs de comunicacion YAMA con FS*/

typedef struct{
	uint32_t numero_bloque;
	uint32_t bytes_ocupados;
	char idNodo[NOMBRE_NODO];
	char ip[LENGTH_IP];
	uint32_t puerto;
	uint32_t idBloque;
} t_bloque;

typedef struct{
	t_bloque* lista_bloques;
	uint32_t cantidad_bloques;
} t_bloques_enviados;

#endif /* ESTRUCTURAS_H_ */
