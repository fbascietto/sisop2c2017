

#include <pthread.h>
#include "funcionesyama.h"
#include "prePlanificacion.h"
#include "../bibliotecas/protocolo.h"
#include "../bibliotecas/estructuras.h"
#include "interfaceMaster.h"



void recibirMensajeMaster(void *args){
	t_esperar_mensaje *argumentos = (t_esperar_mensaje*) args;
	int nuevoSocket = argumentos->socketCliente;
	free(args);

	while(1){
		Package* package = createPackage();
		int leidos = recieve_and_deserialize(package, nuevoSocket);

		printf("codigo de mensaje: %d\n",	package->msgCode);

		switch(package->msgCode){

		case ACCION_PROCESAR_ARCHIVO:
			procesarSolicitudArchivoMaster(nuevoSocket, package);
			break;
		case TRANSFORMACION_OK:
			procesarResultadoTransformacion(nuevoSocket, package, TRANSFORMACION_OK);
			break;
		case REDUCCION_LOCAL_OK:
			procesarResultadoReduccionLocal(nuevoSocket, package, REDUCCION_LOCAL_OK);
			break;
		case REDUCCION_GLOBAL_OK:
			procesarResultadoReduccionGlobal(nuevoSocket, package, REDUCCION_GLOBAL_OK);
			break;
		case ALMACENADO_FINAL_OK:
			procesarResultadoAlmacenadoFinal(nuevoSocket, package, ALMACENADO_FINAL_OK);
			break;
		case TRANSFORMACION_ERROR:
			procesarResultadoTransformacion(nuevoSocket, package, TRANSFORMACION_ERROR);
			break;
		case REDUCCION_LOCAL_ERROR:
			procesarResultadoReduccionLocal(nuevoSocket, package, REDUCCION_LOCAL_ERROR);
			break;
		case REDUCCION_GLOBAL_ERROR:
			procesarResultadoReduccionGlobal(nuevoSocket, package, REDUCCION_GLOBAL_ERROR);
			break;
		case ALMACENADO_FINAL_ERROR:
			procesarResultadoAlmacenadoFinal(nuevoSocket, package, ALMACENADO_FINAL_ERROR);
			break;
		}
		sleep(1);
	}
}

/*
 * crea el job y lo agrega a jobsActivos
 * en base a un paquete de bloques
 */
void crearNuevoJob(int idMaster, t_list* bloques, t_job* nuevoJob, char* algoritmo) {
	t_list* nodos;
	t_list* bloquesReducidos;
	// ---------------------------- CREO JOB ----------------------------------------------

	nodos = obtenerNodosParticipantes(bloques);
	bloquesReducidos = reducirBloques(bloques);
	nuevoJob = crearJob(bloquesReducidos, nodos, algoritmo, idMaster);
	list_add(jobsActivos, nuevoJob);

	//todo
	//loguearEstadoJob(nuevoJob, RECIBIR_BLOQUES);
}

void recibirMensajeFS(void *args){
	t_esperar_mensaje *argumentos = (t_esperar_mensaje*) args;
	int nuevoSocket = argumentos->socketCliente;
	free(args);

	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);

	printf("codigo de mensaje: %d\n",	package->msgCode);


	/* declaro variables para el job y la solicitud de transformacion */
	t_job* nuevoJob;
	char* algoritmo = algoritmoBalanceo;

	uint32_t idMaster;

	solicitud_transformacion* solicitudTransformacion;
	char* solicitudTransfSerializado;
	uint32_t longitud;
	int enviados;
	t_list* bloques;

	bloques = procesarBloquesRecibidos(package->message, &idMaster);

	/* proceso los bloques */
	switch(package->msgCode){
	case RECIBIR_BLOQUES:

		crearNuevoJob(idMaster, bloques, nuevoJob, algoritmo);

		// ------------------ ENVIO SOLICITUD TRANSFORMACION A MASTER -------------------------

		solicitudTransformacion = obtenerSolicitudTrasnformacion(nuevoJob);

		solicitudTransfSerializado = serializarSolicitudTransformacion(solicitudTransformacion);
		longitud = getLong_SolicitudTransformacion(solicitudTransformacion);

		enviados = enviarMensajeSocketConLongitud(idMaster,ACCION_PROCESAR_TRANSFORMACION,solicitudTransfSerializado,longitud);
	}

	//todo hacer free
}

