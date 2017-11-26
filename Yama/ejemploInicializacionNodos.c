#include "prePlanificacion.h"
#include "../bibliotecas/estructuras.h"
#include <stdio.h>

// TO-DO
void hacerPedidoDeTransformacionYRL(){

	char prueba[20];
	strcpy(prueba, "/tmp/archivoTemporal");

	char prueba2[20];
	strcpy(prueba2, "/tmp/archivoTemporal2");

	t_list* rutasTemporales = list_create();
	solicitud_transformacion* solicitud;
	t_list* planificacion = list_create();
	t_planificacion* unaPlanificacion = malloc(sizeof(t_planificacion));
	t_planificacion* otraPlanificacion = malloc(sizeof(t_planificacion));
	t_nodo* nodo = malloc(sizeof(t_nodo));
	t_list* bloques = list_create();
	t_bloque* bloque = malloc(sizeof(t_bloque));

	list_add(rutasTemporales, prueba);
	list_add(rutasTemporales, prueba2);

	bloque->bytesOcupados=999;
	bloque->numeroBloque=10;


	strcpy(nodo->idNodo, "nodo1");
	strcpy(nodo->ipWorker, "127.0.0.1");
	nodo->puerto = 5555;

	unaPlanificacion->nodo = nodo;
	unaPlanificacion->bloque = bloque;
	unaPlanificacion->reduccionGlobal = 0;

	otraPlanificacion->nodo = nodo;
	otraPlanificacion->bloque = NULL;
	otraPlanificacion->reduccionGlobal = 1;

	list_add(planificacion, unaPlanificacion);
	list_add(planificacion, otraPlanificacion);


	printf("inicio pedido transformacion\n");

	//solicitud = obtenerSolicitudTrasnformacion(planificacion, nodo->puerto, nodo->ipWorker, rutasTemporales);

	printf("termino la solicitud de transformacion\n");

	item_transformacion* items;

	printf("comienzo a mostrar elementos\n");

	items = solicitud->items_transformacion;

	printf("deberia haber 1 elemento, hay %d elemento(s)\n", solicitud->item_cantidad);
	printf("primer elemento tiene al nodo %s, bloque %d, ip %s\n\n", items[0].nodo_id, items[0].bloque, items[0].ip_worker);
	printf("la ruta temporal es %s\n", items[0].archivo_temporal);



	char ruta3[20];
	strcpy(ruta3, "/tmp/unaRutaTemporal");
	solicitud_reduccion_local* solicitudRL;
	item_reduccion_local* itemsRL;

	//solicitudRL = obtenerSolicitudReduccionLocal(planificacion, nodo->puerto, nodo->ipWorker, rutasTemporales, &ruta3);

	itemsRL = solicitudRL->items_reduccion_local;

	printf("deberia haber 1 elemento, hay %d elemento(s)\n", solicitud->item_cantidad);
	printf("primer elemento tiene al nodo %s, ruta del primer archivo temporal de transformacion %s, ip %s\n\n", itemsRL[0].nodo_id, itemsRL[0].archivos_temporales_transformacion[0].archivo_temp, itemsRL[0].ip_worker);
	printf("la ruta temporal del archivo de reduccion local es %s\n", itemsRL[0].archivo_temporal_reduccion_local);
}



/*
 * tener en cuenta que no es
 * responsabilidad de yama chequear
 * que esten todos los bloques
 * por lo que se bugea si falta un bloque
 * entre todos los nodos de la lista
 *
 * solo es responsabilidad hacer este
 * chequeo cuando se pide replanificar()
 */


/*
 * para probar otras variaciones
 * modificar asignarBloque de cada
 * nodo.
 *
 * para agregar mas bloques
 * modificar la variable cantidadBloques
 * y que coincida con las cantidad de bloques
 * distintos asignados
 *
 * EDIT: la preplanificacion puede
 * asignarse a cualquier lista de bloques
 */
