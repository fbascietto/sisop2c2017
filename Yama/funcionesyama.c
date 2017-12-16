

#include <pthread.h>
#include "funcionesyama.h"
#include "prePlanificacion.h"
#include "../bibliotecas/protocolo.h"
#include "../bibliotecas/estructuras.h"
#include "interfaceMaster.h"
#include "../bibliotecas/sockets.h"
#include <errno.h>


void recibirMensajeMaster(void *args){
	t_esperar_mensaje *tEsperarMensaje = (t_esperar_mensaje*) args;
	int nuevoSocket = tEsperarMensaje->socketCliente;
	//free(args);

	while(1){
		Package* package = createPackage();
		int leidos = recieve_and_deserialize(package, nuevoSocket);

		if(leidos == 0){
			close(nuevoSocket);
			break;
		}
		log_trace(logYamaImpreso, "bytes leidos %d", leidos);
		log_trace(logYamaImpreso, "codigo mensaje %d", package->msgCode);

		switch(package->msgCode){

		case ACCION_PROCESAR_ARCHIVO:
			log_trace(logYama, "iniciando nuevo job");
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
		free(package->message);
		free(package);
	}
}

/*
 * crea el job y guarda en jobGlobal
 * y lo agrega a jobsActivos
 * en base a un paquete de bloques
 */
t_job* crearNuevoJob(int idMaster, t_list* bloques, char* algoritmo) {
	t_list* nodos;
	t_list* bloquesReducidos;
	t_job* nuevoJob;
	// ---------------------------- CREO JOB ----------------------------------------------

	/** insertar mutex **/
	nodos = obtenerNodosParticipantes(bloques);
	bloquesReducidos = reducirBloques(bloques);
	nuevoJob = crearJob(bloquesReducidos, nodos, algoritmo, idMaster);

	return nuevoJob;

}

void recibirMensajeFS(void *args){
	t_esperar_mensaje *argumentos = (t_esperar_mensaje*) args;
	int nuevoSocket = argumentos->socketCliente;
	free(args);


	t_list* bloques = list_create();

	int idMaster;

	//recibo idMaster
	recibirInt(nuevoSocket, &idMaster);

	while(1){
		int messageCode = 0;
		recibirInt(nuevoSocket, &messageCode);
		switch(messageCode){
		case PROCESAR_BLOQUE: ;

			Package* package = createPackage();
			recieve_and_deserialize(package, nuevoSocket);
			t_bloque* bloqueRecibido = deserializar_bloque_serializado(package->message);
			procesarBloqueRecibido(bloques, bloqueRecibido);
			free(package);
			break;

		case ENVIO_BLOQUE_TERMINADO: ;	break;

		}

		if(messageCode == ENVIO_BLOQUE_TERMINADO) break;
	}



	/* declaro variables para el job y la solicitud de transformacion */
	t_job* nuevoJob;
	char* algoritmo = algoritmoBalanceo;

	//	solicitud_transformacion* solicitudTransformacion;
	char* solicitudTransfSerializado;
	uint32_t longitud;

	log_trace(logYamaImpreso, "se recibieron %d bloques de yamafs para el master %d", list_size(bloques), idMaster);

	nuevoJob = crearNuevoJob(idMaster, bloques, algoritmo);

	tablaDeEstados(nuevoJob);

	log_trace(logYamaImpreso, "               ---------job creado---------\n id de job: %d, id de master: %d, cantidad de nodos: %d, cantidad de bloques a transformar: %d", nuevoJob->idJob, nuevoJob->idMaster, list_size(nuevoJob->estadosReduccionesLocales), list_size(nuevoJob->estadosTransformaciones));

	// ------------------ ENVIO SOLICITUD TRANSFORMACION A MASTER -------------------------

	enviarInt(idMaster, nuevoJob->idJob);
	//	solicitudTransformacion = obtenerSolicitudTrasnformacion(nuevoJob);

	//	solicitudTransfSerializado = serializarSolicitudTransformacion(solicitudTransformacion);
	//	longitud = getLong_SolicitudTransformacion(solicitudTransformacion);

	//	log_trace(logYamaImpreso, "solicitud de transformacion terminada y serializada\n se enviaran %d bytes al master %d", longitud, idMaster);
	log_trace(logYamaImpreso, "se envian datos de transformacion a master %d", idMaster);

	int i;
	int tamanioTransformacion = list_size(nuevoJob->estadosTransformaciones);
	int enviados = 0;
	t_estado* unEstado;
	item_transformacion* item;
	for(i=0; i<tamanioTransformacion ;i++){
		unEstado = list_get(nuevoJob->estadosTransformaciones, i);
		item = crearItemTransformacion(unEstado->nodoPlanificado->nodo->idNodo, unEstado->nodoPlanificado->nodo->ipWorker, unEstado->nodoPlanificado->nodo->puerto, unEstado->nodoPlanificado->bloque->numero_bloque, unEstado->nodoPlanificado->bloque->bytes_ocupados, unEstado->archivoTemporal);
		uint32_t size_item_transformacion = getLong_one_item_transformacion(item);
		char* serialized_item_transformacion = serializar_item_transformacion(item);
		enviados += enviarMensajeSocketConLongitud(idMaster,ACCION_PROCESAR_TRANSFORMACION,serialized_item_transformacion,size_item_transformacion);
	}
	//	int enviados = enviarMensajeSocketConLongitud(idMaster,ACCION_PROCESAR_TRANSFORMACION,solicitudTransfSerializado,longitud);



	log_trace(logYamaImpreso, "bytes enviados %d", enviados);

	//	free(solicitudTransformacion->items_transformacion);
	//	free(solicitudTransfSerializado);
	//	free(solicitudTransformacion);
}

void recargarConfiguracion(int signal){

	log_trace(logYamaErrorImpreso, "codigo del error de errno %d", errno);
	log_trace(logYamaImpreso,"SIGUSR1: cargando nuevamente configuracion");

	cargarValoresPlanificacion();

}

int esperarConexionesSocketYama(fd_set *master, int socketEscucha) {
	//dado un set y un socket de escucha, verifica mediante select, si hay alguna conexion nueva para aceptar
	int nuevoSocket = -1;
	fd_set* readSet = malloc(sizeof(fd_set));
	FD_ZERO(readSet);
	*readSet = *(master);

	int retorno;



	fallo_por_signal:
	retorno = select(socketEscucha + 1, readSet, NULL, NULL, NULL);
	if (retorno == -1) {
		if(errno == EINTR){
			fd_set* readSet = malloc(sizeof(fd_set));
			FD_ZERO(readSet);
			log_trace(logYamaImpreso, "Signal SIGUSR1 recibido, el socket se reabrio, se vuelve a escuchar el puerto");
			goto fallo_por_signal;
		}
		log_trace(logYamaErrorImpreso, "select");
		exit(4);
	}
	if (FD_ISSET(socketEscucha, readSet)) {
		// handle new connections
		nuevoSocket = aceptarConexion(socketEscucha);
	}
	return nuevoSocket;
}


void esperarMensajeMaster(t_esperar_conexion* argumentos) {
	while (1) {

		int nuevoSocket = -1;
		nuevoSocket = esperarConexionesSocketYama(&argumentos->fdSocketEscucha,
				argumentos->socketEscucha);

		if (nuevoSocket != -1) {

			//log_trace(logSockets,"Nuevo Socket!");
			log_trace(logYamaImpreso,"Nueva Conexion Recibida - Socket N°: %d", nuevoSocket);
			int cliente;
			recibirInt(nuevoSocket, &cliente);

			t_esperar_mensaje *tEsperarMensaje = malloc(sizeof(t_esperar_mensaje));
			tEsperarMensaje->socketCliente = nuevoSocket;

			/* define el thread */
			pthread_t threadSolicitudes;

			int er1;

			switch (cliente) {
			case PROCESO_MASTER:
				er1 = pthread_create(&threadSolicitudes, NULL, recibirMensajeMaster, (void*) tEsperarMensaje);
				break;
			}
		}
	}
}

void esperarConexionMasterYFS(void *args) {

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------
	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;

	log_trace(logYamaImpreso, "Esperando conexiones en Yama...");

	esperarMensajeMaster(argumentos);
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
	solicitud->items_transformacion = calloc(tamanioTransformacion, sizeof(item_transformacion));
	solicitud->item_cantidad = 0;
	item_transformacion* item = malloc(sizeof(item_transformacion));

	for(i=0; i<tamanioTransformacion ;i++){
		unEstado = list_get(job->estadosTransformaciones, i);
		item = crearItemTransformacion(unEstado->nodoPlanificado->nodo->idNodo, unEstado->nodoPlanificado->nodo->ipWorker, unEstado->nodoPlanificado->nodo->puerto, unEstado->nodoPlanificado->bloque->numero_bloque, unEstado->nodoPlanificado->bloque->bytes_ocupados, unEstado->archivoTemporal);
		agregarItemTransformacion(solicitud, item);
	}
	return solicitud;
}

item_reduccion_local* obtenerSolicitudReduccionLocal(t_job* job, char idNodo[NOMBRE_NODO]){

	int i;
	int j;
	int cantTransformaciones;
	t_estado* unEstado;
	archivo_temp* listaRutasTransformacion;
	int tamanioTransformacion = list_size(job->estadosTransformaciones);
	int tamanioReduccionLocal = list_size(job->estadosReduccionesLocales);

	item_reduccion_local* item = malloc(sizeof(item_reduccion_local));

	cantTransformaciones = cantidadTransformaciones(idNodo, job->estadosTransformaciones);
	listaRutasTransformacion = malloc(sizeof(archivo_temp) * cantTransformaciones);
	int indice = 0;
	for(j=0; j < tamanioTransformacion; j++){
		unEstado = list_get(job->estadosTransformaciones, j);
		if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idNodo) == 0 && indice<cantTransformaciones){
			strncpy(listaRutasTransformacion[indice].archivo_temp, unEstado->archivoTemporal, LENGTH_RUTA_ARCHIVO_TEMP);
			indice++;
		}
	}

	for(i=0; i<tamanioReduccionLocal ;i++){
		unEstado = list_get(job->estadosReduccionesLocales, i);
		if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idNodo) == 0){
			break;
		}
	}

	item = crearItemReduccionLocal(unEstado->nodoPlanificado->nodo->idNodo,
			unEstado->nodoPlanificado->nodo->ipWorker,
			unEstado->nodoPlanificado->nodo->puerto,
			unEstado->archivoTemporal);
	item->archivos_temporales_transformacion = listaRutasTransformacion;
	item->cantidad_archivos_temp = cantTransformaciones;
	return item;
}