void *esperarConexionMasterYFS(void *args) {

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------


	/*int nuevoSocket;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,
				argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);

		}*/

	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;

	printf("Esperando conexiones en Yama...\n");


	while(1){

		//TODO: Recibir instrucciones master y crear thread por cada una
		int nuevoSocket = -1;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,argumentos->socketEscucha);

		if (nuevoSocket != -1) {

			//log_trace(logSockets,"Nuevo Socket!");
			printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);
			int cliente;
			recibirInt(nuevoSocket,&cliente);

			/* define el thread */
			pthread_t threadSolicitudes;
			t_esperar_mensaje *tEsperarMensaje = malloc(sizeof(t_esperar_mensaje));
			tEsperarMensaje->socketCliente = nuevoSocket;
			int er1;

			switch(cliente){
			//TODO: iniciar un hilo para manejar cliente
			case PROCESO_MASTER:
				er1 = pthread_create(&threadSolicitudes, NULL,recibirMensajeMaster,(void*) tEsperarMensaje);
				pthread_join(threadSolicitudes, NULL);
				break;
			case PROCESO_FILESYSTEM:
				tEsperarMensaje->socketCliente = nuevoSocket;
				er1 = pthread_create(&threadSolicitudes, NULL, recibirMensajeFS,(void*) tEsperarMensaje);
				pthread_join(threadSolicitudes, NULL);
				break;
			}
		}
		sleep(1);
	}
}


solicitud_transformacion* obtenerSolicitudTrasnformacionMock(char* message){
	item_transformacion* item1 = crearItemTransformacion("nodo 1","127.0.0.1",8080,2222,12345,"/temp1/archivo1.txt");
	item_transformacion* item2 = crearItemTransformacion("nodo 2","127.23.0.1",0101,523,5777666,"/temsssssp1211/otro.txt");
	item_transformacion* item3 = crearItemTransformacion("nodo 3","187.0.56.1",9090,62,643,"/temp655/tercero.txt");

	solicitud_transformacion* solicitudTransformacion = malloc(sizeof(solicitud_transformacion));

	solicitudTransformacion->item_cantidad = 0;

	agregarItemTransformacion(solicitudTransformacion,item1);
	free(item1);
	agregarItemTransformacion(solicitudTransformacion,item2);
	free(item2);
	agregarItemTransformacion(solicitudTransformacion,item3);
	free(item3);
	return solicitudTransformacion;
}

solicitud_reduccion_local* obtenerSolicitudReduccionLocalMock(char* message){
	archivo_temp *arch_temp11 = crearArchivoTemporal("/tmp/Master1-temp11");
	archivo_temp *arch_temp12 = crearArchivoTemporal("/tmp/Master1-temp12");
	item_reduccion_local* item1 = crearItemReduccionLocal("nodo 1","127.0.0.1",8080,"/tmp/Master1-temp38");
	agregarArchivoTemporalTransf(item1, arch_temp11);
	free(arch_temp11);
	agregarArchivoTemporalTransf(item1, arch_temp12);
	free(arch_temp12);

	item_reduccion_local* item2 = crearItemReduccionLocal("nodo 2","127.23.0.1",0101,"/tmp/Master1-Worker1");
	archivo_temp *arch_temp21 = crearArchivoTemporal("/tmp/Master1-temp21");
	archivo_temp *arch_temp22 = crearArchivoTemporal("/tmp/Master1-temp22");
	agregarArchivoTemporalTransf(item2, arch_temp21);
	free(arch_temp21);
	agregarArchivoTemporalTransf(item2, arch_temp22);
	free(arch_temp22);

	item_reduccion_local* item3 = crearItemReduccionLocal("nodo 3","187.0.56.1",9090,"/tmp/Master1-Worker2");
	archivo_temp *arch_temp31 = crearArchivoTemporal("/tmp/Master1-temp31");
	archivo_temp *arch_temp32 = crearArchivoTemporal("/tmp/Master1-temp32");
	agregarArchivoTemporalTransf(item3, arch_temp31);
	free(arch_temp31);
	agregarArchivoTemporalTransf(item3, arch_temp32);
	free(arch_temp32);

	solicitud_reduccion_local* solicitudReduccionLocal = malloc(sizeof(solicitud_reduccion_local));

	solicitudReduccionLocal->item_cantidad = 0;

	agregarItemReduccionLocal(solicitudReduccionLocal,item1);
	free(item1);
	agregarItemReduccionLocal(solicitudReduccionLocal,item2);
	free(item2);
	agregarItemReduccionLocal(solicitudReduccionLocal,item3);
	free(item3);
	return solicitudReduccionLocal;
}

