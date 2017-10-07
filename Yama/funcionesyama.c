#include "funcionesyama.h"

#include "../bibliotecas/sockets.h"
#include <commons/collections/list.h>
#include <stdbool.h>

void *esperarConexionesYama(void *args) {

	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;

	printf("Esperando conexiones en Yama...\n");

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------
	while (1) {

		int nuevoSocket;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,
				argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			printf("Nueva Conexion Recibida - Socket N°: %d\n",
					nuevoSocket);


		}

	}

	void prePlanificacionClock(int bloques, int dispBase, t_list* listaNodos){
		int bloque = 0;
		int numNodo = 0;
		int cantBloques = bloques;
		int valorBase = dispBase;
		t_nodo * nodo;
		t_nodo * aux;
		t_list* nodos = listaNodos;
		nodo = list_get(nodos, 0);

		for(;bloque < cantBloques;){

		if(tieneElBloque(nodo, bloque)){

			if((nodo -> disponibilidad) > 0){

				agregarWorkerPlanificacion(bloque, nodo);
				numNodo = siguienteWorker(numNodo, nodos, nodo);
				bloque++;

			} else{
				nodo -> disponibilidad = valorBase;
				numNodo = siguienteWorker(numNodo, nodos, nodo);
				bloque++;
			}

		} else{

			int auxNodo = numNodo;
			while(1){

				auxNodo = siguienteWorker(auxNodo, nodos, nodo);

				if(aux == nodo){
					agregarDisponibilidadWorkers(valorBase, nodos);
				}else{

					if(tieneElBloque(aux, bloque)){

						if((aux->  disponibilidad) > 0){
							agregarWorkerALaPlanificacion(bloque, aux);
							bloque++;
							break;

						} else{
							auxNodo = siguienteWorker(auxNodo, nodos, nodo);
						}

					} else{
						auxNodo = siguienteWorker(auxNodo, nodos, nodo);
					}
				}
			}
		}
	}
}

int siguienteWorker(int numNodo, t_list* nodos, t_nodo* nodo) {
//si no llegue al fin de la lista de nodos
	if (numNodo != list_size(nodos)) {
		numNodo++;
		nodo = list_get(nodos, numNodo);
	} else {
		numNodo = 0;
		nodo = list_get(nodos, numNodo);
	}
	return numNodo;
}


//void agregarWorkerALaPlanificacion(int bloque, t_nodo* worker) {
//	worker->disponibilidad = worker->disponibilidad - 1;
//	asignarBloque(worker, bloque);
//}



//bool tieneElBloque(t_nodo * nodo, int bloque){
//	t_list* bloques = nodo->bloques;
//	if (list_any_satisfy(bloques, )){
//			return true;
//		}else{
//			return false;
//		}
//	}
//}


