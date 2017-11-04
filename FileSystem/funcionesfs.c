
#include "funcionesfs.h"
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../bibliotecas/protocolo.h"
#include "../bibliotecas/serializacion.h"
#include "../bibliotecas/serializacion.c"
#include <openssl/md5.h>

#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void levantarNodos(int clean){


	if(clean>0){
		FILE* file = fopen(nodos_file, "wb+");
		fprintf(file,"TAMANIO=0\n");
		fprintf(file,"LIBRE=0\n");
		fprintf(file,"NODOS=[]\n");
		fclose(file);
	}

	if (nodos_file == NULL){
		printf("No se pudo abrir el archivo nodos.bin.\n");
		exit(EXIT_FAILURE);
	}

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


void listarDirectorios(t_list* folderList, t_directory* carpetaActual){

		void* imprimoCarpetas(void* parametro){
				t_directory* carpeta = (t_directory*) parametro;
				printf(ANSI_COLOR_CYAN "%s " ANSI_COLOR_RESET, carpeta->nombre);
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
		t_directory* carpeta;

		carpeta = malloc(sizeof(t_directory));
		carpeta->index = list_size(folderList);
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
				} else {printf("Directorio inexistente\n");
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

int identificaDirectorio(char* directorio_yamafs, t_list* folderList){

	ordenoDirectorios(folderList);
	t_directory* carpetaActual= list_get(folderList,0); //arranco de root siempre?

	if(!string_starts_with(directorio_yamafs,"yamafs:")){
		return -1;
	}

	int i = 0;
	char* ruta;
	ruta = string_substring_from(directorio_yamafs,7);
	char** arrayString = string_split(ruta,"/");
	while (arrayString[i]!= NULL){
		/*if((strchr(arrayString[i],'.') != NULL) && (arrayString[i+1] == NULL)){ //ignoro el componente "archivo" de la ruta, considero el fin cuando lo recibí

			break;
		}*/
		carpetaActual = cambiarAdirectorio(arrayString[i],carpetaActual,folderList);
		i++;
	}
	free(ruta);
	strcpy(directorio_yamafs,arrayString[i-1]);
	free(arrayString);
	return carpetaActual->index;

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
				case PROCESO_MASTER:
					procesarSolicitudMaster(nuevoSocket);

			}


		}
	}
}