solicitud_reduccion_global* obtenerSolicitudReduccionGlobalMock(char* message){
	t_worker* item1 = crearItemWorker("nodo 1","127.0.0.1",8080,"/tmp/Master1-temp38");
	t_worker* item2 = crearItemWorker("nodo 2","127.23.0.1",0101,"/tmp/Master1-temp39");
	t_worker* item3 = crearItemWorker("nodo 3","187.0.56.1",9090,"/tmp/Master1-temp44");

	t_worker* itemEncargado = crearItemWorker("nodo 1","187.0.56.1",9090,"/tmp/ruta_encargado");

	solicitud_reduccion_global* solicitudReduccionGlobal = malloc(sizeof(solicitud_reduccion_global));

	solicitudReduccionGlobal->item_cantidad = 0;

	agregarItemWorker(solicitudReduccionGlobal,item1);
	free(item1);
	agregarItemWorker(solicitudReduccionGlobal,item2);
	free(item2);
	agregarItemWorker(solicitudReduccionGlobal,item3);
	free(item3);

	solicitudReduccionGlobal->encargado_worker = itemEncargado;
	strcpy(solicitudReduccionGlobal->archivo_temporal_reduccion_global, "/tmp/archivoResultante");

	return solicitudReduccionGlobal;
}

solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinalMock(char* message){
	solicitud_almacenado_final *solicitud = malloc(sizeof(solicitud_almacenado_final));
	strcpy(solicitud->archivo_temporal_reduccion_global, "/tmp/Master1-final");
	solicitud->nodo_id = 2;
	solicitud->puerto_worker = 5555;
	strcpy(solicitud->ip_worker,"192.168.1.11");
	return solicitud;
}


/*
 * TO DO
 * no son la version final
 */
solicitud_transformacion* obtenerSolicitudTrasnformacion(t_job* job){
	int i;
	t_estado* unEstado;
	int tamanioTransformacion = list_size(job->estadosTransformaciones);

	// menos 1 porque esta el de reduccion global en la planificacion
	solicitud_transformacion* solicitud = malloc(sizeof(solicitud_transformacion));
	item_transformacion* item = malloc(sizeof(item_transformacion));

	for(i=0; i<tamanioTransformacion ;i++){
		unEstado = list_get(job->estadosTransformaciones, i);
		item = crearItemTransformacion(unEstado->nodoPlanificado->nodo->idNodo,
				unEstado->nodoPlanificado->nodo->ipWorker,
				unEstado->nodoPlanificado->nodo->puerto,
				unEstado->nodoPlanificado->bloque->numeroBloque,
				unEstado->nodoPlanificado->bloque->bytesOcupados,
				unEstado->archivoTemporal);
		agregarItemTransformacion(solicitud, item);
	}
	return solicitud;
	//return obtenerSolicitudTrasnformacionMock(message);
}

