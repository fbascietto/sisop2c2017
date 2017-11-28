
#include "funcionesfs.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "../bibliotecas/estructuras.h"
#include "../bibliotecas/serializacion.h"
#include "../bibliotecas/serializacion.c"


#define ANSI_COLOR_CYAN		"\x1b[36m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_RESET	"\x1b[0m"
#define ANSI_COLOR_BOLD		"\x1b[1m"
#define ANSI_COLOR_RED		"\x1b[31m"

void formatFs(){

	/* Elimino la metadata */
	system("rm -r metadata");
	/* Elimino archivos.dat*/
	system("rm -f archivos.dat");
	/*creo carpeta metadata*/
	int status = mkdir("metadata", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	/*creo carpeta bitmap*/
	status = mkdir("metadata/bitmap", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	/*creo carpeta para archivos*/
	status = mkdir("metadata/archivos", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	/*Genero nuevo directorios.dat*/
	inicializarDirectorios();

	/*Si existe nodos.bin, actualizo la lista nodos*/

	if( access(nodos_file, F_OK) != -1 ) {
		list_clean(nodos);
		creoListaNodosDesdeNodosBin(nodos);
	}

	/*Actualizo los nodos, seteandolos todos como limpios*/
	int size = list_size(nodos);
	int i =0;

	char * descripcion;
	descripcion = malloc(sizeof(char[11])*size);
	int tamanio = 0;
	int libre = 0;

	for(;i<size;i++){
		t_nodo * nodo = list_get(nodos,i);
		nodo->bloquesLibres = nodo->tamanio/(1024*1024);
		t_bitarray* t_fs_bitmap;
		t_fs_bitmap = crearBitmapVacio(nodo->tamanio);
		escribirBitMap(nodo->tamanio, nodo->nombre_nodo, t_fs_bitmap);
		destruir_bitmap(t_fs_bitmap);
		tamanio = tamanio + nodo->tamanio;
		libre = libre + nodo->bloquesLibres;

		if(i>0){
			strcat(descripcion,",");
			strcat(descripcion,nodo->nombre_nodo);
		}else{
			strcpy(descripcion,nodo->nombre_nodo);
		}
	}

	pthread_mutex_lock(&mx_nodobin);
	FILE* nodosbin = fopen(nodos_file,"wb+");
	fprintf(nodosbin, "TAMANIO=%d\n", tamanio/(1024*1024));
	fprintf(nodosbin, "LIBRE=%d\n", libre);
	fprintf(nodosbin, "Q_NODOS=%d\n", size);
	fprintf(nodosbin, "NODOS=[%s]\n",descripcion);

	for(i=0;i<size;i++){
		t_nodo * nodo = list_get(nodos,i);
		fprintf(nodosbin, "%sTotal=%d\n", nodo->nombre_nodo, nodo->tamanio/(1024*1024));
		fprintf(nodosbin, "%sLibre=%d\n", nodo->nombre_nodo, nodo->bloquesLibres);
	}

	fclose(nodosbin);
	pthread_mutex_unlock(&mx_nodobin);

	printf("Formato finalizado con éxito.\n");
	imprimeNodosBin();

}

void levantarNodos(int clean){

	if(clean>0){

		system("rm -r metadata");
		system("rm -f archivos.dat");
		/*creo carpeta metadata*/

		int status = mkdir("metadata", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		/*creo carpeta bitmap*/
		status = mkdir("metadata/bitmap", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		/*creo carpeta para archivos*/
		status = mkdir("metadata/archivos", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


		FILE* file = fopen(nodos_file, "wb+");
		fprintf(file,"TAMANIO=0\n");
		fprintf(file,"LIBRE=0\n");
		fprintf(file,"NODOS=[]\n");
		fclose(file);
	}

	if(clean==0){
		creoListaNodosDesdeNodosBin();
		if (list_size(nodos) > 0){
			printf("Filesystem");
			printf(ANSI_COLOR_BOLD ANSI_COLOR_RED " no estable" ANSI_COLOR_RESET);
			printf(". Conectados %d de %d nodos.\n", cantNodos, list_size(nodos));
		}
	}

	if (nodos_file == NULL){
		printf("No se pudo abrir el archivo nodos.bin.\n");
		exit(EXIT_FAILURE);
	}

}

void creoListaNodosDesdeNodosBin(){

	char ** parametros;
	char * line = string_new();
	size_t len = 0;

	FILE* nodosbin = fopen(nodos_file,"r");
	if (nodosbin == NULL){
			fprintf(stderr, "Fallo al abrir el archivo nodos.bin %s. Considere ejecutar fs --clean.\n", strerror(errno));
			exit(EXIT_FAILURE);
	}

	/* recorro nodos bin*/
	getline(&line, &len, nodosbin);// TAMANIO
	parametros = string_split(line,"=");
	int tamanio = atoi(parametros[1]);
	if(tamanio==0){
		free(parametros);
		free(line);
		return;
	}
	getline(&line, &len, nodosbin);// LIBRE

	getline(&line, &len, nodosbin);// QNODOS

	parametros = string_split(line,"=");
	int qNodos = atoi(parametros[1]);

	getline(&line, &len, nodosbin);// LISTA NODOS
	line[strlen(line)-1]=NULL;
	parametros = string_split(line,"=");

	char ** nombresNodos = string_get_string_as_array(parametros[1]);


	int i = 0;

	for(;i < qNodos;i++){
		t_nodo * nodo;
		nodo = malloc(sizeof(t_nodo));
		nodo->socket_nodo = -1;
		nodo->ip[0]='\0';
		nodo->puerto=-1;

		strcpy(nodo->nombre_nodo,nombresNodos[i]);

		getline(&line, &len, nodosbin);
		parametros = string_split(line,"=");
		nodo->tamanio = atoi(parametros[1]) * (1024*1024);

		getline(&line, &len, nodosbin);
		parametros = string_split(line,"=");
		nodo->bloquesLibres = atoi(parametros[1]);

		list_add(nodos, nodo);
	}

	free(parametros);
	free(nombresNodos);
	free(line);
}

t_nodo* getNodoPorNombre(char* nombre_nodo, t_list* listaABuscar){

	t_nodo* nodoBuscado;

	bool* buscaNodoPorNombre(void* parametro) {
	t_nodo* nodo = (t_nodo*) parametro;
	return (strcmp(nodo->nombre_nodo,nombre_nodo) == 0);
	}

	nodoBuscado = list_find(listaABuscar,buscaNodoPorNombre);

	return nodoBuscado;

}

t_list* getNodosMenosCargados(t_list* listaABuscar){

	t_list* nodosAUsar;

	bool* comparadorBloquesLibres(void* parametro, void* parametro2) {
	t_nodo* nodo = (t_nodo*) parametro;
	t_nodo* nodo2 = (t_nodo*) parametro2;
	return (nodo->bloquesLibres < nodo2->bloquesLibres);
	}

	list_sort(listaABuscar,comparadorBloquesLibres);

	nodosAUsar = list_take(listaABuscar,2);
	return nodosAUsar;

}

t_list* inicializarDirectorios(){

		t_list *folderList;

		FILE *fptr;
	    t_directory *folders;

	    int nuevo = 0;

	    fptr = fopen("./metadata/directorios.dat", "r");
	    if(!fptr) //archivo no existe, crear
	    {
	    	fptr = fopen("./metadata/directorios.dat", "wb+");
	    	nuevo = 1;
	        if (fptr == NULL)
	        	printf("Error al abrir directorios.dat\n");
	        	//exit(1);
	    }


	    if(nuevo){

    	fprintf(fptr,"%s%s%s", "Index|", "Directorio|", "Padre\n");
	    folders = malloc(sizeof(t_directory));
	    folderList = list_create();
	    folders->index = 0;
	    folders->nombre = "root";
	    folders->padre = -1;
	    fprintf(fptr,"%d%s%s%s%d%s",folders->index,"|",folders->nombre,"|",folders->padre,"\n");
	    list_add(folderList, folders);
	    char* ruta = string_new();
	    /*creo la carpeta para root*/
	    string_append(&ruta, "./metadata/archivos/");
	   	string_append(&ruta, string_itoa(folders->index));
	    mkdir(ruta, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	    } else {
	    //cargo directorios.dat
	    	char* linea[256];
	    	char** col;
	    	folderList = list_create();
	    	fgets(linea,sizeof(linea),fptr);
	    	while(!feof(fptr)){
	    		col = string_split(linea,"|");
	    		if(!strcmp(col[0],"Index")){
	    			//ignoro titulos (para qué los puse?!)
	    		} else {
    		    folders = malloc(sizeof(t_directory));
	    		folders->index = atoi(col[0]);
	    		folders->nombre = col[1];
	    		folders->padre = atoi(col[2]);
	    	    list_add(folderList, folders);
	    		}
	    		fgets(linea,sizeof(linea),fptr);
	    	}
	    }

	    fclose(fptr);
	    return folderList;
}

void actualizarDirectorioDat(t_list* folderList){

		FILE *fptr;
	    t_directory *folders;

	    fptr = fopen("./metadata/directorios.dat", "w");
	    if(!fptr) //archivo no existe, crear
	    {
	    	printf("Error al abrir directorios.dat\n");
	        return;
	    }

	    int i = 0;
		fprintf(fptr,"%s%s%s", "Index|", "Directorio|", "Padre\n");

	    for(;i < list_size(folderList);i++){
	    	folders = list_get(folderList, i);
			fprintf(fptr,"%d%s%s%s%d%s",folders->index,"|",folders->nombre,"|",folders->padre,"\n");
		}

	    fclose(fptr);

}

int buscoEnArchivosDat(char* ruta_metadata){

	FILE* file = fopen(archivos_file, "r+");
	int found = 0;
	if (file == NULL){
		printf("Error en el registro de archivos.dat.\n");
		return 0;
	}

	string_append(&ruta_metadata,"\n");
	string_trim(&ruta_metadata);
	char * line = NULL;
	size_t len = 0;
	getline(&line,&len,file);
	string_trim(&line);
	while(!feof(file)){
		if(string_equals_ignore_case(line,ruta_metadata)){
			found = 1;
		}
		getline(&line,&len,file);
		string_trim(&line);
	}
	return found;
}

void listarDirectorios(t_list* folderList, t_directory* carpetaActual){

		void* imprimoCarpetas(void* parametro){
				t_directory* carpeta = (t_directory*) parametro;
				printf(ANSI_COLOR_BOLD ANSI_COLOR_CYAN "%s " ANSI_COLOR_RESET, carpeta->nombre);
			}

		bool* carpetasNivelActual(void* parametro) {
			t_directory* dir = (t_directory*) parametro;
			return (dir->padre == carpetaActual->index);
		}

		t_list* listado;
		listado = list_filter(folderList, carpetasNivelActual);

		list_iterate(listado,imprimoCarpetas);
		printf("");

}

void ordenoDirectorios(t_list* folderList){
	bool ordenar(void* parametro1, void* parametro2) {
		t_directory* entrada1 = (t_directory*) parametro1;
		t_directory* entrada2 = (t_directory*) parametro2;

		return entrada1->index < entrada2->index;
	}
	list_sort(folderList,ordenar);
}

void crearDirectorio(t_list* folderList, t_directory* carpetaActual, char* nombre){

		if (list_size(folderList)>100){
			printf("No se pueden crear más de 100 directorios.\n");
			return;
		}

		bool* condicionCrearCarpeta(void* parametro) {
				t_directory* dir = (t_directory*) parametro;
				return ((dir->padre == carpetaActual->index) && (strcmp(dir->nombre,nombre) == 0));
			}

		t_list* chequeo = list_filter(folderList,condicionCrearCarpeta);;
		if(list_size(chequeo)>0){
			printf("Directorio ya existente, imposible crear.\n");
			return;
		}

		t_directory* carpeta;

		carpeta = malloc(sizeof(t_directory));
		carpeta->index = obtenerDirectorioFaltante(folderList);
		carpeta->nombre = nombre;
		carpeta->padre = carpetaActual->index;

		FILE* fptr = fopen("./metadata/directorios.dat", "a+");
	    fprintf(fptr,"%d%s%s%s%d%s",carpeta->index,"|",carpeta->nombre,"|",carpeta->padre,"\n");
		list_add(folderList, carpeta);
		/*creo carpeta para archivos*/
		char* ruta =string_new();
		string_append(&ruta, "./metadata/archivos/");
		string_append(&ruta, string_itoa(carpeta->index));
		mkdir(ruta, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		fclose(fptr);
}

int obtenerDirectorioFaltante(t_list* folderList){

		t_directory* carpeta;
	    int i, n;
	    i = 0;
	    n = 0;
	    for (; i < list_size(folderList); i++){
			carpeta = list_get(folderList, i);
			if(carpeta->index != n){
				return n;
			}
			n++;
	    }
	    return list_size(folderList);
}

t_directory * cambiarAdirectorio(char* nombre, t_directory* carpetaActual, t_list* folderList){

	bool* matcheaCarpetaConIndice(void* parametro) {
				t_directory* dir = (t_directory*) parametro;
				return dir->index == carpetaActual->padre;
			}

	t_directory* carpetaNueva;
	int comparacion = strcmp(nombre, "..");
	if(!comparacion){
		return list_find(folderList,matcheaCarpetaConIndice);

	} else {

		bool* carpetasConMismoNombre(void* parametro) {
			t_directory* dir = (t_directory*) parametro;
			return (strcmp(dir->nombre,nombre) == 0);
		}


		t_list* carpetas = list_filter(folderList,carpetasConMismoNombre);


		int encontrado = 0;
		int i = 0;

		carpetaNueva= list_get(carpetas,i);
		switch(list_size(carpetas)){
			case 0:
				if((strchr(nombre,'.') != NULL)){
				} else {printf("Directorio inexistente.\n");
				}
				return carpetaActual;
			break;
			/*case 1:
				return carpetaNueva;
				break;*/
			default:
				//recorro lista buscando carpeta con mismo padre
				while(!encontrado){
					if(carpetaActual->index == carpetaNueva->padre ){
						return carpetaNueva;
						encontrado = 1;
					} else if(++i > list_size(carpetas)){
						return carpetaActual;
					} else {
					carpetaNueva= list_get(carpetas,i);
					break;
					}
				}
				break;
		}
	/*} llave del else de la comparación ".." */
	}
}

t_directory * cambiarAdirectorioConChequeo(char* nombre, t_directory* carpetaActual, t_list* folderList){

	bool* matcheaCarpetaConIndice(void* parametro) {
				t_directory* dir = (t_directory*) parametro;
				return dir->index == carpetaActual->padre;
			}

	t_directory* carpetaNueva;
	int comparacion = strcmp(nombre, "..");
	if(!comparacion){
		return list_find(folderList,matcheaCarpetaConIndice);

	} else {

		bool* carpetasConMismoNombre(void* parametro) {
			t_directory* dir = (t_directory*) parametro;
			return (strcmp(dir->nombre,nombre) == 0);
		}


		t_list* carpetas = list_filter(folderList,carpetasConMismoNombre);


		int encontrado = 0;
		int i = 0;

		carpetaNueva= list_get(carpetas,i);
		switch(list_size(carpetas)){
			case 0:
				if((strchr(nombre,'.') != NULL)){
				} else {
				printf("Directorio inexistente.\n");
				carpetaActual->index = -2;
				}
				return carpetaActual;
			break;
			/*case 1:
				return carpetaNueva;
				break;*/
			default:
				//recorro lista buscando carpeta con mismo padre
				while(!encontrado){
					if(carpetaActual->index == carpetaNueva->padre ){
						return carpetaNueva;
						encontrado = 1;
					} else if(++i > list_size(carpetas)){
						return carpetaActual;
					} else {
					carpetaNueva= list_get(carpetas,i);
					break;
					}
				}
				break;
		}
	/*} llave del else de la comparación ".." */
	}
}

t_nodo* getDirectorioPorNombre(char* carpeta, t_list* folderList){

	t_directory* carpetaBusc;

	bool* buscaNodoPorNombre(void* parametro) {
	t_directory* carpetaBusc = (t_directory*) parametro;
	return (strcmp(carpetaBusc->nombre,carpeta) == 0);
	}

	carpetaBusc = list_find(carpeta,buscaNodoPorNombre);

	return carpetaBusc;

}

int identificaDirectorio(char* directorio_yamafs, t_list* folderList){

	ordenoDirectorios(folderList);
	t_directory* carpetaActual= list_get(folderList,0); //arranco de root siempre?

	if(!string_starts_with(directorio_yamafs,"yamafs:")){
		printf("Los directorios y archivos del YamaFS se identifican con yamafs: al inicio\n");
		return -1;
	}

	int i = 0;
	char* ruta;
	ruta = replace_str(directorio_yamafs,"yamafs:","");
	char** arrayString = string_split(ruta,"/");
	while (arrayString[i]!= NULL){
		carpetaActual = cambiarAdirectorioConChequeo(arrayString[i],carpetaActual,folderList);
		i++;
	}

	if(strcmp(ruta,"/")){
	strcpy(directorio_yamafs,arrayString[i-1]);
	}
	//free(ruta);
	//free(arrayString);
	return carpetaActual->index;

}

int directorioVacio(char *dirname) {

  int n = 0;
  struct dirent *d;
  DIR *dir = opendir(dirname);
  if (dir == NULL) //Not a directory or doesn't exist
    return 1;
  while ((d = readdir(dir)) != NULL) {
    if(++n > 2)
      break;
  }
  closedir(dir);
  if (n <= 2) //Directory Empty
    return 1;
  else
    return 0;

}

void *esperarConexiones(void *args) {

	t_log_level logL;
	t_log* logSockets = log_create("log.txt","Yamafs",0,logL);
	t_esperar_conexion *argumentos = (t_esperar_conexion*) args;
	printf("Esperando conexiones...\n");



	// ---------------ME QUEDO ESPERANDO UNA CONEXION NUEVA--------------
	while (1) {

		int nuevoSocket = -1;

		nuevoSocket = esperarConexionesSocket(&argumentos->fdSocketEscucha,argumentos->socketEscucha);

		if (nuevoSocket != -1) {
			log_trace(logSockets,"Nuevo Socket!");
			printf("Nueva Conexion Recibida - Socket N°: %d\n",	nuevoSocket);
			int cliente;
			recibirInt(nuevoSocket,&cliente);

			switch(cliente){
				case PROCESO_NODO:
					recibirConexionDataNode(nuevoSocket);
					break;
				case PROCESO_WORKER:
					//etapa de transformación final
					transformacionFinalWorker(nuevoSocket);
					break;
				case PROCESO_YAMA:
					//respondo solicitud de bloques de archivo
					pthread_t threadSolicitudYama;
					int er1 = pthread_create(&threadSolicitudYama, NULL, procesarSolicitudYama, nuevoSocket);
					pthread_join(threadSolicitudYama);
					break;
			}
		}
	}
}

int recibirConexionDataNode(int nuevoSocket){
	//pthread_t threadEscucharConexionNodo;

	t_bitarray* t_fs_bitmap;
	int nuevo = 1;
	t_nodo * nodoReal;
	t_nodo * nodo;
	nodo = malloc(sizeof(t_nodo));
	nodo->socket_nodo = nuevoSocket;

	struct sockaddr_in sa;
	socklen_t sa_len;
	sa_len = sizeof(sa);
	if (getpeername(nodo->socket_nodo, &sa, &sa_len) == -1){
		perror("Error al traer ip nodo.");
		return -1;
	}

	strcpy(nodo->ip, inet_ntoa(sa.sin_addr));
	nodo->puerto = ntohs(sa.sin_port);

	size_t tam_buffer = 0;
	recibirInt(nuevoSocket,&tam_buffer);
	void* buffer;
	buffer = malloc(tam_buffer);
	recv(nuevoSocket, buffer, tam_buffer, NULL);

	deserializar_a_nodo(buffer, nodo);

	free(buffer);


	nodoReal = getNodoPorNombre(nodo->nombre_nodo, nodos);

	if(nodoReal != NULL){
		nodo = nodoReal;
		nodo->socket_nodo = nuevoSocket;
		nuevo = 0;
	}

	printf("Se conecto el nodo %s\n", nodo->nombre_nodo);
	printf("Cuenta con %d bloques en total.\n", nodo->tamanio/TAMANIO_BLOQUE);

	t_fs_bitmap = creaAbreBitmap(nodo->tamanio,nodo->nombre_nodo);
	int bloquesLibresNodo = cuentaBloquesLibre(nodo->tamanio,t_fs_bitmap);
	nodo->bloquesLibres = bloquesLibresNodo;

	printf("Cuenta con %d bloques libres.\n", nodo->bloquesLibres);

	if(nuevo){
	list_add(nodos,nodo);
	}

	actualizarNodosBin();

	//int er1 = pthread_create(&threadEscucharConexionNodo,NULL,escucharConexionNodo,(void*) nuevoSocket);
	destruir_bitmap(t_fs_bitmap);
	cantNodos++;

	//chequear archivos
	chequeoEstadoFS();
	//imprimir estable/no estable
	if (!estable){
		printf("Filesystem");
		printf(ANSI_COLOR_BOLD ANSI_COLOR_RED " no estable" ANSI_COLOR_RESET);
		printf(". Conectados %d de %d nodos.\n", cantNodos, list_size(nodos));
	} else {
		printf("Filesystem");
		printf(ANSI_COLOR_BOLD ANSI_COLOR_GREEN" estable" ANSI_COLOR_RESET);
		printf(". Conectados %d de %d nodos.\n", cantNodos, list_size(nodos));
	}

	return nuevoSocket;
}

void escucharConexionNodo(void* socket){
	int socketNodo = (int) socket;
	int a = 0;
	int error = 1;

	while(1){
		int i;
		int found = 0;
		t_nodo * nodo;
		for(i=0;found<1 && i<list_size(nodos);i++){
			nodo = list_get(nodos,i);
			if(nodo->socket_nodo == socketNodo){
				found = 1;
		}
		error = recv(socketNodo, &a, sizeof(int), 0);
		if(error<=0){
			nodo->socket_nodo = -1;
			printf("Se desconecto nodo del socket %d\n", socketNodo);
			break;
		} else {
				switch(a){
					case LEER_BLOQUE_NODO:
						recibirDatosBloque(nodo);
						break;
				}
			}
			//actualizarNodosBin();
		}
	}
}

t_bitarray* creaAbreBitmap(int tamNodo, char nombreNodo[10]){

	char * ruta;
	int nuevo = 0;
	ruta = malloc(sizeof(char)*256);
	snprintf(ruta, 256, "%s%s%s", "./metadata/bitmap/", nombreNodo, ".bin");

	int bloquesEnBits = (tamNodo / (1024*1024)) ;

	if(bloquesEnBits % 8 == 0)
		{
		bloquesEnBits = bloquesEnBits/8;
		}else{
		bloquesEnBits = bloquesEnBits/8 + 1;
	}

	FILE* bitmap = fopen(ruta, "rb+");
		    if(!bitmap) //archivo no existe, crear
		    {
		    	bitmap = fopen(ruta, "wb+");
		    	nuevo = 1;
		        if (bitmap == NULL)
		        	printf("Error al abrir directorios.dat\n");
		        	//exit(1);
		    }

	/* Declara el array de bits en cero si el bitmap no existía antes. Este bitmap se asigna a un dominio, en este caso, un nodo.  */

	t_bitarray* t_fs_bitmap;

	if(!nuevo){
		t_fs_bitmap = leerBitmap(bitmap, tamNodo);
	}else{
		//memset(bitarray,0,sizeof(char)*bloquesEnBits);
		t_fs_bitmap = crearBitmapVacio(tamNodo);

		fwrite(t_fs_bitmap->bitarray,(size_t) bloquesEnBits,1,bitmap);
		//txt_write_in_file(bitmap,t_fs_bitmap->bitarray);
	}

	fclose(bitmap);
	free(ruta);
	return t_fs_bitmap;
}

t_bitarray *crearBitmapVacio(int tamNodo) {
	int cantBloq = tamNodo / (1024*1024);
	size_t bytes = ROUNDUP(cantBloq, CHAR_BIT);
	char *bitarray = calloc(bytes, sizeof(char));
	return bitarray_create_with_mode(bitarray, bytes, LSB_FIRST);
}

t_bitarray *leerBitmap(FILE* bitmap_file, int tamNodo) {

	int cantBloq = tamNodo / (1024*1024);
	size_t bitarray_size = ROUNDUP(cantBloq, CHAR_BIT); // CHAR_BIT = cantidad bits x char

	char *bitarray = malloc(bitarray_size);

	size_t read_bytes = fread(bitarray, 1, bitarray_size, bitmap_file);

	if (read_bytes != bitarray_size) {
		fclose(bitmap_file);
		free(bitarray);
		//logear_error("El Bitmap esta incompleto", true);
		return NULL;
	}

	return bitarray_create_with_mode(bitarray, bitarray_size, LSB_FIRST);
}

bool escribirBitMap(int tamNodo, char nombreNodo[10], t_bitarray* t_fs_bitmap){

	char * ruta;
	int nuevo = 0;
	ruta = malloc(sizeof(char)*256);
	snprintf(ruta, 256, "%s%s%s", "./metadata/bitmap/", nombreNodo, ".bin");

	FILE* bitmap = fopen(ruta, "w");
	int bytes = fwrite(t_fs_bitmap->bitarray, sizeof(char), t_fs_bitmap->size, bitmap);
		if (bytes != t_fs_bitmap->size) {
			// logear_error("Error al actualizar el archivo \"Bitmap.bin\"", false);
			fclose(bitmap);
			free(ruta);
			return 0;
		}

	free(ruta);
	fclose(bitmap);
	return 1;
}

int findFreeBloque(int tamNodo, t_bitarray* t_fs_bitmap){

	int bloques = (tamNodo / (1024*1024));
	int pos = 0, i = 0;
		for (i = 0; i < bloques; i++) {
			 if(bitarray_test_bit(t_fs_bitmap, i) == 0){
					pos = i;
					break;
			}
		}
	return pos;
}

int cuentaBloquesLibre(int tamNodo, t_bitarray* t_fs_bitmap){

	int bloques = (tamNodo / (1024*1024)) ;

	int libre = 0;
	int i = 0;
		for (; i < bloques; i++) {
			 if(!bitarray_test_bit(t_fs_bitmap, i)){ // if(t_fs_bitmap->bitarray[i] == 0){
					libre++;
			}
		}
	return libre;
}

t_bitarray *limpiar_bitmap(int tamNodo, char* nomNodo[10], t_bitarray* bitmap) {
	memset(bitmap->bitarray, 0, bitmap->size);
	escribirBitMap(tamNodo, nomNodo, bitmap);
	return bitmap;
}

void destruir_bitmap(t_bitarray* bitmap) {
	free(bitmap->bitarray);
	bitarray_destroy(bitmap);
}

void deserializar_a_nodo(void* serializado, t_nodo *nodo){
	int offset =0;
	deserializar_a_int(serializado, &nodo->tamanio,&offset);
	deserializar_a_int(serializado, &nodo->bloquesLibres,&offset);
	deserializar_a_string(serializado, nodo->nombre_nodo, sizeof(char[10]),&offset);
}

void actualizarNodosBin(){

	char * descripcion;
	int size = list_size(nodos);
	descripcion = malloc(sizeof(char[11])*size);
	int tamanio = 0;
	int libre = 0;
	int i = 0;

	for(;i<size;i++){
		t_nodo * nodo = list_get(nodos,i);
		tamanio = tamanio + nodo->tamanio;
		libre = libre + nodo->bloquesLibres;
		if(i>0){
			strcat(descripcion,",");
			strcat(descripcion,nodo->nombre_nodo);
		}else{
			strcpy(descripcion,nodo->nombre_nodo);
		}

	}

	pthread_mutex_lock(&mx_nodobin);
	FILE* nodosbin = fopen(nodos_file,"wb+");
	fprintf(nodosbin, "TAMANIO=%d\n", tamanio/(1024*1024));
	fprintf(nodosbin, "LIBRE=%d\n", libre);
	fprintf(nodosbin, "Q_NODOS=%d\n", size);
	fprintf(nodosbin, "NODOS=[%s]\n",descripcion);

	for(i=0;i<size;i++){
		t_nodo * nodo = list_get(nodos,i);
		fprintf(nodosbin, "%sTotal=%d\n", nodo->nombre_nodo, nodo->tamanio/(1024*1024));
		fprintf(nodosbin, "%sLibre=%d\n", nodo->nombre_nodo, nodo->bloquesLibres);

	}
	fclose(nodosbin);
	pthread_mutex_unlock(&mx_nodobin);

}

void imprimeNodosBin(){

	FILE* nodosbin = fopen(nodos_file,"r");

	char * line = NULL;
	size_t len = 0;

	do {
		getline(&line, &len, nodosbin);
		if(feof(nodosbin)){break;};
		printf("%s", line);
	} while (!feof(nodosbin));

	fclose(nodosbin);

	if (line){
	   free(line);}
}

int traeBloquesLibres(){

	FILE* nodosbin = fopen(nodos_file,"r");

	char * line = NULL;
	size_t len = 0;

	getline(&line, &len, nodosbin);
	getline(&line, &len, nodosbin);
	char** parametros;

	parametros = string_split(line,"=");
	int libres = atoi(parametros[1]);

	fclose(nodosbin);
	free(parametros);
	if (line){
	   free(line);}

	return libres;
}

void *escucharConsola(){
	t_log_level logL;
	t_log* logFS = log_create("log.txt","Yamafs",0,logL);

	carpetas = inicializarDirectorios();

	t_directory * carpetaActual = list_get(carpetas, 0); // donde 0 siempre DEBERÍA SER root

	char * linea;

	  while(1) {
		linea = readline("yamafs:" );

		if(linea)
		  add_history(linea);

		if(!strncmp(linea, "exit", 4)) {
		   log_trace(logFS,"Consola recibe ""exit""");
		   log_destroy(logFS);
		   free(linea);
		   exit(1);
		} else
		if(!strncmp(linea, "format", 6)) {
			log_trace(logFS,"Consola recibe ""format""");
			formatFs();
		}else
		if(!strncmp(linea, "rm", 2)) {
			log_trace(logFS,"Consola recibe ""rm""");
			// printf("Seleccionaste remover\n");
			char ** parametros = string_split(linea, " ");

			if(parametros[1] == NULL){
				printf("Faltan argumentos.\n");}
			else if (parametros[2] == NULL){
				removerArchivo(parametros[1], "", carpetas);
			} else {
				removerArchivo(parametros[1], parametros[2], carpetas);
			}

		}else
		if(!strncmp(linea, "rename", 6)) {
			log_trace(logFS,"Consola recibe ""rename""");
			//printf("Seleccionaste renombrar\n");
			char ** parametros = string_split(linea, " ");
				if(parametros[1] == NULL){
					printf("Faltan argumentos: rename [path_original] [nombre_final]  \n");
				}
				else if (parametros[2] == NULL){
					printf("Faltan argumentos: rename [path_original] [nombre_final]  \n");
				} else {
					renombrarArchivo(parametros[1], parametros[2], carpetas);
				}
		}
		else
		if(!strncmp(linea, "mv", 2)) {
			log_trace(logFS,"Consola recibe ""mv""");
			printf("Seleccionaste mover\n");

		}
		else
		if(!strncmp(linea, "cat", 3)) {
			log_trace(logFS,"Consola recibe ""cat""");
			//printf("Seleccionaste concatenar\n");

			char ** parametros = string_split(linea, " ");

		}
		else
		if(!strncmp(linea, "mkdir", 5)) {
			log_trace(logFS,"Consola recibe ""mkdir""");

			char ** parametros = string_split(linea, " ");
			/* variable = ltrim(variable);*/

			if(parametros[1] == NULL){
				printf("Falta argumento: nombre a designar al directorio.\n");}
			else {
				crearDirectorio(carpetas, carpetaActual, parametros[1]);
			}
			// printf("Seleccionaste crear carpeta %s\n", linea);
		}
		else
		if(!strncmp(linea, "cd", 2)) {
			log_trace(logFS,"Consola recibe ""cd""");
			// printf("Seleccionaste cambiar diretorio\n");
			char ** parametros = string_split(linea, " ");
			carpetaActual = cambiarAdirectorio(parametros[1], carpetaActual, carpetas);

		}
		else
		if(!strncmp(linea, "cpfrom", 6)) {
			log_trace(logFS,"Consola recibe ""cpfrom""");
			// printf("Seleccionaste copiar desde\n");
			char ** parametros = string_split(linea, " ");
			char * tipo_archivo = parametros[3];
			if(tipo_archivo != NULL && !strcmp(parametros[3],"1")){
				guardarArchivoLocalDeTextoEnFS(parametros[1],parametros[2], carpetas);
			}else{
				guardarArchivoLocalEnFS(parametros[1],parametros[2], carpetas);
			}
		}else
		if(!strncmp(linea, "cpto", 4)) {
			log_trace(logFS,"Consola recibe ""cpto""");
			// printf("Seleccionaste copiar hasta\n");

			if(estaEstable){
				char ** parametros = string_split(linea, " ");
				traerArchivoDeFs(parametros[1],parametros[2], carpetas, 0);
			}

		}else
		if(!strncmp(linea, "cpblock", 7)) {
			log_trace(logFS,"Consola recibe ""cpblock""");
			printf("Seleccionaste copiar bloque\n");

		}else
		if(!strncmp(linea, "md5", 3)) {
			log_trace(logFS,"Consola recibe ""md5""");
			//printf("Seleccionaste obtener md5\n");
			char ** parametros = string_split(linea, " ");
			obtenerMD5Archivo(parametros[1], carpetas);
		}else
		if(!strncmp(linea, "ls", 2)) {
			log_trace(logFS,"Consola recibe ""ls""");

			listarDirectorios(carpetas, carpetaActual);
			char* ruta =string_new();
			string_append(&ruta, "ls metadata/archivos/");
			string_append(&ruta, string_itoa(carpetaActual->index));
			string_append(&ruta, " | tr '\n' ' '");
			system(ruta);
			printf("\n");
			//printf("Seleccionaste ver directorios y archivos\n");

		}else
		if(!strncmp(linea, "info", 4)) {
			log_trace(logFS,"Consola recibe ""info""");
			//printf("Seleccionaste obtener informacion\n");
			char ** parametros = string_split(linea, " ");
			imprimeMetadata(parametros[1], carpetas);

		}else
		if(!strncmp(linea, "statusfs", 4)) {
			log_trace(logFS,"Consola recibe ""statusfs""");

			imprimeNodosBin();
		}else
		if(!strncmp(linea, "clear", 4)) {
			log_trace(logFS,"Consola recibe ""cls""");

			cls();
		}else
		if(!strncmp(linea, "help",4)) {
			log_trace(logFS,"Consola recibe ""help""");
			  printf("YamaFS Ayuda\n");
			  printf("Los parámetros se indican con [] \n------\n");
			  printf("format - Formatear el Filesystem.\n");
			  printf("rename [path_original] [nombre_final] - Renombra un Archivo o Directorio");
			  printf("rm [path_archivo] ó rm -d [path_directorio] ó rm -b [path_archivo] [nro_bloque] [nro_copia] - Eliminar un Archivo/Directorio/Bloque.\n");
			  printf("mv [path_original] [path_final] - Mueve un Archivo o Directorio.\n");
			  printf("cat [path_archivo] - Muestra el contenido del archivo como texto plano.\n");
			  printf("mkdir [path_dir] - Crea un directorio. \n");
			  printf("cpfrom [path_archivo_origen] [directorio_yamafs] [tipo_archivo]- Copiar un archivo local al yamafs, siguiendo los lineamientos en la operación Almacenar Archivo de la Interfaz del FileSystem. 1 para archivo de texto o 0 para binario, si no se especifica, por defecto se considera archivo binario.\n");
			  printf("cpto [path_archivo_yamafs] [directorio_filesystem] - Copiar un archivo local desde el yamafs.\n");
			  printf("cpblock [path_archivo] [nro_bloque] [id_nodo] - Crea una copia de un bloque de un archivo en el nodo dado.\n");
			  printf("md5 [path_archivo_yamafs] - Solicitar el MD5 de un archivo en yamafs.\n");
			  printf("ls [path_directorio] - Lista los archivos de un directorio.\n");
			  printf("info [path_archivo] - Muestra toda la información del archivo, incluyendo tamaño, bloques, ubicación de los bloques, etc.\n");
			  printf("exit - Sale del programa.\n");
		}
		else {
			printf("No se reconoce el comando %s\n",linea);
			printf("Para más información utilice el comando ""help"".\n");
		}
  }
}

void actualizoArchivosDat(char* ruta_metadata, int flag){

	/* flag = 1 - agrega
	 * flag = 0 - borra */

	if(flag){
		FILE* file = fopen(archivos_file, "a");

		if (file == NULL){
			printf("Error en el registro de archivos.dat.\n");
			return;
		}


		fprintf(file, ruta_metadata);
		fprintf(file, "\n");
		fclose(file);
		}
	else {

		FILE* file = fopen(archivos_file, "r+");
		if (file == NULL){
			printf("Error en el registro de archivos.dat.\n");
			return;
		}

		string_append(&ruta_metadata,"\n");
		char * str = string_new();
		char * line = NULL;
		size_t len = 0;
		getline(&line,&len,file);

		while(!feof(file)){

			if(strcmp(ruta_metadata,line)){
				string_append(&str,line);
			}
			getline(&line,&len,file);
		}
		fclose(file);

		file = fopen(archivos_file, "w");
		fprintf(file, str);
		fclose(file);

	}

}

int chequeoEstadoFS(){

	int new = 0;
	FILE* file = fopen(archivos_file, "r");

	if(file == NULL){
		FILE* file = fopen(archivos_file, "w");
		new = 1;
	}

	t_list *listaNodosRelacionados;
	listaNodosRelacionados = list_create();

	t_nodo * nodo;

	char * line = NULL;
	size_t len = 0;
	int iteration = 0;
	if (!new){
		while(!feof(file)){
			getline(&line, &len, file);

			if(feof(file))break;

			line[strlen(line)-1]=NULL;

			getNodosRelacionadosDeMetadata(line, listaNodosRelacionados);
			iteration++;
		}
	}
	if(iteration == 0 && list_size(nodos) >= 2){
		estable = 1;
	} else if(iteration == 0 && list_size(nodos) < 2){
		estable = 0;
	}

	int i, size;
	i = 0;
	size = list_size(listaNodosRelacionados);

	for(;i < size;i++){
		nodo = list_get(listaNodosRelacionados,i);
		enviarInt(nodo->socket_nodo, ESTA_VIVO_NODO);
		if(nodo->socket_nodo != -1){
			recibirInt(nodo->socket_nodo,&estable);
		}
	}

	list_destroy(listaNodosRelacionados);
	if(line)free(line);
	return estable;
}

void getNodosRelacionadosDeMetadata(char* ruta_metadata, t_list* listaNodosRelacionados){

		FILE * metadata;
		t_nodo * nodo;
		char * line = NULL;
		size_t len = 0;

		metadata = fopen(ruta_metadata,"r");
		if (metadata == NULL){
			exit(EXIT_FAILURE);}

		//linea de la ruta
		getline(&line, &len, metadata);
		//linea de tamaño
		getline(&line, &len, metadata);

		char** parametros;
		char ** parametros1;

		//linea de TIPO
		getline(&line, &len, metadata);

		while (!feof(metadata)) {

			getline(&line, &len, metadata); //primer getline lo hago antes para evaluar si esta para salir
			if(feof(metadata))break;

			int bloque;
			t_bloque* currentBloque;
			currentBloque = malloc(sizeof(t_bloque));

			/* SECTOR COPIA 0 */
			parametros = string_split(line,"=");
			currentBloque->Copia0 = string_new();
			string_append(&currentBloque->Copia0, parametros[1]);// string_get_string_as_array(parametros[1]);
			parametros1 = string_get_string_as_array(currentBloque->Copia0);
			nodo = getNodoPorNombre(parametros1[0],nodos);

			if(getNodoPorNombre(nodo->nombre_nodo,listaNodosRelacionados)== NULL){
				list_add(listaNodosRelacionados,nodo);
			}

			/* SECTOR COPIA 1 */
			getline(&line, &len, metadata);
			parametros = string_split(line,"=");
			currentBloque->Copia1 = string_new();
			string_append(&currentBloque->Copia1, parametros[1]);// string_get_string_as_array(parametros[1]);
			parametros1 = string_get_string_as_array(currentBloque->Copia1);
			nodo = getNodoPorNombre(parametros1[0],nodos);

			if(getNodoPorNombre(nodo->nombre_nodo,listaNodosRelacionados)== NULL){
				list_add(listaNodosRelacionados,nodo);
			}

			/*SECTOR TAMAÑO EN BYTES */
			getline(&line, &len, metadata);

		}

		fclose(metadata);
		if(line) free(line);

}

char* getNombreArchivo(char* path){
	int i = 0;
	char** arrayString = string_split(path,"/");
		while (arrayString[i]!= NULL){
			i++;
		}
	return arrayString[i-1];
}

char* getRutaMetadata(char* ruta_archivo, t_list* folderList, int carpeta){

	char* ruta_metadata = string_new();

	string_append(&ruta_metadata, "./metadata/archivos/");
	string_append(&ruta_metadata, string_itoa(carpeta));
	string_append(&ruta_metadata, "/");
	string_append(&ruta_metadata, getNombreArchivo(ruta_archivo));

	return ruta_metadata;
}

void guardarArchivoLocalEnFS(char* path_archivo_origen, char* directorio_yamafs, t_list* folderList){

	FILE* origen = fopen(path_archivo_origen, "rb");
	if (origen == NULL){
				fprintf(stderr, "Fallo al abrir el archivo %s %s\n",path_archivo_origen, strerror(errno));
				return;
			}

	int fd = fileno(origen);

	struct stat fileStat;
			if (fstat(fd, &fileStat) < 0){
				  fprintf(stderr, "Error fstat --> %s\n", strerror(errno),"\n");
				  return;
			}


	int bloquesLibres = traeBloquesLibres();

	if(((int)fileStat.st_size/(1024*1024))*2 > bloquesLibres){
		printf("No hay espacio suficiente en FS para guardar el archivo.\n");
		return;
	}

	FILE * metadata = crearMetadata(path_archivo_origen, directorio_yamafs, folderList, "BIN", (int)fileStat.st_size);

	if(metadata == NULL){
		printf("Error al guardar el archivo.\n");
		return;
	}

	int socketnodo;
	int socketnodo2; // para el formato RAID 1

	t_nodo* nodo;
	t_nodo* nodo2; // para el formato RAID 1

	int iteration=0;
	int bloque = 0;
	int bloqueCopia = 0;

	void* buffer;
	size_t bytesRead;
	buffer = malloc(sizeof(char)*1024*1024);

	while(!feof(origen)){
		bytesRead = 0;
		t_bitarray* t_fs_bitmap;
		t_bitarray* t_fs_bitmap2;
		t_list * nodosAusar;

		nodosAusar = list_create();
		nodosAusar = getNodosMenosCargados(nodos);

		nodo = list_get(nodosAusar,0);
		socketnodo = nodo->socket_nodo;

		nodo2 = list_get(nodosAusar,1);
		socketnodo2 = nodo2->socket_nodo;

		int err = enviarInt(socketnodo,ESCRIBIR_BLOQUE_NODO);
		//enviarInt(socketnodo,ENVIAR_ARCHIVO_BINARIO);
		if(err<0){
			printf("Error de conexion con el nodo %s\n", nodo->nombre_nodo);
			return;
		}

		err = enviarInt(socketnodo2,ESCRIBIR_BLOQUE_NODO);
		//enviarInt(socketnodo2,ENVIAR_ARCHIVO_BINARIO);
		if(err<0){
			printf("Error de conexion con el nodo %s\n", nodo2->nombre_nodo);
			return;
		}

		t_fs_bitmap = creaAbreBitmap(nodo->tamanio, nodo->nombre_nodo);
		t_fs_bitmap2 = creaAbreBitmap(nodo2->tamanio, nodo2->nombre_nodo);
		bloque = findFreeBloque(nodo->tamanio, t_fs_bitmap);
		bloqueCopia = findFreeBloque(nodo2->tamanio, t_fs_bitmap2);

		if(enviarInt(socketnodo,bloque) > 0 && enviarInt(socketnodo2,bloqueCopia) > 0){
			bitarray_set_bit(t_fs_bitmap,bloque);
			bitarray_set_bit(t_fs_bitmap2,bloqueCopia);
			escribirBitMap(nodo->tamanio, nodo->nombre_nodo, t_fs_bitmap);
			escribirBitMap(nodo2->tamanio, nodo2->nombre_nodo, t_fs_bitmap2);
			destruir_bitmap(t_fs_bitmap);
			destruir_bitmap(t_fs_bitmap2);

			int leido = fread(buffer, 1, sizeof(char)*1024*1024, origen);
			bytesRead += leido;

			escribirBloque(socketnodo, bloque, buffer, leido);
			escribirBloque(socketnodo2, bloqueCopia, buffer, leido);

			fprintf(metadata,"%s%d%s%s%s%d%s%s","BLOQUE",iteration,"COPIA0=[",nodo->nombre_nodo, ", ", bloque, "]","\n");
			fprintf(metadata,"%s%d%s%s%s%d%s%s","BLOQUE",iteration,"COPIA1=[",nodo2->nombre_nodo, ", ", bloqueCopia, "]","\n");
			fprintf(metadata,"%s%d%s%d%s","BLOQUE",iteration,"BYTES=",bytesRead, "\n");

			nodo->bloquesLibres = nodo->bloquesLibres - 1;
			nodo2->bloquesLibres = nodo2->bloquesLibres - 1;

		}

	list_destroy(nodosAusar);
	iteration++;
	}

	free(buffer);
	fclose(origen);
	fclose(metadata);

	actualizarNodosBin();

	printf("¡Archivo guardado con éxito! Cantidad de bloques: %d\n", iteration);
}

void guardarArchivoLocalDeTextoEnFS(char* path_archivo_origen, char* directorio_yamafs, t_list* folderList){

	FILE* origen = fopen(path_archivo_origen, "rb");
	if (origen == NULL){
				fprintf(stderr, "Fallo al abrir el archivo %s %s\n",path_archivo_origen, strerror(errno));
				return;
			}
	int fd = fileno(origen);
	struct stat fileStat;
			if (fstat(fd, &fileStat) < 0){
				  fprintf(stderr, "Error fstat --> %s\n", strerror(errno),"\n");
				  return;
			}



	int bloquesLibres = traeBloquesLibres();

	if(((int)fileStat.st_size/(1024*1024))*2 > bloquesLibres){
			printf("No hay espacio suficiente en FS para guardar el archivo.\n");
			return;
		}

	FILE * metadata = crearMetadata(path_archivo_origen, directorio_yamafs, folderList, "TEXTO", (int)fileStat.st_size);

	if(metadata == NULL){
		printf("Error al guardar el archivo.\n");
		return;
	}

	int socketnodo;
	int socketnodo2; // para el formato RAID 1

	t_nodo* nodo;
	t_nodo* nodo2; // para el formato RAID 1

	int iteration=0;
	int bloque = 0;
	int bloqueCopia = 0;

	char * hastaNuevaLinea;
	hastaNuevaLinea = malloc(1024*1024);

	while(!feof(origen)){

		t_bitarray* t_fs_bitmap;
		t_bitarray* t_fs_bitmap2;
		t_list * nodosAusar;

		nodosAusar = list_create();
		nodosAusar = getNodosMenosCargados(nodos);

		nodo = list_get(nodosAusar,0);
		socketnodo = nodo->socket_nodo;

		nodo2 = list_get(nodosAusar,1);
		socketnodo2 = nodo2->socket_nodo;
		int errNodo, errNodo2;

		errNodo = enviarInt(socketnodo,ESCRIBIR_BLOQUE_NODO);
		if(errNodo<0){
			printf("Error de conexion con el nodo %s\n", nodo->nombre_nodo);
		}
		errNodo2 = enviarInt(socketnodo2,ESCRIBIR_BLOQUE_NODO);
		//enviarInt(socketnodo2,ENVIAR_ARCHIVO_BINARIO);
		if(errNodo2<0){
			printf("Error de conexion con el nodo %s\n", nodo2->nombre_nodo);
		}

		t_fs_bitmap = creaAbreBitmap(nodo->tamanio, nodo->nombre_nodo);
		t_fs_bitmap2 = creaAbreBitmap(nodo2->tamanio, nodo2->nombre_nodo);
		bloque = findFreeBloque(nodo->tamanio, t_fs_bitmap);
		bloqueCopia = findFreeBloque(nodo2->tamanio, t_fs_bitmap2);

		if(enviarInt(socketnodo,bloque) > 0 && enviarInt(socketnodo2,bloqueCopia) > 0){
			bitarray_set_bit(t_fs_bitmap,bloque);
			bitarray_set_bit(t_fs_bitmap2,bloqueCopia);
			escribirBitMap(nodo->tamanio, nodo->nombre_nodo, t_fs_bitmap);
			escribirBitMap(nodo2->tamanio, nodo2->nombre_nodo, t_fs_bitmap2);
			destruir_bitmap(t_fs_bitmap);
			destruir_bitmap(t_fs_bitmap2);

			//enviarInt(socketnodo,ENVIAR_ARCHIVO_TEXTO);
			int largo= 0;
			char * aMandar = string_new();
			string_append(&aMandar,hastaNuevaLinea);
			largo += strlen(hastaNuevaLinea);
			while(!feof(origen) && largo<1024*1024){
				fgets(hastaNuevaLinea,1024*1024,origen);
				largo += strlen(hastaNuevaLinea);

				 if(largo>1024*1024 || feof(origen)){
					 int largoAMandar = strlen(aMandar);
					 escribirBloque(socketnodo, bloque, aMandar, largoAMandar);
					 escribirBloque(socketnodo2, bloqueCopia, aMandar, largoAMandar);

					 fprintf(metadata,"%s%d%s%s%s%d%s%s","BLOQUE",iteration,"COPIA0=[",nodo->nombre_nodo, ", ", bloque, "]","\n");
					 fprintf(metadata,"%s%d%s%s%s%d%s%s","BLOQUE",iteration,"COPIA1=[",nodo2->nombre_nodo, ", ", bloqueCopia, "]","\n");
					 fprintf(metadata,"%s%d%s%d%s","BLOQUE",iteration,"BYTES=",largoAMandar, "\n");
					 free(aMandar);

					 nodo->bloquesLibres = nodo->bloquesLibres - 1;
					 nodo2->bloquesLibres = nodo2->bloquesLibres - 1;
					 //break;
				 } else {
					 string_append(&aMandar,hastaNuevaLinea);
				 }

			}

		}
		else{

			printf("Nodo %s desconectado", nodo->nombre_nodo);
		}
		list_destroy(nodosAusar);
		iteration++;
	}

	free(hastaNuevaLinea);
	fclose(origen);
	fclose(metadata);

	actualizarNodosBin();

	printf("¡Archivo guardado con éxito! Cantidad de bloques: %d\n", iteration);
}


int escribirBloque(int socketnodo, int bloque, void * buffer, int largoAMandar){

	 enviarInt(socketnodo,largoAMandar);
	 int enviado = 0;
	 while(enviado < largoAMandar){
		 int err = 0;

		 int i = largoAMandar - enviado;
		 if(i>=4096){
			 i = 4096;
		 }

		 enviarInt(socketnodo,i);
		 err = send(socketnodo,buffer+enviado,(size_t)sizeof(char)*i,NULL);
		 if(err <= 0){
			 return -1;
		 }else {
			 enviado += err;
		 }
	 }
	return enviado;
}

FILE * crearMetadata(char * destino, char* directorio_yamafs, t_list* folderList, char* tipo, int tamanio){

	int carpeta = 0;
	carpeta = identificaDirectorio(directorio_yamafs, folderList);
	if(carpeta == -2){
		return NULL;
	}

	char* ruta_metadata = getRutaMetadata(destino, folderList, carpeta);

	FILE* metadata = fopen(ruta_metadata, "r");
	if (metadata != NULL){
		fprintf(stderr, "Archivo ya existe en FS.\n");
		return NULL;
	}

	metadata = fopen(ruta_metadata,"w+");

	if (metadata == NULL){
			fprintf(stderr, "Fallo al guardar metadata en %s %s\n",metadata, strerror(errno));
			return NULL;
		}

	fprintf(metadata,"%s%s",ruta_metadata,"\n");
	actualizoArchivosDat(ruta_metadata, 1);
	fprintf(metadata,"%s%s%s","TAMANIO=",string_itoa(tamanio),"\n");
	fprintf(metadata,"%s%s%s","TIPO=",tipo,"\n");

	return metadata;

}


int estaEstable(){

	if(!estable){
		printf("Filesystem");
		printf(ANSI_COLOR_BOLD ANSI_COLOR_RED " no estable" ANSI_COLOR_RESET);
		printf(". No se puede operar de esta manera. Verifique nodos conectados.\n");
		return 0;
	}

	return 1;
}

int traerArchivoDeFs(char* archivoABuscar, char* directorio, t_list* folderList, int md5flag){

	if(!estaEstable()){
		return 0;
	}

	void destruyoBloques(void* parametro) {
		 t_bloque* dir = (t_bloque*) parametro;
		 free(dir->Copia0);
		 free(dir->Copia1);
		 free(dir);
	}

	int carpeta = identificaDirectorio(archivoABuscar, folderList);
	if(carpeta == -2){
			printf("Error al traer el archivo.\n");
			return 0;
	}

	char* ruta_metadata = getRutaMetadata(archivoABuscar,folderList, carpeta);

	char * pathObjetivo = string_new();
	if(directorio == "") {

	} else if(directorio != NULL){
		string_append(&pathObjetivo,directorio);
		if(!string_ends_with(directorio, "/")){
			string_append(&pathObjetivo,"/");
		}
	}

	string_append(&pathObjetivo,archivoABuscar);
	FILE * destino = fopen(pathObjetivo,"wb");
	if(destino == NULL){
		printf("Problema al recopilar archivo en ruta %s. \n", pathObjetivo);
		return -1;
	}

	t_list * lista_bloques = obtener_lista_metadata(ruta_metadata);

	int i = 0;
	for(;i<list_size(lista_bloques);i++){

		t_bloque* bloque = list_get(lista_bloques,i);
		char ** parametros1 = string_get_string_as_array(bloque->Copia0);
		t_nodo * nodo = getNodoPorNombre(parametros1[0],nodos);
		unsigned char * buff;
		buff = malloc((size_t)(bloque->tamanio_bloque));
		if(leerBloque(nodo,atoi(parametros1[1]),bloque->tamanio_bloque,buff)<=0){
			char ** parametros2 = string_get_string_as_array(bloque->Copia1);
			t_nodo * nodo = getNodoPorNombre(parametros2[0],nodos);
			free(parametros1);
			if(leerBloque(nodo,atoi(parametros2[1]),bloque->tamanio_bloque,buff)<=0){
				free(parametros2);
				printf("No se puede recuperar bloque %d, Nodos inaccesibles:\n%s%sFS", bloque->bloque, bloque->Copia0, bloque->Copia1);
				printf(ANSI_COLOR_BOLD ANSI_COLOR_RED " no estable" ANSI_COLOR_RESET ".\n");
				list_destroy_and_destroy_elements(lista_bloques,destruyoBloques);

				fclose(destino);
				free(ruta_metadata);
				remove(pathObjetivo);
				free(pathObjetivo);
				estable = 0;
				return -1;
			}
			free(parametros2);
		}else{
			free(parametros1);
		}
		// fprintf(destino,"%s",buff);
		fwrite(buff,sizeof(unsigned char),(size_t)(bloque->tamanio_bloque),destino);
		free(buff);
	}



	list_destroy_and_destroy_elements(lista_bloques,destruyoBloques);

	free(pathObjetivo);
	fclose(destino);
	free(ruta_metadata);

	if(!md5flag){
	printf("Archivo guardado correctamente en %s.\n", directorio);
	}

	return 1;
}

t_list * obtener_lista_metadata(char * ruta_metadata){

	FILE * metadata;

	char * line = NULL;
	size_t len = 0;

	metadata = fopen(ruta_metadata,"r");
	if (metadata == NULL){
		printf("Error al acceder a metadata.\n");
		return NULL;}

	t_list *listaBloques;
	listaBloques = list_create();

	//linea de la ruta
	getline(&line, &len, metadata);
	//linea de tamaño
	getline(&line, &len, metadata);

	char** parametros;

	parametros = string_split(line,"=");
	int tamArch = atoi(parametros[1]);
	int copiaNotAvail = 0;

	//linea de TIPO, todavía no hace nada particular
	getline(&line, &len, metadata);

	while (!feof(metadata)) {

		getline(&line, &len, metadata); //primer getline lo hago antes para evaluar si esta para salir
		if(feof(metadata))break;

		int bloque;
		t_bloque* currentBloque;
		currentBloque = malloc(sizeof(t_bloque));

		/* SECTOR COPIA 0 */
		parametros = string_split(line,"=");
		currentBloque->bloque = atoi(replace_char(replace_char(parametros[0],"BLOQUE", ""),"COPIA0", ""));
		currentBloque->Copia0 = string_new();
		string_append(&currentBloque->Copia0, parametros[1]);// string_get_string_as_array(parametros[1]);

		/* SECTOR COPIA 1 */
		getline(&line, &len, metadata);
		parametros = string_split(line,"=");
		currentBloque->Copia1 = string_new();
		string_append(&currentBloque->Copia1, parametros[1]);// string_get_string_as_array(parametros[1]);

		// currentBloque->Copia0 donde esta la copia 0, formato "[NOMBRENODO, NUMEROBLOQUE]"
		// currentBloque->Copia1 donde esta la copia 1, formato "[NOMBRENODO, NUMEROBLOQUE]"

		/*SECTOR TAMAÑO EN BYTES */
		getline(&line, &len, metadata);
		parametros = string_split(line,"=");
		currentBloque->tamanio_bloque = atoi(parametros[1]);

		/*Ya tengo el nodo y el bit donde esta el bloque, de dos fuentes distintas, y el tamaño que debería leer. Agrego todo a la lista*/
		list_add(listaBloques, currentBloque);

	}

	fclose(metadata);
	if(line) free(line);

	return listaBloques;
}

void removerArchivo(char* archivoABuscar, char* parametro, t_list* folderList){

	if(!strcmp(parametro,"-d")){

		int folderIndex = identificaDirectorio(archivoABuscar, folderList);

		// remueve directorio, se fija primero que no existan archivos dentro del mismo.
		char* path = string_new();
		string_append(&path, "./metadata/archivos/");
		string_append(&path,  string_itoa(folderIndex));

		if(!directorioVacio(path)){
			printf("Directorio no vacío, imposible remover.\n");
			return;
		}

		list_remove_and_destroy_element(folderList,folderIndex,free);
		actualizarDirectorioDat(folderList);
		char* command = string_new();
		string_append(&command, "rm -r -f ");
		string_append(&command, path);
		system(command);


		printf("Directorio eliminado con éxito.\n");

		free(path);
		free(command);
		return;
	}

	if(!strcmp(parametro,"-b")){
			return;
	}

	if(!estaEstable()){
			return;
	}

	void destruyoBloques(void* parametro) {
		 t_bloque* dir = (t_bloque*) parametro;
		 free(dir->Copia0);
		 free(dir->Copia1);
		 free(dir);
	}

	int carpeta = identificaDirectorio(archivoABuscar, folderList);

	if(carpeta == -2 || carpeta == -1){
		// printf("Error al traer el archivo.\n");
		return;
	}

	char* ruta_metadata = getRutaMetadata(archivoABuscar,folderList, carpeta);
	t_list * lista_bloques = obtener_lista_metadata(ruta_metadata);

	if(lista_bloques == NULL){
		printf("Verifique la existencia del archivo.\n");
		return;
	}
	int i = 0;

	for(;i<list_size(lista_bloques);i++){
		t_bitarray* t_fs_bitmap;
		t_bitarray* t_fs_bitmap2;
		t_bloque* bloque = list_get(lista_bloques,i);

		char ** parametros1 = string_get_string_as_array(bloque->Copia0);
		t_nodo * nodo = getNodoPorNombre(parametros1[0],nodos);

		t_fs_bitmap = creaAbreBitmap(nodo->tamanio, nodo->nombre_nodo);
		bitarray_clean_bit(t_fs_bitmap,atoi(parametros1[1]));
		escribirBitMap(nodo->tamanio, nodo->nombre_nodo, t_fs_bitmap);
		destruir_bitmap(t_fs_bitmap);
		nodo->bloquesLibres++;

		char ** parametros2 = string_get_string_as_array(bloque->Copia1);
		nodo = getNodoPorNombre(parametros2[0],nodos);

		t_fs_bitmap2 = creaAbreBitmap(nodo->tamanio, nodo->nombre_nodo);
		bitarray_clean_bit(t_fs_bitmap2,atoi(parametros2[1]));
		escribirBitMap(nodo->tamanio, nodo->nombre_nodo, t_fs_bitmap2);
		destruir_bitmap(t_fs_bitmap2);
		nodo->bloquesLibres++;

		free(parametros1);
		free(parametros2);
	}

	actualizarNodosBin();
	list_destroy_and_destroy_elements(lista_bloques,destruyoBloques);

	remove(ruta_metadata);

	actualizoArchivosDat(ruta_metadata,0);

	free(ruta_metadata);

	return;

}

void moverArchivo(char* archivoABuscar, char* destino){

}

void renombrarArchivo(char* archivoABuscar, char* nombreNuevo, t_list* folderList){

    //char* nombreActual = getNombreArchivo(archivoABuscar);
	int folderIndex = identificaDirectorio(archivoABuscar, folderList);
	char* ruta_metadata = string_new();
	ruta_metadata = getRutaMetadata(archivoABuscar,folderList, folderIndex);
	char* str = string_new();
	string_append(&str, ruta_metadata);

	if(buscoEnArchivosDat(str)){
		FILE* metadata = fopen(ruta_metadata,"r+");

		if (metadata == NULL){
			printf("Archivo inexistente en yamafs.\n");
		return;
		}

		char* new_ruta_metadata = string_new();
		string_append(&new_ruta_metadata, "./metadata/archivos/");
		string_append(&new_ruta_metadata, string_itoa(folderIndex));
		string_append(&new_ruta_metadata, "/");
		string_append(&new_ruta_metadata, nombreNuevo);

		char * line = NULL;
		size_t len = 0;
		FILE* new_metadata = fopen(new_ruta_metadata,"w");

		if (new_metadata == NULL){
			if(line)free(line);
			free(new_ruta_metadata);
			fclose(metadata);
			return;
		}

		fprintf(new_metadata,"%s%s",new_ruta_metadata,"\n");
		actualizoArchivosDat(ruta_metadata, 0);
		actualizoArchivosDat(new_ruta_metadata, 1);

		getline(&line,&len,metadata);

		while(!feof(metadata)){
			if(feof(metadata)){break;}
			getline(&line,&len,metadata);
			fprintf(new_metadata,"%s",line);
		}

		fclose(metadata);
		replace_char(ruta_metadata, '\n',NULL);
		remove(ruta_metadata);

		if(line)free(line);
		free(new_ruta_metadata);
		fclose(new_metadata);
		free(ruta_metadata);
		return;

	} else if(folderIndex > 0){
		t_directory* cambiarNombre;
		list_get(folderList, folderIndex);
		strcpy(cambiarNombre->nombre,nombreNuevo);
		return;
	} else {
		printf("Archivo o directorio erróneo, imposible renombrar.\n");
	}

}

int leerBloque(t_nodo * nodo, int bloque, int largo, unsigned char * buffer){

	int error = 0;

	error = enviarInt(nodo->socket_nodo, LEER_BLOQUE_NODO);
	if(error<=0){
		return error;
	}
	error = enviarInt(nodo->socket_nodo, bloque);
	if(error<=0){
			return error;
		}
	error = enviarInt(nodo->socket_nodo, largo);
	if(error<=0){
			return error;
	}

	int recibido = 0;
	int bytesRecibidos = 0;
	int i = 0;
	int j = 0;

	while(recibido<largo){
			int bytesAleer = 0;
			recibirInt(nodo->socket_nodo,&bytesAleer);
			unsigned char * buff;
			buff = malloc((size_t)bytesAleer);
			while(bytesRecibidos<bytesAleer){
				int rec =  recv(nodo->socket_nodo,buff,(size_t)bytesAleer-bytesRecibidos,NULL);
				bytesRecibidos += rec;
				for (;j<rec;j++){
					buffer[i]=buff[j];
					i++;
				}
				j=0;
			}
			free(buff);
			recibido += bytesRecibidos;
			bytesRecibidos = 0;

	}

		//printf("%s", buffer);
		return recibido;

}

void recibirDatosBloque(t_nodo * nodo){
	int bytesRecibidos = 0;
	int largoArecibir = 0;
		//while(bytesRecibidos<largo){
			recibirInt(nodo->socket_nodo,&largoArecibir);
			void * buffer;
			buffer = malloc((size_t)largoArecibir);
			bytesRecibidos += recv(nodo->socket_nodo,buffer,largoArecibir,NULL);
			printf("recibido");
			printf("%s", buffer);
		//	free(buffer);
		//}
}

int obtenerMD5Archivo(char * archivo, t_list* folderList){



		if(!estaEstable()){
			return 0;
		}

		traerArchivoDeFs(archivo,"",folderList, 1);
		char* command = string_new();
		string_append(&command,"md5sum ");
		string_append(&command,archivo);
		system(command);
		remove(archivo);

		free(command);

	    return 1;
}

void * cls(){
	system("clear");
}



void imprimeMetadata(char* rutaEnYamafs, t_list* folderList){

	int carpeta = identificaDirectorio(rutaEnYamafs, folderList);
	if(carpeta == -2){
			printf("Error al traer la información del archivo.\n");
			return;
	}

	char* ruta_metadata = getRutaMetadata(rutaEnYamafs,folderList, carpeta);

	FILE * metadata;

	char * line = NULL;
	size_t len = 0;

	metadata = fopen(ruta_metadata,"r");

	if(metadata == NULL){
			printf("Error al acceder al archivo.\n");
			return;
	}

	do {
		getline(&line, &len, metadata);
		if(feof(metadata)){break;};
		printf("%s", line);
	} while (!feof(metadata));

	fclose(metadata);

	if (line){
	   free(line);}
}

char* replace_char(char* str, char find, char replace){
    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}

void serializarDato(char* buffer, void* dato, int size_to_send, int* offset){
	memcpy(buffer + *offset, dato, size_to_send);
	*offset += size_to_send;
}

void deserializarDato(void* dato, char* buffer, int size, int* offset){
	memcpy(dato,buffer + *offset,size);
	*offset += size;
}

char* serializar_un_bloque(t_bloque_serializado* unBloque){
	int offset = 0;
	uint32_t tamanioBloque = getLongitud_bloques(1);

	char* buffer = malloc(sizeof(char)* tamanioBloque);

	serializarDato(buffer, &(unBloque->numero_bloque),sizeof(uint32_t), &offset);
	serializarDato(buffer, &(unBloque->bytes_ocupados) ,sizeof(uint32_t), &offset);
	serializarDato(buffer, &(unBloque->ip),LENGTH_IP,&offset);
	serializarDato(buffer, &(unBloque->puerto),sizeof(uint32_t), &offset);
	serializarDato(buffer, &(unBloque->idNodo),sizeof(uint32_t), &offset);
	return buffer;
}


char* serialize_blocks(t_bloque_serializado** bloques, uint32_t item_cantidad){
	t_bloque_serializado* aux_bloques = *bloques;
	uint32_t total_size = getLongitud_bloques(item_cantidad);
	char *serializedPackage = malloc(sizeof(char)*total_size);

	int offset = 0;

	int i;
	for (i = 0; i < item_cantidad; i++) {
		char* serialized_block = serializar_un_bloque(&aux_bloques[i]);
		uint32_t size_un_bloque = getLongitud_bloques(1);
		serializarDato(serializedPackage,&(size_un_bloque),sizeof(uint32_t),&offset); //size_un_bloque
		serializarDato(serializedPackage,serialized_block,sizeof(char)*size_un_bloque,&offset); //block
		free(serialized_block);
	}
	return serializedPackage;
}

uint32_t getLongitud_bloques(uint32_t item_cantidad){
	uint32_t total;

	total += sizeof(uint32_t);
	total += sizeof(t_bloque_serializado);
	total = total * item_cantidad;
	return total;
}


uint32_t longitudBloques(t_bloques_enviados* bloques){
	uint32_t total_size = 0;

	uint32_t size_blocks = getLongitud_bloques(bloques->cantidad_bloques);
	total_size += sizeof(uint32_t)*2; //campo cantidad_bloques y size_items
	total_size += size_blocks;

	return total_size;
}

char* serializar_bloques(t_bloques_enviados* bloques, uint32_t* id_master, uint32_t* longitud){
	char* paqueteSerializado;
	int offset = 0;
	uint32_t longitudTotal;

	longitudTotal = longitudBloques(bloques);

	paqueteSerializado = malloc(longitudTotal);

	serializarDato(paqueteSerializado, id_master, sizeof(uint32_t), &offset);

	//serializar items
	uint32_t size_blocks = getLongitud_bloques(bloques->cantidad_bloques);
	serializarDato(paqueteSerializado,&(size_blocks),sizeof(uint32_t),&offset);

	char* serialized_blocks = serialize_blocks(&(bloques->lista_bloques),bloques->cantidad_bloques);
	serializarDato(paqueteSerializado,serialized_blocks,sizeof(char)*size_blocks,&offset);

	free(serialized_blocks);

	return paqueteSerializado;

}


int serializar_y_enviar_yama(t_bloques_enviados* bloques, uint32_t id_master, int socketYama){
	char* paqueteSerializado;
	uint32_t longitud;
	int enviados;

	paqueteSerializado = serializar_bloques(bloques, &id_master, &longitud);

	enviados = enviarMensajeSocketConLongitud(socketYama, RECIBIR_BLOQUES, paqueteSerializado, longitud);

	return enviados;
}
t_bloque_serializado* crearBloqueSerializado(uint32_t numeroBloque, uint32_t bytesOcupados, char* ip, uint32_t puerto, char* idNodo, uint32_t idBloque){
	t_bloque_serializado *bloque = malloc(sizeof(t_bloque_serializado));
	bloque->numero_bloque = numeroBloque;
	bloque->bytes_ocupados = bytesOcupados;
	strcpy(bloque->ip,ip);
	bloque->puerto = puerto;
	strcpy(bloque->idNodo,idNodo);
	bloque->idBloque = idBloque;
	return bloque;
}

void agregarBloqueSerializado(t_bloques_enviados* bloquesEnviados, t_bloque_serializado* bloqueAAgregar){
	bloquesEnviados->lista_bloques = realloc(bloquesEnviados->lista_bloques,sizeof(t_bloque_serializado)*(bloquesEnviados->cantidad_bloques+1));
	bloquesEnviados->lista_bloques[bloquesEnviados->cantidad_bloques].numero_bloque=(bloqueAAgregar->numero_bloque);
	bloquesEnviados->lista_bloques[bloquesEnviados->cantidad_bloques].bytes_ocupados=(bloqueAAgregar->bytes_ocupados);
	strcpy(bloquesEnviados->lista_bloques[bloquesEnviados->cantidad_bloques].ip,bloqueAAgregar->ip);
	bloquesEnviados->lista_bloques[bloquesEnviados->cantidad_bloques].puerto = bloqueAAgregar->puerto;
	strcpy(bloquesEnviados->lista_bloques[bloquesEnviados->cantidad_bloques].idNodo,bloqueAAgregar->idNodo);
	bloquesEnviados->lista_bloques[bloquesEnviados->cantidad_bloques].idBloque = bloqueAAgregar->idBloque;
	bloquesEnviados->cantidad_bloques++;
}

void transformacionFinalWorker(int nuevoSocket){

	char* ruta_final = string_new();
	char* archivo_tmp = string_new();

	ruta_final = recibirMensaje(nuevoSocket);
	archivo_tmp = recibirMensaje(nuevoSocket);
	recibirArchivo(nuevoSocket);

	guardarArchivoLocalDeTextoEnFS(archivo_tmp,ruta_final,carpetas);

	free(ruta_final);
	free(archivo_tmp);
}

void procesarSolicitudYama(void* args){

	t_esperar_mensaje *argumentos = (t_esperar_mensaje*) args;
		int nuevoSocket = argumentos->socketCliente;
		free(args);

		Package* package = createPackage();
			int leidos = recieve_and_deserialize(package, nuevoSocket);

			int idMaster;
			char* solicitudArchivo;

			idMaster = recibirInt(nuevoSocket, &idMaster);
			solicitudArchivo = recibirMensaje(nuevoSocket);


	/*
	 *	todo completar con los bloques usados
	 *
	 *
	 *	todo usar las funciones de nahuel
	 *
	 *
	 *	serializar_y_enviar_yama(bloques, idMaster, socketYama);
	 */
}