int cantidadTransformaciones(char* idNodo, t_list* estados){
	int j, k = 0;
	int tamanioTransformacion = list_size(estados);
	t_estado* unEstado;
	for(j=0; j < tamanioTransformacion; j++){
		unEstado = list_get(estados, j);
		if(strcmp(unEstado->nodoPlanificado->nodo->idNodo, idNodo) == 0){
			k++;
		}
	}
	return k;
}

solicitud_reduccion_global* obtenerSolicitudReduccionGlobal(t_job* job){ //t_list* planificacion, int puerto_worker, char* ip_worker, t_list* rutasRedLocalTemporales, char* rutaReduccionGlobal){

	int i;
	t_estado* unEstado;
	t_estado* estadoGlobal;
	t_worker* item;
	int tamanioReduccionLocal = list_size(job->estadosReduccionesLocales);

	solicitud_reduccion_global* solicitud = malloc(sizeof(solicitud_reduccion_global));
	solicitud->item_cantidad = 0;
	solicitud->workers = malloc(sizeof(t_worker)*tamanioReduccionLocal);

	for(i=0; i<tamanioReduccionLocal ;i++){
		unEstado = list_get(job->estadosReduccionesLocales, i);
		item = crearItemWorker(unEstado->nodoPlanificado->nodo->idNodo,
				unEstado->nodoPlanificado->nodo->ipWorker,
				unEstado->nodoPlanificado->nodo->puerto,
				unEstado->archivoTemporal);
		agregarItemWorker(solicitud, item);
	}
	estadoGlobal = job->reduccionGlobal;
	item = crearItemWorker(estadoGlobal->nodoPlanificado->nodo->idNodo,
			estadoGlobal->nodoPlanificado->nodo->ipWorker,
			estadoGlobal->nodoPlanificado->nodo->puerto,
			estadoGlobal->archivoTemporal);
	solicitud->encargado_worker = item;
	strcpy(solicitud->archivo_temporal_reduccion_global, estadoGlobal->archivoTemporal);
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
	 * (i) 	deserealizar el resultado de transformacion
	 * (ii)	evaluar si el resultado es ok o error, si es error, replanificar
	 * (iii)actualizar tabla de estados
	 * (iv)	si esta ok, evaluar si ya se terminaron todos los bloques
	 * (v)	si ya se terminaron crear dicha reduccion local para el nodo
	 */

	/* (i) */
	int offset = 0;
	char idNodo[NOMBRE_NODO];
	uint32_t numero_bloque;

	deserializarDato(&numero_bloque, package->message, sizeof(uint32_t), &offset);
	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);

	/* (iii) */
	actualizarEstado(idNodo, numero_bloque, RESULTADO_TRANSFORMACION, idJob, resultado);

	/* (ii) */
	if(resultado == TRANSFORMACION_OK){

		/* (iv) */
		if(finalizoTransformacionesNodo(idNodo, numero_bloque, idJob)){

			/* (v) */
			t_job* job = obtenerJob(idJob, jobsActivos);
			enProcesoReduccionLocal(idNodo, job);
			tablaDeEstados(job);

			item_reduccion_local* itemReduccion = obtenerSolicitudReduccionLocal(job, idNodo);
			char* solicitudSerializado = serializar_item_reduccion_local(itemReduccion);
			uint32_t longitud = getLong_one_item_reduccion_local(itemReduccion);
			enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_REDUCCION_LOCAL,solicitudSerializado,longitud);

			free(itemReduccion->archivos_temporales_transformacion);
			free(solicitudSerializado);
			free(itemReduccion);
		}
	} else{



		/***** termino el job *****/
		t_job* jobFallado;
		jobFallado = terminarJob(idJob);
		tablaDeEstados(jobFallado);

		/********* JOB *************/
		t_list* bloques;
		bloques = obtenerBloques(jobFallado);
		t_job* jobReplanificado;

		desconectarNodo(idNodo);

		if(sePuedePlanificar(bloques)){

			solicitud_transformacion* solicitudTransformacion;
			char* solicitudTransfSerializado;
			uint32_t longitud;

			jobReplanificado = crearNuevoJob(nuevoSocket, bloques, algoritmoBalanceo);

			/**** fallo uno de los nodos ***/
			log_trace(logYamaErrorImpreso, "fallo el job %d en el nodo %d, notificando replanificacion a master, id de job replanificado: %d", idJob, idNodo, jobReplanificado->idJob);
			char* idJobSerializado = malloc(sizeof(uint32_t));
			int offset = 0;
			serializarDato(idJobSerializado, &(jobReplanificado->idJob), sizeof(uint32_t), &offset);
			enviarMensajeSocketConLongitud(nuevoSocket, ACCION_REPLANIFICACION, idJobSerializado, sizeof(uint32_t));

			solicitudTransformacion = obtenerSolicitudTrasnformacion(jobReplanificado);

			solicitudTransfSerializado = serializarSolicitudTransformacion(solicitudTransformacion);
			longitud = getLong_SolicitudTransformacion(solicitudTransformacion);

			enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_TRANSFORMACION,solicitudTransfSerializado,longitud);

			free(solicitudTransformacion->items_transformacion);
			free(solicitudTransformacion);
			free(solicitudTransfSerializado);

		}else{
			log_trace(logYamaErrorImpreso, "el job %d no se puede replanificar, job finalizado", idJob);
			char* jobFinalizado = malloc(sizeof(uint32_t));
			int offset = 0;
			int terminoJob = ACCION_TERMINAR_JOB;
			serializarDato(jobFinalizado, &terminoJob, sizeof(uint32_t), &offset);
			enviarMensajeSocketConLongitud(nuevoSocket, ACCION_TERMINAR_JOB, jobFinalizado, sizeof(uint32_t));

		}
	}
}