solicitud_reduccion_local* obtenerSolicitudReduccionLocal(t_job* job){ // t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasTransformacionTemporales, char* rutaReduccion){

	int i;
	int j;
	int k;
	t_estado* unEstado;
	t_estado* estadoAux;
	archivo_temp* listaRutasTransformacion;
	int tamanioTransformacion = list_size(job->estadosTransformaciones);
	int tamanioReduccionLocal = list_size(job->estadosReduccionesLocales);

	solicitud_reduccion_local* solicitud = malloc(sizeof(solicitud_reduccion_local));
	item_reduccion_local* item = malloc(sizeof(item_reduccion_local));

	for(i=0; i<tamanioReduccionLocal ;i++){
		unEstado = list_get(job->estadosReduccionesLocales, i);
		k=0;
		for(j=0; j < tamanioTransformacion; j++){
			estadoAux = list_get(job->estadosTransformaciones, j);
			listaRutasTransformacion = realloc(listaRutasTransformacion, sizeof(archivo_temp) * (k+1));
			strncpy(listaRutasTransformacion[k].archivo_temp, estadoAux->archivoTemporal, LENGTH_RUTA_ARCHIVO_TEMP);
			k++;
		}
		item = crearItemReduccionLocal(unEstado->nodoPlanificado->nodo->idNodo,
				unEstado->nodoPlanificado->nodo->ipWorker,
				unEstado->nodoPlanificado->nodo->puerto,
				unEstado->archivoTemporal);
		item->archivos_temporales_transformacion = listaRutasTransformacion;
		agregarItemReduccionLocal(solicitud, item);

	}
	return solicitud;
	//return obtenerSolicitudReduccionLocalMock(message);
}

solicitud_reduccion_global* obtenerSolicitudReduccionGlobal(t_job* job){ //t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasRedLocalTemporales, char* rutaReduccionGlobal){

	int i;
	t_estado* unEstado;
	int tamanioReduccionLocal = list_size(job->estadosReduccionesLocales);

	solicitud_reduccion_global* solicitud = malloc(sizeof(solicitud_reduccion_global));
	t_worker* item = malloc(sizeof(t_worker));

	for(i=0; i<tamanioReduccionLocal ;i++){
		unEstado = list_get(job->estadosReduccionesLocales, i);
		item = crearItemWorker(unEstado->nodoPlanificado->nodo->idNodo,
				unEstado->nodoPlanificado->nodo->ipWorker,
				unEstado->nodoPlanificado->nodo->puerto,
				unEstado->archivoTemporal);
		agregarItemWorker(solicitud, item);
	}
	unEstado = job->reduccionGlobal;
	item = crearItemWorker(unEstado->nodoPlanificado->nodo->idNodo,
			unEstado->nodoPlanificado->nodo->ipWorker,
			unEstado->nodoPlanificado->nodo->puerto,
			NULL);
	strcpy(solicitud->archivo_temporal_reduccion_global, item->archivo_temporal_reduccion_local);
	solicitud->encargado_worker = item;
	return solicitud;
}

solicitud_almacenado_final* obtenerSolicitudAlmacenadoFinal(t_job* job){ //t_nodo* nodoEncargado, int puerto_worker, char* ip_worker, char* rutaReduccionGlobal){

	t_estado* unEstado;

	solicitud_almacenado_final* solicitud = malloc(sizeof(solicitud_almacenado_final));

	unEstado = job->reduccionGlobal;

	strncpy(solicitud->nodo_id, unEstado->nodoPlanificado->nodo->idNodo, NOMBRE_NODO);
	solicitud->puerto_worker = unEstado->nodoPlanificado->nodo->puerto;
	strcpy(solicitud->ip_worker, unEstado->nodoPlanificado->nodo->ipWorker);
	strcpy(solicitud->archivo_temporal_reduccion_global, unEstado->archivoTemporal);


	return solicitud;
	//return obtenerSolicitudAlmacenadoFinalMock(message);
}