void ejemploPrePlanificacion(){

		int i;

		t_list* listaBloques = list_create();

		//bloques
		t_bloque* bl0 = malloc(sizeof(t_bloque));
				bl0->bytesOcupados = 100;
		list_add(listaBloques, &bl0);

		t_bloque* bl1 = malloc(sizeof(t_bloque));
				bl1->bytesOcupados = 150;
		list_add(listaBloques, &bl1);

		t_bloque* bl2 = malloc(sizeof(t_bloque));
						bl2->bytesOcupados = 10294;
		list_add(listaBloques, &bl2);

		t_bloque* bl3 = malloc(sizeof(t_bloque));
						bl3->bytesOcupados = 2;
		list_add(listaBloques, &bl3);

		t_bloque* bl4 = malloc(sizeof(t_bloque));
						bl4->bytesOcupados = 720;
		list_add(listaBloques, &bl4);

		t_bloque* bl5 = malloc(sizeof(t_bloque));
						bl5->bytesOcupados = 102492;
		list_add(listaBloques, &bl5);

		int cantidadBloques = list_size(listaBloques);
		int valorBase = 1;
		char* tipoAlgoritmo = "WCLOCK";


		t_list* listaNodos = list_create();
		t_list* planificacion = list_create();
		t_planificacion* unaPlanificacion;
		int ejemploNodoFallado = 2;

		//inicializacion nodo0
		//le agrego aparte un valor a cargaDeTrabajo
		//para que quede igual que en el ejemplo del PPT
		t_nodo* nodo0 = malloc(sizeof(t_nodo));
		inicializarNodoTest(nodo0, "0");
		inicializarBloque(nodo0, bl0, 10);
		inicializarBloque(nodo0, bl4, 230);
		inicializarBloque(nodo0, bl5, 329);
		nodo0->cargaDeTrabajo++;
		list_add(listaNodos, nodo0);
		printf("nodo %s agregado a la lista\n", nodo0->idNodo);

		//inicializacion nodo1
		t_nodo* nodo1 = malloc(sizeof(t_nodo));
		inicializarNodoTest(nodo1, "1");
		inicializarBloque(nodo1, bl1, 1248);
		inicializarBloque(nodo1, bl2, 128);
		inicializarBloque(nodo1, bl3, 256);
		inicializarBloque(nodo1, bl5, 929);
		nodo1->cargaDeTrabajo++;
		nodo1->cargaDeTrabajoActual++;
		list_add(listaNodos, nodo1);
		printf("nodo %s agregado a la lista\n", nodo1->idNodo);

		//inicializacion nodo2
		t_nodo* nodo2 = malloc(sizeof(t_nodo));
		inicializarNodoTest(nodo2, "2");
		inicializarBloque(nodo2, bl0, 999);
		inicializarBloque(nodo2, bl1, 1024);
		inicializarBloque(nodo2, bl2, 1999);
		inicializarBloque(nodo2, bl3, 1996);
		inicializarBloque(nodo2, bl4, 2777);
		nodo2->cargaDeTrabajo++;
		nodo2->cargaDeTrabajoActual++;
		list_add(listaNodos, nodo2);
		printf("nodo %s agregado a la lista\n", nodo2->idNodo);

		planificacion = prePlanificacion(listaBloques, valorBase, listaNodos, tipoAlgoritmo);

		printf("ejecucion de preplanificacion exitosa\n\n");
		for(i=0; i<cantidadBloques; i++){
			unaPlanificacion = list_get(planificacion, i);
			printf("bloque %d  asignado al nodo: %s  ",
					unaPlanificacion->bloque->numeroBloque, unaPlanificacion->nodo->idNodo);
			printf("con disponibilidad restante : %d  y reduccionGlobal (deberia estar en 0): %d \n",
					unaPlanificacion->nodo->disponibilidad, unaPlanificacion->reduccionGlobal);
		}

		unaPlanificacion = list_get(planificacion, i);

		printf("nodo seleccionado para reduccion global: %s \n", unaPlanificacion->nodo->idNodo);
		printf("con carga de trabajo para la reduccion global: %d\n", unaPlanificacion->reduccionGlobal);
		printf("tamanio lista planificacion %d + 1 (nodo reduccion global): %d\n", cantidadBloques, list_size(planificacion));


		t_list* nuevaReplanificacion;
		nuevaReplanificacion = replanificacion(ejemploNodoFallado, listaBloques, valorBase, tipoAlgoritmo);


		if(nuevaReplanificacion==NULL){
				//todo terminarJob(planificacion);
			printf("TO-DO, deberia ejecutar el metodo terminarJob()");
		} else{
			printf("ejecucion de replanificacion exitosa\n\n");
			printf("tamanio de la lista de nodos: %d\n", list_size(listaNodos));
					for(i=0; i<cantidadBloques; i++){
						unaPlanificacion = list_get(nuevaReplanificacion, i);
						printf("bloque %d asignado al nodo: %s ",
								unaPlanificacion->bloque->numeroBloque, unaPlanificacion->nodo->idNodo);
						printf("con disponibilidad: %d y reduccionGlobal: %d \n",
								unaPlanificacion->nodo->disponibilidad, unaPlanificacion->reduccionGlobal);
					}

			unaPlanificacion = list_get(nuevaReplanificacion, i);

			printf("nodo seleccionado para reduccion global: %s \n", unaPlanificacion->nodo->idNodo);
			printf("con carga de trabajo para la reduccion global: %d\n", unaPlanificacion->reduccionGlobal);
			printf("tamanio lista replanificacion %d + 1 (nodo reduccion global): %d\n",
						cantidadBloques, list_size(nuevaReplanificacion));

		}

		printf("destruccion de listas para testear en vvalgrind\n");
		free(unaPlanificacion);
		list_destroy(planificacion);
		list_destroy(nuevaReplanificacion);
}


void inicializarNodoTest(t_nodo* nodo, char* id){
	strncpy(nodo->idNodo, id, NOMBRE_NODO);
	nodo->cargaDeTrabajo = 0;
	nodo->cargaDeTrabajoActual = 0;
	nodo->cargaDeTrabajoHistorica = 0;
	nodo->disponibilidad = 0;
}

void inicializarBloque(t_nodo* nodo, t_bloque* bloque, int numeroBloque){
	bloque->numeroBloque = numeroBloque;
	strcpy(bloque->idNodo, nodo->idNodo);
	printf("el nodo %s tiene un bloque en el lugar %d\n", nodo->idNodo, bloque->numeroBloque);
}