void procesarResultadoReduccionLocal(int nuevoSocket, Package* package, uint32_t resultado){

	int offset = 0;
	char idNodo[NOMBRE_NODO];

	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);
	actualizarEstado(idNodo, NULL, RESULTADO_REDUCCION_LOCAL, idJob, resultado);

	t_job* job = obtenerJob(idJob, jobsActivos);

	if(resultado == REDUCCION_LOCAL_OK){
		if(finalizaronReduccionesLocalesNodos(job)){

			enProcesoReduccionGlobal(job);

			tablaDeEstados(job);

			solicitud_reduccion_global* solicitudReduccionGlobal = obtenerSolicitudReduccionGlobal(job);
			log_trace(logYamaImpreso, "ruta temporal reduccion global %s\n", solicitudReduccionGlobal->archivo_temporal_reduccion_global);

			char* solicitudSerializado = serializarSolicitudReduccionGlobal(solicitudReduccionGlobal);
			uint32_t longitud = getLong_SolicitudReduccionGlobal(solicitudReduccionGlobal);
			enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_REDUCCION_GLOBAL,solicitudSerializado,longitud);

			free(solicitudReduccionGlobal);
			free(solicitudSerializado);
		}
	}else{
		log_trace(logYamaErrorImpreso, "fallo el job %d en la reduccion local, terminando job", idJob);
		job = terminarJob(idJob);
		char* jobFinalizado = malloc(sizeof(uint32_t));
		int offset = 0;
		int terminoJob = ACCION_TERMINAR_JOB;
		serializarDato(jobFinalizado, &terminoJob, sizeof(uint32_t), &offset);
		enviarMensajeSocketConLongitud(nuevoSocket, ACCION_TERMINAR_JOB, jobFinalizado, sizeof(uint32_t));

	}



}