int recibirConexionDataNode(int nuevoSocket){
	//pthread_t threadEscucharConexionNodo;

	t_nodo * nodo;
	nodo = malloc(sizeof(t_nodo));

	nodo->socket_nodo = nuevoSocket;

	size_t tam_buffer = 0;
	recibirInt(nuevoSocket,&tam_buffer);
	void* buffer;
	buffer = malloc(tam_buffer);
	recv(nuevoSocket, buffer, tam_buffer, NULL);

	deserializar_a_nodo(buffer, nodo);

	free(buffer);
	printf("Se conecto el nodo %s\n", nodo->nombre_nodo);
	printf("Cuenta con %d bloques en total.\n", nodo->tamanio/(1024*1024));

	creaAbreBitmap(nodo->tamanio,nodo->nombre_nodo);

	/*int i;
	int encontrado = 0;
	for(i = 0; i<list_size(nodos);i++){
		t_nodo * nodo_en_lista = nodos[0];
		if(nodo_en_lista->nombre_nodo == nodo->nombre_nodo) {
			nodo_en_lista->socket_nodo = nodo->socket_nodo;
			nodo_en_lista->espacioLibre = nodo->espacioLibre;
			encontrado = 1;
		}

	}
	if(encontrado == 0)*/ list_add(nodos,nodo);

	actualizarNodosBin();

	//int er1 = pthread_create(&threadEscucharConexionNodo,NULL,escucharConexionNodo,(void*) nuevoSocket);

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

t_bitarray* creaAbreBitmap(int tamNodo, char* nombreNodo[10]){

	char * ruta;
	int nuevo = 0;
	ruta = malloc(sizeof(char)*256);
	snprintf(ruta, 256, "%s%s%s", "./metadata/bitmap/", nombreNodo, ".bin");
	int sizeNodoEnBits ;
	if(tamNodo % 8 == 0)
		{
		sizeNodoEnBits = tamNodo/8;
		}else{
		sizeNodoEnBits = tamNodo/8 + 1;
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
	char bitarray[sizeNodoEnBits];
	t_bitarray* t_fs_bitmap;

	if(!nuevo){
		fread(&bitarray,sizeNodoEnBits,1,bitmap);
		t_fs_bitmap = bitarray_create_with_mode(&bitarray, (size_t) sizeNodoEnBits, LSB_FIRST); //queda seteado
	}else{
		memset(bitarray,0,sizeof(bitarray));
		t_fs_bitmap = bitarray_create_with_mode(&bitarray, (size_t) sizeNodoEnBits, LSB_FIRST); //queda seteado
		txt_write_in_file(bitmap,t_fs_bitmap->bitarray);
	}

	// fwrite(t_fs_bitmap->bitarray,sizeNodo,1,bitmap);

	fclose(bitmap);
	free(ruta);
	return t_fs_bitmap;
}

void escribirBitMap(int tamNodo, char* nombreNodo[10], t_bitarray* t_fs_bitmap){

	char * ruta;
	int nuevo = 0;
	ruta = malloc(sizeof(char)*256);
	snprintf(ruta, 256, "%s%s%s", "./metadata/bitmap/", nombreNodo, ".bin");
	int sizeNodoEnBits ;
		if(tamNodo % 8 == 0)
			{
			sizeNodoEnBits = tamNodo/8;
			}else{
			sizeNodoEnBits = tamNodo/8 + 1;
		}

	FILE* bitmap = fopen(ruta, "w");
	fwrite(t_fs_bitmap->bitarray,sizeNodoEnBits,1,bitmap);
	fclose(bitmap);
}

int findFreeBloque(int tamNodo, t_bitarray* t_fs_bitmap){
	int sizeNodoEnBits ;
		if(tamNodo % 8 == 0)
			{
			sizeNodoEnBits = tamNodo/8;
			}else{
			sizeNodoEnBits = tamNodo/8 + 1;
		}
	int pos, i = 0;
		for (i = 0; i < sizeNodoEnBits; i++) {
			 if(bitarray_test_bit(t_fs_bitmap, i*8) == 0){ // if(t_fs_bitmap->bitarray[i] == 0){
					pos = i;
					break;
			}
		}
	return pos;
}

void deserializar_a_nodo(void* serializado, t_nodo *nodo){
	int offset =0;
	deserializar_a_int(serializado, &nodo->tamanio,&offset);
	deserializar_a_int(serializado, &nodo->espacioLibre,&offset);
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
		libre = libre + nodo->espacioLibre;
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
	fprintf(nodosbin, "LIBRE=%d\n", libre/(1024*1024));
	fprintf(nodosbin, "NODOS=[%s]\n",descripcion);

	for(i=0;i<size;i++){
		t_nodo * nodo = list_get(nodos,i);
		fprintf(nodosbin, "%sTotal=%d\n", nodo->nombre_nodo, nodo->tamanio/(1024*1024));
		fprintf(nodosbin, "%sLibre=%d\n", nodo->nombre_nodo, nodo->espacioLibre/(1024*1024));

	}
	fclose(nodosbin);
	pthread_mutex_unlock(&mx_nodobin);

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

void *escucharConsola(){
	t_log_level logL;
	t_log* logFS = log_create("log.txt","Yamafs",0,logL);

	t_list *carpetas;
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
			printf("Seleccionaste formatear\n");

		}else
		if(!strncmp(linea, "rm", 2)) {
			log_trace(logFS,"Consola recibe ""rm""");
			printf("Seleccionaste remover\n");

		}else
		if(!strncmp(linea, "rename", 6)) {
			log_trace(logFS,"Consola recibe ""rename""");
			printf("Seleccionaste renombrar\n");
		}
		else
		if(!strncmp(linea, "mv", 2)) {
			log_trace(logFS,"Consola recibe ""mv""");
			printf("Seleccionaste mover\n");

		}
		else
		if(!strncmp(linea, "cat", 3)) {
			log_trace(logFS,"Consola recibe ""cat""");
			printf("Seleccionaste concatenar\n");
			char ** parametros = string_split(linea, " ");
			//leerBloque(atoi(parametros[1]), atoi(parametros[2]));
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
			char ** parametros = string_split(linea, " ");
			traerArchivoDeFs(parametros[1],parametros[2], carpetas);

		}else
		if(!strncmp(linea, "cpblock", 7)) {
			log_trace(logFS,"Consola recibe ""cpblock""");
			printf("Seleccionaste copiar bloque\n");

		}else
		if(!strncmp(linea, "md5", 3)) {
			log_trace(logFS,"Consola recibe ""md5""");
			printf("Seleccionaste obtener md5\n");
			char ** parametros = string_split(linea, " ");
			char * archivo = string_new();
			string_append(&archivo,parametros[1]);
			if(string_starts_with(parametros[1],"yamafs:")){
				traerArchivoDeFs(archivo,NULL,carpetas);
				obtenerMD5Archivo(archivo);
				remove(archivo);
			}else{
				obtenerMD5Archivo(archivo);
			}
			free(archivo);
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
		if(!strncmp(linea, "help",4)) {
			log_trace(logFS,"Consola recibe ""help""");
			  printf("YamaFS Ayuda\n");
			  printf("Los parámetros se indican con [] \n------\n");
			  printf("format - Formatear el Filesystem.\n");
			  printf("rm [path_archivo] ó rm -d [path_directorio] ó rm -b [path_archivo] [nro_bloque] [nro_copia] - Eliminar un Archivo/Directorio/Bloque.\n");
			  printf("mv [path_original] [path_final] - Mueve un Archivo o Directorio.\n");
			  printf("cat [path_archivo] - Muestra el contenido del archivo como texto plano.\n");
			  printf("mkdir [path_dir] - Crea un directorio. Si el directorio ya existe, el comando deberá informarlo.\n");
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

char* getNombreArchivo(char* path){
	int lastSlash = strrchr(path, '/');
	return string_substring_from(path, lastSlash + 1);
}

char* getRutaMetadata(char* ruta_archivo, t_list* folderList, int carpeta){
	char *fileName = getNombreArchivo(ruta_archivo);
	char* ruta_metadata = string_from_format("./metadata/archivos/%d/%s", carpeta, fileName);

	free(fileName);
	return ruta_metadata;
}

void guardarArchivoLocalEnFS(char* path_archivo_origen, char* directorio_yamafs, t_list* folderList){

	/*int carpeta = 0;
	carpeta = identificaDirectorio(directorio_yamafs, folderList);*/

	FILE* origen = fopen(path_archivo_origen, "rb");
	int fd = fileno(origen);

	struct stat fileStat;
			if (fstat(fd, &fileStat) < 0){
				  fprintf(stderr, "Error fstat --> %s\n", strerror(errno),"\n");
				  exit(EXIT_FAILURE);
			}

	if (origen == NULL){
			fprintf(stderr, "Fallo al abrir el archivo %s %s\n",path_archivo_origen, strerror(errno));
			exit(EXIT_FAILURE);
		}


	FILE * metadata = crearMetadata(path_archivo_origen, directorio_yamafs, folderList, "BIN", (int)fileStat.st_size);

/*	char* ruta_metadata = getRutaMetadata(path_archivo_origen, folderList, carpeta);

	FILE* metadata = fopen(ruta_metadata,"w+");

	if (metadata == NULL){
			fprintf(stderr, "Fallo al guardar metadata en %s %s\n",metadata, strerror(errno));
			exit(EXIT_FAILURE);
		}

	fprintf(metadata,"%s%s",ruta_metadata,"\n");
	fprintf(metadata,"%s%s%s","TAMANIO=",string_itoa((int)fileStat.st_size),"\n");
	fprintf(metadata,"%s%s%s","TIPO=","BIN","\n");*/



	int nodopos = 0;
	int socketnodo;
	t_nodo* nodo;


	int iteration=0;
	int bloque = 0;

	void* buffer;
	size_t bytesRead;
	buffer = malloc(sizeof(char)*1024*1024);


	while(!feof(origen)){
		bytesRead = 0;
		t_bitarray* t_fs_bitmap;
		nodo = list_get(nodos,nodopos);
		socketnodo = nodo->socket_nodo;
		int err = enviarInt(socketnodo,ESCRIBIR_BLOQUE_NODO);
		//enviarInt(socketnodo,ENVIAR_ARCHIVO_BINARIO);
		if(err<0){
			printf("error de conexion con el nodo %s\n", nodo->nombre_nodo);
		}
		t_fs_bitmap = creaAbreBitmap(nodo->tamanio, nodo->nombre_nodo);
		bloque = findFreeBloque(nodo->tamanio, t_fs_bitmap);
		if(enviarInt(socketnodo,bloque) > 0){
			bitarray_set_bit(t_fs_bitmap,bloque*8);
			escribirBitMap(nodo->tamanio, nodo->nombre_nodo, t_fs_bitmap);
			int leido = fread(buffer, 1, sizeof(char)*1024*1024, origen);
			bytesRead += leido;

			escribirBloque(socketnodo, bloque, buffer, leido);
			fprintf(metadata,"%s%d%s%s%s%d%s%s","BLOQUE",iteration,"=[",nodo->nombre_nodo, ", ", bloque, "]","\n");
			fprintf(metadata,"%s%d%s%d%s","BLOQUE",iteration,"BYTES=",bytesRead, "\n");
		}
		nodopos++;
		if(nodopos >= list_size(nodos)){
			nodopos = 0;
		}



	  iteration++;
	}

	free(buffer);
	fclose(origen);
	fclose(metadata);
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

	char* ruta_metadata = getRutaMetadata(destino, folderList, carpeta);

		FILE* metadata = fopen(ruta_metadata,"w+");

		if (metadata == NULL){
				fprintf(stderr, "Fallo al guardar metadata en %s %s\n",metadata, strerror(errno));
				//exit(EXIT_FAILURE);
			}

		fprintf(metadata,"%s%s",ruta_metadata,"\n");
		fprintf(metadata,"%s%s%s","TAMANIO=",string_itoa(tamanio),"\n");
		fprintf(metadata,"%s%s%s","TIPO=",tipo,"\n");

		return metadata;

}

void guardarArchivoLocalDeTextoEnFS(char* path_archivo_origen, char* directorio_yamafs, t_list* folderList){
/*
	int carpeta = 0;
	carpeta = identificaDirectorio(directorio_yamafs, folderList);
*/

	//size_t bytesRead;


	FILE* origen = fopen(path_archivo_origen, "rb");
	int fd = fileno(origen);
	struct stat fileStat;
			if (fstat(fd, &fileStat) < 0){
				  fprintf(stderr, "Error fstat --> %s\n", strerror(errno),"\n");
				  exit(EXIT_FAILURE);
			}

	if (origen == NULL){
			fprintf(stderr, "Fallo al abrir el archivo %s %s\n",path_archivo_origen, strerror(errno));
			exit(EXIT_FAILURE);
		}

	FILE * metadata = crearMetadata(path_archivo_origen, directorio_yamafs, folderList, "TEXTO", (int)fileStat.st_size);

	/*char* ruta_metadata = getRutaMetadata(path_archivo_origen, folderList, carpeta);

	FILE* metadata = fopen(ruta_metadata,"w+");

	if (metadata == NULL){
			fprintf(stderr, "Fallo al guardar metadata en %s %s\n",metadata, strerror(errno));
			exit(EXIT_FAILURE);
		}

	fprintf(metadata,"%s%s",ruta_metadata,"\n");
	fprintf(metadata,"%s%s%s","TAMANIO=",string_itoa((int)fileStat.st_size),"\n");
	fprintf(metadata,"%s%s%s","TIPO=","TEXTO","\n");
*/


	int nodopos = 0;
	int socketnodo;
	t_nodo* nodo;


	int iteration=0;
	int bloque = 0;


	char * hastaNuevaLinea;
	hastaNuevaLinea = malloc(1024*1024);
	while(!feof(origen)){

		t_bitarray* t_fs_bitmap;
		nodo = list_get(nodos,nodopos);
		socketnodo = nodo->socket_nodo;
		int err = enviarInt(socketnodo,ESCRIBIR_BLOQUE_NODO);
		if(err<0){

			printf("error de conexion con el nodo %s\n", nodo->nombre_nodo);
		}
		t_fs_bitmap = creaAbreBitmap(nodo->tamanio, nodo->nombre_nodo);
		bloque = findFreeBloque(nodo->tamanio, t_fs_bitmap);
		if(enviarInt(socketnodo,bloque) > 0){
			bitarray_set_bit(t_fs_bitmap,bloque*8);
			escribirBitMap(nodo->tamanio, nodo->nombre_nodo, t_fs_bitmap);

			//enviarInt(socketnodo,ENVIAR_ARCHIVO_TEXTO);
			int largo= 0;
			char * aMandar = string_new();
			string_append(&aMandar,hastaNuevaLinea);
			while(!feof(origen) && largo<1024*1024){
				fgets(hastaNuevaLinea,1024*1024,origen);

				largo += strlen(hastaNuevaLinea);

				 if(largo>=1024*1024 || feof(origen)){
					 int largoAMandar = strlen(aMandar);
					 escribirBloque(socketnodo, bloque, aMandar, largoAMandar);
					 /*enviarInt(socketnodo,largoAMandar);
					 int enviado = 0;
					 while(enviado < largoAMandar){
						 char buff[4096];
						 int i = 0;
						 for(;i<4096 && (enviado + i < largoAMandar) ;i++){
							 buff[i] = aMandar[enviado + i];
						 }

						 enviarInt(socketnodo,i);
						 enviado += send(socketnodo,buff,(size_t)i,NULL);
					 }*/

					 fprintf(metadata,"%s%d%s%s%s%d%s%s","BLOQUE",iteration,"=[",nodo->nombre_nodo, ", ", bloque, "]","\n");
					 fprintf(metadata,"%s%d%s%d%s","BLOQUE",iteration,"BYTES=",largoAMandar, "\n");
					 free(aMandar);
					 //break;
				 } else {
					 string_append(&aMandar,hastaNuevaLinea);
				 }

			}

		}else{
			printf("nodo %s desconectado", nodo->nombre_nodo);
		}
		nodopos++;
		if(nodopos >= list_size(nodos)){
			nodopos = 0;
		}
	  iteration++;
	}
	free(hastaNuevaLinea);
	fclose(origen);
	fclose(metadata);
	printf("¡Archivo guardado con éxito! Cantidad de bloques: %d\n", iteration);
}


int traerArchivoDeFs(char* archivoABuscar, char* directorio, t_list* folderList){

	int carpeta = identificaDirectorio(archivoABuscar, folderList);
	char* ruta_metadata = getRutaMetadata(archivoABuscar,folderList, carpeta);


	char * pathObjetivo = string_new();
	    if(directorio != NULL){
			string_append(&pathObjetivo,directorio);
			if(!string_ends_with(directorio, "/")){
				string_append(&pathObjetivo,"/");
			}
	    }

	    string_append(&pathObjetivo,archivoABuscar);
	    FILE * destino = fopen(pathObjetivo,"wb+");
	    if(destino == NULL){
	    	printf("problema al recopilar archivo %s", pathObjetivo);
	    	return -1;
	    }

	    t_list * lista_bloques = obtener_lista_metadata(ruta_metadata);

	    int i = 0;
	    for(;i<list_size(lista_bloques);i++){

	    	t_bloque* bloque = list_get(lista_bloques,i);
	    	char ** parametros1 = string_get_string_as_array(bloque->Copia0);
	    	t_nodo * nodo = getNodoPorNombre(parametros1[1],nodos);
	    	unsigned char * buffer;
	    	buffer = malloc((size_t)(bloque->tamanio_bloque));
	    	if(leerBloque(nodo,atoi(parametros1[2]),bloque->tamanio_bloque,buffer)<=0){
	    		char ** parametros2 = string_get_string_as_array(bloque->Copia1);
	    		t_nodo * nodo = getNodoPorNombre(parametros2[1],nodos);
	    		free(parametros1);
	    		if(leerBloque(nodo,atoi(parametros2[2]),bloque->tamanio_bloque,buffer)<=0){
	    			free(parametros2);
	    			printf("no se puede recuperar 1 o mas bloques");
	    			return -1;
	    		}
	    		free(parametros2);
	    	}else{
	    		free(parametros1);
	    	}
	    	fprintf(destino,"%s",buffer);
	    	free(buffer);
	    }



	    void carpetasNivelActual(void* parametro) {
	     t_directory* dir = (t_directory*) parametro;
	     free(parametro);
	    }



	list_destroy_and_destroy_elements(lista_bloques,carpetasNivelActual);

	free(pathObjetivo);
	fclose(destino);
	free(ruta_metadata);

	return 1;
}

t_list * obtener_lista_metadata(char * ruta_metadata){

	FILE * metadata;

			char * line = NULL;
			size_t len = 0;

		metadata = fopen(ruta_metadata,"r");
		if (metadata == NULL){
			exit(EXIT_FAILURE);}




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

	//		char** arrayBloqueC0;
	//		char** arrayBloqueC1;
	//		t_nodo* nodoBloque;

			int bloque;

			t_bloque* currentBloque;
			currentBloque = malloc(sizeof(t_directory));

			/* SECTOR COPIA 0 */
			getline(&line, &len, metadata);
			parametros = string_split(line,"=");
			currentBloque->bloque = atoi(replace_char(replace_char(parametros[0],"BLOQUE", ""),"COPIA0", ""));
			strcpy(currentBloque->Copia0, parametros[1]);// string_get_string_as_array(parametros[1]);

			/* SECTOR COPIA 1
			getline(&line, &len, metadata);
			parametros = string_split(line,"=");
			strcpy(currentBloque->Copia1, parametros[1]);// string_get_string_as_array(parametros[1]); */


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
		return bytesRecibidos;

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



int obtenerMD5Archivo(char * archivo){
		unsigned char c[MD5_DIGEST_LENGTH];

	    int i;
	    FILE *inFile = fopen (archivo, "rb");
	    MD5_CTX mdContext;
	    int bytes;
	    unsigned char data[1024];

	    if (inFile == NULL) {
	        printf ("%s can't be opened.\n", archivo);
	        return -1;
	    } else {

			if(MD5_Init (&mdContext)<0){
				return -1;
			}
				while ((bytes = fread (data, 1, 1024, inFile)) != 0){
					if(	MD5_Update (&mdContext, data, bytes) < 0){
						return -1;
					}
				}
				if(MD5_Final (c,&mdContext)<0) return -1;
				for(i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", c[i]);
				printf (" %s\n", archivo);
				fclose (inFile);
	    }

	    return 1;
}

void * imprimeMetadata(char* rutaEnYamafs, t_list* folderList){

	int carpeta = identificaDirectorio(rutaEnYamafs, folderList);
	char* ruta_metadata = getRutaMetadata(rutaEnYamafs,folderList, carpeta);

	FILE * metadata;

	char * line = NULL;
	size_t len = 0;

	metadata = fopen(ruta_metadata,"r");

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


