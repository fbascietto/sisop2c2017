
#include "funcionesfs.h"
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../bibliotecas/protocolo.h"
#include "../bibliotecas/serializacion.h"
#include "../bibliotecas/serializacion.c"


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
				printf("%s ", carpeta->nombre);
			}

		bool* carpetasNivelActual(void* parametro) {
		t_directory* dir = (t_directory*) parametro;
		return (dir->padre == carpetaActual->index);
		}

		t_list* listado;
		listado = list_filter(folderList, carpetasNivelActual);

		list_iterate(listado,imprimoCarpetas);

		printf("\n");
		/*---FORMA SIN USAR LIST_ITERATE (TAMPOCO FUNCIONA)
		t_directory* carpeta;
		int i = 0;
		for(;i<list_size(listado);i++){
			carpeta = list_get(listado,i);
			printf("%s ", carpeta->nombre);
		}
		*/
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

void cambiarAdirectorio(char* nombre, t_directory* carpetaActual, t_list* folderList){

	bool* carpetasConMismoNombre(void* parametro) {
				t_directory* dir = (t_directory*) parametro;
				return (strcmp(dir->nombre,nombre) == 0);
			}
	t_list* carpetas = list_filter(folderList,carpetasConMismoNombre);
	int encontrado = 0;
	int i = 0;
	t_directory* carpetaNueva= list_get(carpetas,i);
	switch(list_size(carpetas)){
		case 0: printf("Directorio inexistente\n"); break;
		case 1:
			if(carpetaActual->index == carpetaNueva->padre ){
				//actualizo carpetaActual
				carpetaActual->padre = carpetaNueva->padre;
				carpetaActual->nombre = carpetaNueva->nombre;
				carpetaActual->index = carpetaNueva->index;
			} else {printf("Directorio inexistente\n");}
			break;
		default:
			//recorro lista buscando carpeta con mismo padre
			while(!encontrado){
				if(carpetaActual->index == carpetaNueva->padre ){
					carpetaActual->padre = carpetaNueva->padre;
					carpetaActual->nombre = carpetaNueva->nombre;
					carpetaActual->index = carpetaNueva->index;
					encontrado = 1;

				} else if(++i > list_size(carpetas)){} else {printf("Directorio inexistente\n");break;}
			}
			break;
	}
}
int identificaDirectorio(char* directorio_yamafs, t_list* folderList){

	ordenoDirectorios(folderList);
	t_directory* carpetaActual= list_get(folderList,0); //arranco de root siempre?

	if(!string_starts_with(directorio_yamafs,"yamafs:")){
		return -1;
		}
	else {
	int i = 0;
	char* ruta;
	ruta = replace_str(directorio_yamafs,"yamafs:","");
	char** arrayString = string_split(ruta,"/");
	while (arrayString[i]!= NULL){
		cambiarAdirectorio(arrayString[i],carpetaActual,folderList);
		i++;
	}
	return carpetaActual->index;
	// TODO: encontrar la última carpeta del string y buscarla en la lista para hacer return del indice. El indice sirve para guardarlo en la tabla de archivos y para crear /metadata/archivos/<numero de carpeta>/archivo.bin
	// printf("%s", arrayString[i-1]);

	}
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
	pthread_t threadEscucharConexionNodo;

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
	list_add(nodos,nodo);

	actualizarNodosBin();

	int er1 = pthread_create(&threadEscucharConexionNodo,NULL,escucharConexionNodo,(void*) nuevoSocket);

	return nuevoSocket;
}

