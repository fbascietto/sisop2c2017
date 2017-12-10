

#include <pthread.h>
#include "funcionesyama.h"
#include "prePlanificacion.h"
#include "../bibliotecas/protocolo.h"
#include "../bibliotecas/estructuras.h"
#include "interfaceMaster.h"
#include "../bibliotecas/sockets.h"
#include <errno.h>


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
 * crea el job y guarda en jobGlobal
 * y lo agrega a jobsActivos
 * en base a un paquete de bloques
 */
void crearNuevoJob(int idMaster, t_list* bloques, char* algoritmo) {
	t_list* nodos;
	t_list* bloquesReducidos;
	t_job* nuevoJob = malloc(sizeof(t_job));
	// ---------------------------- CREO JOB ----------------------------------------------

	/** insertar mutex **/
	nodos = obtenerNodosParticipantes(bloques);
	bloquesReducidos = reducirBloques(bloques);
	crearJob(bloquesReducidos, nodos, algoritmo, idMaster);

	//todo
	//loguearEstadoJob(nuevoJob, RECIBIR_BLOQUES);
}

void recibirMensajeFS(void *args){
	t_esperar_mensaje *argumentos = (t_esperar_mensaje*) args;
	int nuevoSocket = argumentos->socketCliente;
	free(args);

	Package* package = createPackage();
	int leidos = recieve_and_deserialize(package, nuevoSocket);

	uint32_t idMaster;

	printf("codigo de mensaje: %d\n",	package->msgCode);


	/* declaro variables para el job y la solicitud de transformacion */
	t_job* nuevoJob;
	char* algoritmo = algoritmoBalanceo;


	solicitud_transformacion* solicitudTransformacion;
	char* solicitudTransfSerializado;
	uint32_t longitud;
	int enviados;
	t_list* bloques;


	/* proceso los bloques */
	switch(package->msgCode){
	case RECIBIR_BLOQUES:

		bloques = procesarBloquesRecibidos(package->message, &idMaster);
		crearNuevoJob(idMaster, bloques, algoritmo);

		nuevoJob = jobGlobal;
		/** fin mutex **/

		printf("\n\nTERMINE EL JOB LA CONCHA DE TU MADRE, PODES APROBARME EL TP?\n\n");

		printf("\nDATOS TRANSFORMACION\n");
		list_iterate(nuevoJob->estadosTransformaciones, estadisticas);
		printf("\nDATOS REDUCCIONLOCAL\n");
		list_iterate(nuevoJob->estadosReduccionesLocales, estadisticas);
		printf("id de Job: %d\n", nuevoJob->idJob);
		printf("serializando transformaciones\n");
		// ------------------ ENVIO SOLICITUD TRANSFORMACION A MASTER -------------------------

		solicitudTransformacion = obtenerSolicitudTrasnformacion(nuevoJob);

		printf("solicitud de transformacion terminada\n");

		solicitudTransfSerializado = serializarSolicitudTransformacion(solicitudTransformacion);
		longitud = getLong_SolicitudTransformacion(solicitudTransformacion);

		printf("solicitud de transformacion serializada\n se enviaran %d bytes a master\n", longitud);


		enviados = enviarMensajeSocketConLongitud(idMaster,ACCION_PROCESAR_TRANSFORMACION,solicitudTransfSerializado,longitud);
	}

	//todo hacer free
}

void recargarConfiguracion(int signal){
	printf("SIGUSR1: cargando nuevamente configuracion\n");
	cargarValoresPlanificacion();
	//	printf("%d\n", valorSocket);
	//	if(valorSocket < 0){
	//
//	close(esperarConexion->socketEscucha);
	printf("codigo del error %d\n", errno);
	printf("el error deberia ser %d\n", EINTR);
}