void procesarResultadoReduccionGlobal(int nuevoSocket, Package* package, uint32_t resultado){

	int offset = 0;
	char idNodo[NOMBRE_NODO];
	uint32_t numero_bloque;

	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);
	actualizarEstado(idNodo, NULL, RESULTADO_REDUCCION_GLOBAL, idJob, resultado);

	t_job* job = obtenerJob(idJob, jobsActivos);

	if(resultado == REDUCCION_GLOBAL_OK){

		enProcesoAlmacenadoFinal(job);
		tablaDeEstados(job);

		solicitud_almacenado_final* solicitudAlmacenadoFinal = obtenerSolicitudAlmacenadoFinal(job);
		char* solicitudSerializado = serializarSolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
		uint32_t longitud = getLong_SolicitudAlmacenadoFinal(solicitudAlmacenadoFinal);
		int enviados = enviarMensajeSocketConLongitud(nuevoSocket,ACCION_PROCESAR_ALMACENADO_FINAL,solicitudSerializado,longitud);
	}else{

		log_trace(logYamaErrorImpreso, "fallo el job %d en la reduccion global, terminando job", idJob);
		job = terminarJob(idJob);
		tablaDeEstados(job);
		char* jobFinalizado = malloc(sizeof(uint32_t));
		int offset = 0;
		int terminoJob = ACCION_TERMINAR_JOB;
		serializarDato(jobFinalizado, &terminoJob, sizeof(uint32_t), &offset);
		enviarMensajeSocketConLongitud(nuevoSocket, ACCION_TERMINAR_JOB, jobFinalizado, sizeof(uint32_t));
	}

}