void escucharConexionNodo(void* socket){
	int socketNodo = (int) socket;
	int a = 0;
	int error = 1;
	while(1){
		error = recv(socketNodo, &a, sizeof(int), 0);
		if(error<=0){
			int i;
			int found = 0;
			t_nodo * nodo;
			for(i=0;found<1 && i<list_size(nodos);i++){
				nodo = list_get(nodos,i);
				if(nodo->socket_nodo == socketNodo){
					nodo->socket_nodo = -1;
					found = 1;
				}
			}
			//actualizarNodosBin();
			printf("Se desconecto nodo del socket %d\n", socketNodo);
			break;
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
			log_trace(logFS,"Consola recibe ""cpfrom""");
			// printf("Seleccionaste cambiar diretorio\n");
			char ** parametros = string_split(linea, " ");
			cambiarAdirectorio(parametros[1], carpetaActual, carpetas);


		}
		else
		if(!strncmp(linea, "cpfrom", 6)) {
			log_trace(logFS,"Consola recibe ""cpfrom""");
			printf("Seleccionaste copiar desde\n");
			char ** parametros = string_split(linea, " ");
			guardarArchivoLocalEnFS(parametros[1],parametros[2], carpetas);


		}else
		if(!strncmp(linea, "cpto", 4)) {
			log_trace(logFS,"Consola recibe ""cpto""");
			printf("Seleccionaste copiar hasta\n");

		}else
		if(!strncmp(linea, "cpblock", 7)) {
			log_trace(logFS,"Consola recibe ""cpblock""");
			printf("Seleccionaste copiar bloque\n");

		}else
		if(!strncmp(linea, "md5", 3)) {
			log_trace(logFS,"Consola recibe ""md5""");
			printf("Seleccionaste obtener md5\n");

		}else
		if(!strncmp(linea, "ls", 2)) {
			log_trace(logFS,"Consola recibe ""ls""");

			listarDirectorios(carpetas, carpetaActual);
			//printf("Seleccionaste ver directorios y archivos\n");

		}else
		if(!strncmp(linea, "info", 4)) {
			log_trace(logFS,"Consola recibe ""info""");
			printf("Seleccionaste obtener informacion\n");

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
			  printf("cpfrom [path_archivo_origen] [directorio_yamafs] - Copiar un archivo local al yamafs, siguiendo los lineamientos en la operación Almacenar Archivo de la Interfaz del FileSystem.\n");
			  printf("cpto [path_archivo_yamafs] [directorio_filesystem] - Copiar un archivo local al yamafs.\n");
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
	int i = 0;
	char** arrayString = string_split(path,"/");
		while (arrayString[i]!= NULL){
			i++;
		}
	replace_char(arrayString[i-1],'.', '_');
	return arrayString[i-1];
}

void guardarArchivoLocalEnFS(char* path_archivo_origen, char* directorio_yamafs, t_list* folderList){

	int carpeta = 0;
	carpeta = identificaDirectorio(directorio_yamafs, folderList);

	void* buffer;
	size_t bytesRead = 0;
	buffer = malloc(sizeof(char)*4096);

	FILE* origen = fopen(path_archivo_origen, "rb");
	int fd = fileno(origen);
	struct stat fileStat;
			if (fstat(fd, &fileStat) < 0){
				  fprintf(stderr, "Error fstat --> %s", strerror(errno),"\n");
				  exit(EXIT_FAILURE);
			}

	if (origen == NULL){
			fprintf(stderr, "Fallo al abrir el archivo %s %s\n",path_archivo_origen, strerror(errno));
			exit(EXIT_FAILURE);
		}

	char* ruta_metadata = string_new();
	string_append(&ruta_metadata, "./metadata/archivos/");
	string_append(&ruta_metadata, string_itoa(carpeta));
	string_append(&ruta_metadata, "/");
	string_append(&ruta_metadata, getNombreArchivo(path_archivo_origen));
	string_append(&ruta_metadata, ".dat");

	FILE* metadata = fopen(ruta_metadata,"w+");

	if (metadata == NULL){
			fprintf(stderr, "Fallo al guardar metadata en %s %s\n",metadata, strerror(errno));
			exit(EXIT_FAILURE);
		}

	fprintf(metadata,"%s%s",ruta_metadata,"\n");
	fprintf(metadata,"%s%s%s","TAMANIO=",string_itoa((int)fileStat.st_size),"\n");
	fprintf(metadata,"%s%s%s","TIPO=","BIN","\n"); //

	t_list * nodos_conectados;
		bool criterio(void* parametro) {
				t_nodo* relacion =
						(t_nodo*) parametro;
				return (relacion->socket_nodo != -1);
			}

	nodos_conectados = list_filter(nodos,criterio);

	int nodopos = 0;
	int socketnodo;
	t_nodo* nodo;


	int iteration=0;
	int bloque = 0;


	while(!feof(origen)){

		t_bitarray* t_fs_bitmap;
		nodo = list_get(nodos_conectados,nodopos);
		t_fs_bitmap = creaAbreBitmap(nodo->tamanio, nodo->nombre_nodo);
		bloque = findFreeBloque(nodo->tamanio, t_fs_bitmap);
		bitarray_set_bit(t_fs_bitmap,bloque*8);
		escribirBitMap(nodo->tamanio, nodo->nombre_nodo, t_fs_bitmap);
		socketnodo = nodo->socket_nodo;
		enviarInt(socketnodo,bloque);
		while(!feof(origen) && bytesRead<=1024*1024){
		  int leido = fread(buffer, 1, sizeof(char)*4096, origen);
		  bytesRead += leido;
		  enviarInt(socketnodo,leido);
		  send(socketnodo,buffer,(size_t)leido,NULL);
		}
		fprintf(metadata,"%s%d%s%s%s%d%s%s","BLOQUE",iteration,"=[",nodo->nombre_nodo, ", ", bloque, "]","\n");
		fprintf(metadata,"%s%d%s%d%s","BLOQUE",iteration,"BYTES=",bytesRead, "\n");

		nodopos++;
		if(nodopos >= list_size(nodos)){
			nodopos = 0;
		}
	  iteration++;
	}

	free(buffer);
	fclose(origen);
	fclose(metadata);
}


char* replace_char(char* str, char find, char replace){
    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}