void procesarResultadoTransformacion(int nuevoSocket, Package* package, uint32_t resultado){
	/*
	 * todo:
	 * (i) 	deserealizar el resultado de transformacion (orden: primero resultado idNodo )
	 * (ii)	evaluar si el resultado es ok o error, si es error, replanificar
	 * (iii)actualizar tabla de estados
	 * (iv)	si esta ok, evaluar si ya se terminaron todos los bloques
	 * (v)	si ya se terminaron crear dicha reduccion local para el nodo
	 */

	/* (i) */
	int offset = 0;
	char idNodo[NOMBRE_NODO];
	uint32_t numeroBloque;

	deserializarDato(&numeroBloque, package->message, sizeof(uint32_t), &offset);
	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);

	/* (iii) */
	actualizarEstado(idNodo, numeroBloque, RESULTADO_TRANSFORMACION, idJob, resultado);

	/* (ii) */
	if(resultado == TRANSFORMACION_OK){


		/* (iv) */
		if(finalizoTransformacionesNodo(idNodo, numeroBloque, idJob)){

			/* (v) */

			t_job* job = obtenerJob(idJob, jobsActivos);
			solicitud_reduccion_local* solicitudTransformacion = obtenerSolicitudReduccionLocal(job);
			char* solicitudSerializado = serializarSolicitudReduccionLocal(solicitudTransformacion);
			uint32_t longitud = getLong_SolicitudReduccionLocal(solicitudTransformacion);
			int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_REDUCCION_LOCAL,solicitudSerializado,longitud);

		}
	} else{

		/**** fallo uno de los nodos ***/
		int enviados;

		//todo
		enviados = enviarMensajeSocketConLongitud(nuevoSocket, ACCION_REPLANIFICACION, NULL, NULL);

		t_job* jobFallado;
		t_list* bloques;

		/***** termino el job *****/

		jobFallado = terminarJob(idJob);

		/********* JOB *************/
		bloques = obtenerBloques(jobFallado);
		t_job* jobReplanificado;

		desconectarNodo(idNodo);


		//todo sePuedePlanificar hablar con francisco
		if(sePuedePlanificar(bloques)){

			solicitud_transformacion* solicitudTransformacion;
			char* solicitudTransfSerializado;
			uint32_t longitud;

			crearNuevoJob(idMaster, bloques, jobReplanificado, algoritmoBalanceo);

			solicitudTransformacion = obtenerSolicitudTrasnformacion(jobReplanificado);

			solicitudTransfSerializado = serializarSolicitudTransformacion(solicitudTransformacion);
			longitud = getLong_SolicitudTransformacion(solicitudTransformacion);

			enviados = enviarMensajeSocketConLongitud(idMaster,ACCION_PROCESAR_TRANSFORMACION,solicitudTransfSerializado,longitud);

			free(solicitudTransformacion);
			free(solicitudTransfSerializado);
		}else{
			free(bloques);

		}
	}
}

void procesarResultadoReduccionLocal(int nuevoSocket, Package* package, uint32_t resultado){

	int offset = 0;
	char idNodo[NOMBRE_NODO];
	uint32_t numeroBloque;

	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);
	actualizarEstado(idNodo, NULL, RESULTADO_REDUCCION_LOCAL, idJob, resultado);

	t_job* job = obtenerJob(idJob, jobsActivos);

	if(resultado == REDUCCION_LOCAL_OK){
		if(finalizaronReduccionesLocalesNodos(job)){

			solicitud_reduccion_global* solicitudReduccionGlobal = obtenerSolicitudReduccionGlobal(job);
			char* solicitudSerializado = serializarSolicitudReduccionGlobal(solicitudReduccionGlobal);
			uint32_t longitud = getLong_SolicitudReduccionGlobal(solicitudReduccionGlobal);
			int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_REDUCCION_GLOBAL,solicitudSerializado,longitud);

		}
	}else{
		job = terminarJob(idJob);
	}



}

void procesarResultadoReduccionGlobal(int nuevoSocket, Package* package, uint32_t resultado){

	int offset = 0;
	char idNodo[NOMBRE_NODO];
	uint32_t numeroBloque;

	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);
	actualizarEstado(idNodo, NULL, RESULTADO_REDUCCION_GLOBAL, idJob, resultado);

	t_job* job = obtenerJob(idJob, jobsActivos);

	if(resultado == REDUCCION_GLOBAL_OK){
		solicitud_almacenado_final* solicitudAlmacenadoFinal = obtenerSolicitudAlmacenadoFinal(job);
		char* solicitudSerializado = serializarSolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
		uint32_t longitud = getLong_SolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
		int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_ALMACENADO_FINAL,solicitudSerializado,longitud);
	}else{
		job = terminarJob(idJob);
	}

}