void procesarResultadoAlmacenadoFinal(int nuevoSocket, Package* package, uint32_t resultado){

	int offset = 0;
	char idNodo[NOMBRE_NODO];

	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);
	actualizarEstado(idNodo, NULL, RESULTADO_ALMACENADO_FINAL, idJob, resultado);


	if(resultado == ALMACENADO_FINAL_OK){
		log_trace(logYamaErrorImpreso, "el job %d se ejecuto con exito, terminando job", idJob);
	}else{
		log_trace(logYamaErrorImpreso, "fallo el job %d en almacenado final, terminando job", idJob);
	}


	//todo insertar mutex
	t_job* job = terminarJob(idJob);
	tablaDeEstados(job);
	char* jobFinalizado = malloc(sizeof(uint32_t));
	offset = 0;
	int terminoJob = ACCION_TERMINAR_JOB;
	serializarDato(jobFinalizado, &terminoJob, sizeof(uint32_t), &offset);
	enviarMensajeSocketConLongitud(nuevoSocket, ACCION_TERMINAR_JOB, jobFinalizado, sizeof(uint32_t));
}

void procesarSolicitudArchivoMaster(int nuevoSocket, Package* package){
	char* solicitudArchivo = malloc(package->message_long);

	strcpy(solicitudArchivo, package->message);

	log_trace(logYamaImpreso, "archivo sobre el cual realizar el job %s", solicitudArchivo);

	enviarInt(socketFS, nuevoSocket);
	enviarMensaje(socketFS, solicitudArchivo);

	free(solicitudArchivo);

	t_esperar_mensaje* tEsperarMensaje = malloc(sizeof(t_esperar_mensaje));
	tEsperarMensaje->socketCliente = socketFS;
	recibirMensajeFS(tEsperarMensaje);

}