int esperarConexionesSocketYama(fd_set *master, int socketEscucha) {
	//dado un set y un socket de escucha, verifica mediante select, si hay alguna conexion nueva para aceptar
	int nuevoSocket = -1;
	fd_set* readSet = malloc(sizeof(fd_set));
	FD_ZERO(readSet);
	*readSet = *(master);
	struct sigaction sa;
	sa.sa_handler = recargarConfiguracion;
	if (sigaction(SIGUSR1,&sa,0) < 0) // Setup signal
		perror("sigaction failed");

	int retorno;
	probando_annotation:
	retorno = select(socketEscucha + 1, readSet, NULL, NULL, NULL);
	if (retorno == -1) {
		if(errno == EINTR){
			fd_set* readSet = malloc(sizeof(fd_set));
			FD_ZERO(readSet);
//			int socket = escuchar(5100);
//			socketEscucha = socket;
			//
			//			if (sigfillset(&sa.sa_mask) < 0)
			//				perror("sigfillset failed");
			//			sa.sa_flags=0;
			printf("Signal SIGUSR1 recibido, el socket se reabrio, se vuelve a escuchar el puerto\n");
			goto probando_annotation;
		}
		perror("select");
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

		//TODO: Recibir instrucciones master y crear thread por cada una
		int nuevoSocket = -1;
		nuevoSocket = esperarConexionesSocketYama(&argumentos->fdSocketEscucha,
				argumentos->socketEscucha);

		if (nuevoSocket != -1) {

			//log_trace(logSockets,"Nuevo Socket!");
			printf("Nueva Conexion Recibida - Socket N°: %d\n", nuevoSocket);
			int cliente;
			recibirInt(nuevoSocket, &cliente);

			/* define el thread */
			pthread_t threadSolicitudes;
			t_esperar_mensaje *tEsperarMensaje = malloc(
					sizeof(t_esperar_mensaje));
			tEsperarMensaje->socketCliente = nuevoSocket;
			int er1;

			switch (cliente) {
			//TODO: iniciar un hilo para manejar cliente
			case PROCESO_MASTER:
				er1 = pthread_create(&threadSolicitudes, NULL,
						recibirMensajeMaster, (void*) tEsperarMensaje);
				pthread_join(threadSolicitudes, NULL);
				break;
			}
		}
		sleep(1);
	}
}

void esperarConexionMasterYFS(void *args) {

	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------


	/*int nuevoSocket;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,
				argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);

		}*/



	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;

	printf("Esperando conexiones en Yama...\n");



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
	solicitud->items_transformacion = malloc(sizeof(item_transformacion)*tamanioTransformacion);
	item_transformacion* item = malloc(sizeof(item_transformacion));

	for(i=0; i<tamanioTransformacion ;i++){
		unEstado = list_get(job->estadosTransformaciones, i);
		item = crearItemTransformacion(unEstado->nodoPlanificado->nodo->idNodo, unEstado->nodoPlanificado->nodo->ipWorker, unEstado->nodoPlanificado->nodo->puerto, unEstado->nodoPlanificado->bloque->numero_bloque, unEstado->nodoPlanificado->bloque->bytes_ocupados, unEstado->archivoTemporal);
		agregarItemTransformacion(solicitud, item);
	}
	return solicitud;
	//return obtenerSolicitudTrasnformacionMock(message);
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
	//return obtenerSolicitudReduccionLocalMock(message);
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
	uint32_t numero_bloque;

	deserializarDato(&numero_bloque, package->message, sizeof(uint32_t), &offset);
	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);

	/* (iii) */
	actualizarEstado(idNodo, numero_bloque, RESULTADO_TRANSFORMACION, idJob, resultado);

	printf("recibi el codigo: %d de la transformacion del bloque %d del nodo %s del job %d",
			resultado,
			numero_bloque,
			idNodo,
			idJob);

	/* (ii) */
	if(resultado == TRANSFORMACION_OK){


		/* (iv) */
		if(finalizoTransformacionesNodo(idNodo, numero_bloque, idJob)){

			/* (v) */

			t_job* job = obtenerJob(idJob, jobsActivos);
			item_reduccion_local* itemReduccion = obtenerSolicitudReduccionLocal(job, idNodo);
			printf("----------------\n");
			printf("La ruta del archivo resultante de reduccion local es: %s\n", itemReduccion->archivo_temporal_reduccion_local);
			int aux;
			for(aux=0; aux<itemReduccion->cantidad_archivos_temp; aux++){
				printf(
						"La ruta del archivo temporal %d de transformacion es: %s\n",
						aux,
						itemReduccion->archivos_temporales_transformacion[aux].archivo_temp
				);
			}
			printf("La cantidad de archivos temporales de transformacion a reducir son: %d\n", itemReduccion->cantidad_archivos_temp);
			printf("El nombre del nodo es: %s\n", itemReduccion->nodo_id);
			printf("La ip del worker es: %s\n", itemReduccion->ip_worker);
			printf("El puerto del worker es: %d\n", itemReduccion->puerto_worker);
			printf("----------------\n");
			char* solicitudSerializado = serializar_item_reduccion_local(itemReduccion);
			uint32_t longitud = getLong_one_item_reduccion_local(itemReduccion);
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

			/** insertar mutex **/
			crearNuevoJob(idMaster, bloques, algoritmoBalanceo);

			jobReplanificado = jobGlobal;
			/** insertar mutex **/

			solicitudTransformacion = obtenerSolicitudTrasnformacion(jobReplanificado);

			solicitudTransfSerializado = serializarSolicitudTransformacion(solicitudTransformacion);
			longitud = getLong_SolicitudTransformacion(solicitudTransformacion);

			enviados = enviarMensajeSocketConLongitud(idMaster,ACCION_PROCESAR_TRANSFORMACION,solicitudTransfSerializado,longitud);

			free(solicitudTransformacion);
			free(solicitudTransfSerializado);
		}else{
			t_job* job = obtenerJob(idJob, jobsFinalizados);
			terminarJob(job);

		}
	}
}