void procesarResultadoAlmacenadoFinal(int nuevoSocket, Package* package, uint32_t resultado){

	int offset = 0;
	char idNodo[NOMBRE_NODO];
	uint32_t numeroBloque;

	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);
	actualizarEstado(idNodo, NULL, RESULTADO_REDUCCION_GLOBAL, idJob, resultado);

	terminarJob(idJob);
}

void procesarSolicitudArchivoMaster(int nuevoSocket, Package* package){
	//solicitud_transformacion* solicitudTransformacion = obtenerSolicitudTrasnformacion(message);
	char* solicitudArchivo = malloc(package->message_long+1);
	uint32_t* tamanioSerializado = malloc(sizeof(uint32_t));

	strcpy(solicitudArchivo, package->message);
	solicitudArchivo[(package->message_long)+1] = '\0';

	enviarInt(socketFS, nuevoSocket);
	enviarMensaje(socketFS, solicitudArchivo);

	free(solicitudArchivo);
	free(package);
	free(tamanioSerializado);

}

//se obtiene ademas el id del master (va primero en la serializacion ((todo))
t_list* procesarBloquesRecibidos(char* message, uint32_t* masterId){
	t_bloques_enviados* bloquesRecibidos;

	//todo
	bloquesRecibidos = deserializarBloques(message, masterId);

	t_list* bloques = list_create();

	adaptarBloques(bloquesRecibidos, bloques);

	return bloques;
}

/*
 * pasa los bloques de bloquesRecibidos a bloques
 * con formatos t_bloque*
 */
void adaptarBloques(t_bloques_enviados* bloquesRecibidos, t_list* bloques){
	t_bloque* unBloque = malloc(sizeof(t_bloque));
	t_bloque_serializado* bloqueRecibido;

	int i;
	for(i=0; i<bloquesRecibidos->cantidad_bloques; i++){
		bloqueRecibido = &(bloquesRecibidos->lista_bloques[i]);

		unBloque->bytesOcupados = bloqueRecibido->bytes_ocupados;
		strcpy(unBloque->idNodo, bloqueRecibido->idNodo);
		strcpy(unBloque->ip, bloqueRecibido->ip);
		unBloque->numeroBloque = bloqueRecibido->numero_bloque;
		unBloque->puerto = bloqueRecibido->puerto;

		list_add(bloques, unBloque);

		free(bloqueRecibido);
	}
}

/*
 * funcion muy generica para todos los estados
 * en particular:
 * si se actualiza el estado de la reduccion global con ok
 * poner "esperando almacenado"
 * y si se pone de actualizar el estado de almacenado final a ok
 * modificar el estado de reduccion global a "job finalizado
 */
void actualizarEstado(char* idNodo, int numeroBloque, int etapa, int idJob, int resultado){
	t_job* job;
	t_estado* estado;

	job = obtenerJob(idJob, jobsActivos);

	switch(etapa){
	case RESULTADO_TRANSFORMACION:
		//todo estado = obtenerEstadoTransformacion(job->estadosTransformaciones, idNodo, numeroBloque);
		if(resultado == TRANSFORMACION_OK){
			strcpy(estado->estado, "finalizado");
		}else{
			//todo actualizarEstadoError(job, idNodo, resultado);
		}
		break;

	case RESULTADO_REDUCCION_LOCAL:
		//todo estado = obtenerEstadoRedLoc(job->estadosReduccionesLocales, idNodo);
		if(resultado == REDUCCION_LOCAL_OK){
			strcpy(estado->estado, "finalizado");
		}else{
			//todo actualizarEstadoError(job, idNodo, resultado);
		}
		break;

	case RESULTADO_REDUCCION_GLOBAL:
		estado = job->reduccionGlobal;
		if(resultado == REDUCCION_GLOBAL_OK){
			strcpy(estado->estado, "esperando almacenado");
		}else{
			//todo actualizarEstadoError(job, idNodo, resultado);
		}
		break;
	case RESULTADO_ALMACENADO_FINAL:
		estado = job->reduccionGlobal;
		if(resultado == ALMACENADO_FINAL_OK){
			strcpy(estado->estado, "job exitoso");
		}
		break;
	}
}