void datoBloque(void* elemento){
	t_bloque* unBloque = (t_bloque*) elemento;
	log_trace(logYamaImpreso, "numeroBytes: %d, idBloque: %d, idNodo: %s, ip: %s, numeroBloque: %d, puerto: %d", unBloque->bytes_ocupados, unBloque->idBloque, unBloque->idNodo, unBloque->ip, unBloque->numero_bloque, unBloque->puerto);
}

//se obtiene ademas el id del master
void procesarBloqueRecibido(t_list* bloques, t_bloque* unBloque){

	list_add(bloques, unBloque);

	datoBloque((void*) unBloque);

}

/*
 * pasa los bloques de bloquesRecibidos a bloques
 * con formatos t_bloque*
 */
void adaptarBloques(t_bloques_enviados* bloquesRecibidos, t_list* bloques){
	int i;

	for(i=0; i<bloquesRecibidos->cantidad_bloques; i++){
		list_add(bloques, &(bloquesRecibidos->lista_bloques[i]));
	}
}

bool finalizoTransformacionesNodo(char* idNodo, int numero_bloque, int idJob){
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

	system("export LC_ALL=C");

	log_trace(logYama, "Cargando configuracion de Yama");

	t_config* infoConfig = config_create("config.txt");

	if(config_has_property(infoConfig,"IP_FILESYSTEM")){
		fsIP = config_get_string_value(infoConfig,"IP_FILESYSTEM");
		log_trace(logYamaImpreso, "IP del filesystem: %s", fsIP);
	}

	if(config_has_property(infoConfig,"PUERTO_FILESYSTEM")){
		fsPort = config_get_int_value(infoConfig,"PUERTO_FILESYSTEM");
		log_trace(logYamaImpreso, "Puerto del filesystem: %d", fsPort);
	}

	cargarValoresPlanificacion();

	log_trace(logYama, "Carga exitosa del archivo de configuracion");


}

void cargarValoresPlanificacion(){

	t_config* infoConfig = config_create("config.txt");

	if(config_has_property(infoConfig,"RETARDO_PLANIFICACION")){
		retardoPlanificacion = config_get_int_value(infoConfig,"RETARDO_PLANIFICACION");
		log_trace(logYamaImpreso,"Tiempo de retardo (en milisegundos: %d", retardoPlanificacion);
	}
	if(config_has_property(infoConfig,"ALGORITMO_BALANCEO")){
		algoritmoBalanceo = config_get_string_value(infoConfig,"ALGORITMO_BALANCEO");
		algoritmoBalanceo[strlen(algoritmoBalanceo)+1]='\0';
		log_trace(logYamaImpreso,"Algoritmo de balanceo seleccionado: %s", algoritmoBalanceo);
	}

	if(strcmp(algoritmoBalanceo, "CLOCK") != 0 && strcmp(algoritmoBalanceo, "W-CLOCK") != 0){
		log_trace(logYamaErrorImpreso, "algoritmo de balanceo incorrecto. Escribir CLOCK o W-CLOCK");
		exit(1);
	}

	if(config_has_property(infoConfig,"DISP_BASE")){
		dispBase = config_get_int_value(infoConfig,"DISP_BASE");
		log_trace(logYamaImpreso, "Disponibilidad Base: %d", dispBase);
	}


}