void procesarResultadoReduccionLocal(int nuevoSocket, Package* package, uint32_t resultado){

	int offset = 0;
	char idNodo[NOMBRE_NODO];
	uint32_t numero_bloque;

	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);
	actualizarEstado(idNodo, NULL, RESULTADO_REDUCCION_LOCAL, idJob, resultado);

	t_job* job = obtenerJob(idJob, jobsActivos);

	if(resultado == REDUCCION_LOCAL_OK){
		if(finalizaronReduccionesLocalesNodos(job)){

			printf("terminaron las reducciones locales\n");

			solicitud_reduccion_global* solicitudReduccionGlobal = obtenerSolicitudReduccionGlobal(job);
			char* solicitudSerializado = serializarSolicitudReduccionGlobal(solicitudReduccionGlobal);

			printf("ruta final reduccion global %s\n", solicitudReduccionGlobal->archivo_temporal_reduccion_global);

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
	uint32_t numero_bloque;

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
	uint32_t numero_bloque;

	deserializarDato(idNodo, package->message, NOMBRE_NODO, &offset);

	int idJob;

	idJob = obtenerIdJob(nuevoSocket, jobsActivos);
	actualizarEstado(idNodo, NULL, RESULTADO_REDUCCION_GLOBAL, idJob, resultado);

	terminarJob(idJob);
}

void procesarSolicitudArchivoMaster(int nuevoSocket, Package* package){
	//solicitud_transformacion* solicitudTransformacion = obtenerSolicitudTrasnformacion(message);
	char* solicitudArchivo = malloc(package->message_long);
	uint32_t* tamanioSerializado = malloc(sizeof(uint32_t));

	strcpy(solicitudArchivo, package->message);

	enviarInt(socketFS, nuevoSocket);
	enviarMensaje(socketFS, solicitudArchivo);

	free(solicitudArchivo);
	free(package);
	free(tamanioSerializado);

	t_esperar_mensaje* tEsperarMensaje = malloc(sizeof(t_esperar_mensaje));
	tEsperarMensaje->socketCliente = socketFS;
	recibirMensajeFS(tEsperarMensaje);

}

void datosBloques(void* elemento){
	t_bloque* bloque = (t_bloque*) elemento;
	int i = rutaGlobal++;
	printf("------------------------\n");
	printf("Los bytes ocupados de %d son: %d\n",i, bloque->bytes_ocupados);
	printf("El id de bloque de %d es: %d\n",i, bloque->idBloque);
	printf("El numero de bloque %d son: %d\n",i,bloque->numero_bloque);
	printf("El nombre de nodo de %d es: %s\n",i,bloque->idNodo);
	printf("El ip de %d es: %s\n",i,bloque->ip);
	printf("El puerto de %d es: %d\n\n",i,bloque->puerto);
	printf("------------------------\n");
}

//se obtiene ademas el id del master (va primero en la serializacion ((todo))
t_list* procesarBloquesRecibidos(char* message, uint32_t* masterId){

	//todo
	t_bloques_enviados* bloquesRecibidos = deserializar_bloques_enviados(message, masterId);

	t_list* bloques = list_create();

	adaptarBloques(bloquesRecibidos, bloques);

	list_iterate(bloques, datosBloques);
	rutaGlobal = 0;

	return bloques;
}

/*
 * pasa los bloques de bloquesRecibidos a bloques
 * con formatos t_bloque*
 */
void adaptarBloques(t_bloques_enviados* bloquesRecibidos, t_list* bloques){

	int i;

	for(i=0; i<bloquesRecibidos->cantidad_bloques; i++){

		printf("posicion de memoria: %p", &(bloquesRecibidos->lista_bloques[i]));
		list_add(bloques, &(bloquesRecibidos->lista_bloques[i]));

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
void actualizarEstado(char idNodo[NOMBRE_NODO], int numero_bloque, int etapa, int idJob, int resultado){
	t_job* job;
	t_estado* estado;

	job = obtenerJob(idJob, jobsActivos);

	switch(etapa){
	case RESULTADO_TRANSFORMACION:
		estado = obtenerEstadoTransformacion(job->estadosTransformaciones, idNodo, numero_bloque);
		if(resultado == TRANSFORMACION_OK){
			strcpy(estado->estado, "finalizado");
		}else{
			strcpy(estado->estado, "error");
		}
		break;

	case RESULTADO_REDUCCION_LOCAL:
		estado = obtenerEstadoRedLoc(job->estadosReduccionesLocales, idNodo);
		if(resultado == REDUCCION_LOCAL_OK){
			strcpy(estado->estado, "finalizado");
		}else{
			strcpy(estado->estado, "error");
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
	t_log_level level = LOG_LEVEL_TRACE;
	t_log_level level_ERROR = LOG_LEVEL_ERROR;
	t_log* yama_log = log_create("logYama.txt", "YAMA", 0, level);

	log_trace(yama_log, "Inicializacion de la configuracion de Yama");

	t_config* infoConfig = config_create("config.txt");

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

	t_config* infoConfig = config_create("config.txt");

	if(config_has_property(infoConfig,"RETARDO_PLANIFICACION")){
		retardoPlanificacion = config_get_int_value(infoConfig,"RETARDO_PLANIFICACION");
		printf("Tiempo de retardo (en milisegundos: %d\n", retardoPlanificacion);
	}
	if(config_has_property(infoConfig,"ALGORITMO_BALANCEO")){
		algoritmoBalanceo = config_get_string_value(infoConfig,"ALGORITMO_BALANCEO");
		algoritmoBalanceo[strlen(algoritmoBalanceo)+1]='\0';
		printf("Algoritmo de balanceo seleccionado: %s\n", algoritmoBalanceo);
	}

	if(strcmp(algoritmoBalanceo, "CLOCK") != 0 && strcmp(algoritmoBalanceo, "W-CLOCK") != 0){
		perror("algoritmo de balanceo incorrecto. Escribir CLOCK o W-CLOCK\n");
		exit(1);
	}

	if(config_has_property(infoConfig,"DISP_BASE")){
		dispBase = config_get_int_value(infoConfig,"DISP_BASE");
		printf("Disponibilidad Base: %d\n", dispBase);
	}

	log_destroy(yama_log);

}




