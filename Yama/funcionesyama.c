#include "funcionesyama.h"

#include "../bibliotecas/sockets.h"
#include <commons/collections/list.h>
#include <stdbool.h>
#include "../bibliotecas/protocolo.h"
#include "../Master/interface.h"


void deserializarDato(void* dato, char* buffer, int size, int* offset){
	memcpy(dato,buffer + *offset,size);
	*offset += size;
}

void *esperarConexionMaster(void *args) {

	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;

	printf("Esperando conexiones en Yama...\n");

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------


		int nuevoSocket;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,
				argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);

		}

		while(1){
			//TODO: Recibir instrucciones master y crear thread por cada una
			int nuevoSocket = -1;

			nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,argumentos->socketEscucha);

			if (nuevoSocket != -1) {
				//log_trace(logSockets,"Nuevo Socket!");
				printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);
				int cliente;
				recibirInt(nuevoSocket,&cliente);
				switch(cliente){
					case PROCESO_MASTER:
						recibirSolicitudMaster(nuevoSocket);
				}


			}
		}
}

void recibirSolicitudMaster(int nuevoSocket){
	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);
	printf("codigo de mensaje: %d\n",	package->msgCode);
	switch(package->msgCode){
		case ACCION_PROCESAR_ARCHIVO:
			procesarSolicitudArchivoMaster(nuevoSocket, package->message_long, package->message);
	}
}

void procesarSolicitudArchivoMaster(int nuevoSocket, uint32_t message_long, char* message){
	//en este caso recibo solo un string como mensaje, pero en caso de recibir un objeto serializado, hay que deserializarlo antes.
	printf("mensaje recibido: %s\n",message);
	printf("longitudmensaje: %d\n",message_long);
}

void procesarSolicitudMaster(nuevoSocket){
    int protocolo;
	recibirInt(nuevoSocket,&protocolo);
	switch(protocolo){
		case ENVIAR_ARCHIVO_TEXTO:
			printf("Se recibio instruccion para recibir archivo de texto\n");
			recibirArchivo(nuevoSocket);
			break;
		}
}


/*
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
*/

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