bool finalizoTransformacionesNodo(char* idNodo, int numeroBloque, int idJob){
	int i;
	int tamanioTransformaciones;
	t_job* job;
	t_estado* unEstado;

	job = obtenerJob(idJob, jobsActivos);
	tamanioTransformaciones = list_size(job->estadosTransformaciones);

	for(i=0; i<tamanioTransformaciones; i++){
		unEstado = list_get(job->estadosTransformaciones, i);

		if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idNodo)== 0 && !termino(unEstado)){
			return false;
		}
	}

	return true;
}


bool finalizaronReduccionesLocalesNodos(t_job* job){
	return list_all_satisfy(job->estadosReduccionesLocales, termino);
}

bool termino(void* elemento){
	t_estado* nodo = (t_estado*) elemento;
	return !strcmp(nodo->estado, "finalizado");
}

void procesarSolicitudMaster(int nuevoSocket){
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
 * recibe un string y devuelve una ruta generada
 * la ruta es la direccion /tmp/ y el
 * nombre del archivo es un simple contador que siempre aumenta
 * por cada vez que pase por la funcion
 */
char* generarRutaTemporal(){
	rutaGlobal++;
	char* ruta = malloc(LENGTH_RUTA_ARCHIVO_TEMP);
	sprintf(ruta, "/tmp/%d", rutaGlobal);
	return ruta;
}


void inicializarConfigYama(){
	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* yama_log = log_create("logYama.txt", "YAMA", 0, level);

	log_trace(yama_log, "Inicializacion de la configuracion de Yama");

	t_config* infoConfig = config_create("../config.txt");

	if(config_has_property(infoConfig,"IP_FILESYSTEM")){
		fsIP = config_get_string_value(infoConfig,"IP_FILESYSTEM");
		printf("IP del filesystem: %s\n", fsIP);
	}

	if(config_has_property(infoConfig,"PUERTO_FILESYSTEM")){
		fsPort = config_get_int_value(infoConfig,"PUERTO_FILESYSTEM");
		printf("Puerto del filesystem: %d\n", fsPort);
	}
	cargarValoresPlanificacion();

	log_trace(yama_log, "Carga exitosa del archivo de configuracion");

	log_destroy(yama_log);


}

void cargarValoresPlanificacion(){
	t_log_level level = LOG_LEVEL_TRACE;
	t_log* yama_log = log_create("logYama.txt", "YAMA", 0, level);

	log_trace(yama_log, "Carga de valores de planificacion");

	t_config* infoConfig = config_create("../config.txt");

	if(config_has_property(infoConfig,"RETARDO_PLANIFICACION")){
		retardoPlanificacion = config_get_int_value(infoConfig,"RETARDO_PLANIFICACION");
		printf("Tiempo de retardo (en milisegundos: %d\n", retardoPlanificacion);
	}
	if(config_has_property(infoConfig,"ALGORITMO_BALANCEO")){
		algoritmoBalanceo = config_get_string_value(infoConfig,"ALGORITMO_BALANCEO");
		printf("Algoritmo de balanceo seleccionado: %s\n", algoritmoBalanceo);
	}
	if(config_has_property(infoConfig,"DISP_BASE")){
		dispBase = config_get_int_value(infoConfig,"DISP_BASE");
		printf("Disponibilidad Base: %d\n", dispBase);
	}

	log_destroy(yama_log);

}

void recargarConfiguracion(int signal){

		printf("SIGUSR1 recibido correctamente\n");
		printf("SIGUSR1 cargando nuevamente configuracion\n");
		cargarValoresPlanificacion();

}
