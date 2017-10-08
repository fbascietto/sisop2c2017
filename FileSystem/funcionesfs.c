
#include "funcionesfs.h"
#include "../bibliotecas/sockets.c"
#include "../bibliotecas/sockets.h"
#include <stdbool.h>
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

t_list* inicializarDirectorios(t_list* folderList){

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

	    int index = 0;
	    int father = -1;
	    folders = malloc(sizeof(t_directory));

	    if(nuevo){

    	fprintf(fptr,"%s%s%s", "Index|", "Directorio|", "Padre\n");
	    folderList = list_create();
	    folders->index = index;
	    folders->nombre = "root";
	    folders->padre = father;
	    fprintf(fptr,"%d%s%s%s%d%s",folders->index,"|",folders->nombre,"|",folders->padre,"\n");
	    index++;
	    father++;
	    list_add(folderList, folders);
	    } else {
	    //cargo directorios.dat
	    }
	    /*
	    folders->index = index;
	    folders->nombre = "user";
	    folders->padre = father;
	    list_add(folderList, folders);
	    */
	    fclose(fptr);
	    return folderList;
}/*
void listarDirectorios(t_list* folderList, int index){
		/*
		    t_directory* carpeta;
		t_list* listado;
		int padre;
		carpeta = malloc(sizeof(t_directory));

		carpeta = (t_directory*) list_get(folderList,index);
		padre = carpeta->padre;


		// debería usar list_filter()? como es el pase de parámetro condicional?

		while(carpeta->padre == padre){
			printf();
		}


		printf("\n");
} */

void crearDirectorio(t_list* folderList, int index, char* nombre){
		t_directory* carpeta;
		t_directory* current;
		int padre;


		current = (t_directory*) list_get(folderList,index);

		padre = current->padre;

		carpeta = malloc(sizeof(t_directory));
		carpeta->index = list_size(folderList)+1;
		carpeta->nombre = nombre;
		carpeta->padre = padre+1;

		list_add(folderList, carpeta);

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



	crearBitmap(nodo->tamanio,nodo->nombre_nodo);
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


void crearBitmap(int tamNodo, char* nombreNodo[10]){

	char * ruta;
	ruta = malloc(sizeof(char)*256);
	snprintf(ruta, 256, "%s%s%s", "./metadata/bitmap/", nombreNodo, ".bin");

	/* Declara el array de bits en cero. Este BIT_MAP se asigna a un dominio, en este caso, un nodo.  */
	int bitmaplen = tamNodo / (1024*1024);
	int i = 0;
	unsigned char* bit_map;
	bit_map = malloc(sizeof(unsigned char)*bitmaplen);
	for(;i<bitmaplen;i++){
		bit_map[i]="0";
	}
	//bit_map = malloc(sizeof(tamNodo/(1024*1024)));
	//memset(bit_map, 0, sizeof(tamNodo/(1024*1024)));


	FILE* bitmap = fopen(ruta ,"wb+");

	fprintf(bitmap,"%s",bit_map);
	fclose(bitmap);
	free(bit_map);
	free(ruta);

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



void *escucharConsola(void *args){
	t_log_level logL;
	t_log* logFS = log_create("log.txt","Yamafs",0,logL);
	t_arg_consola *argumentos = (t_arg_consola*) args;

	int index = argumentos->indice;
	int padre = argumentos->padre;
	t_list *directorios;

	directorios = malloc(sizeof(t_list));
	directorios = argumentos->lista;

	char * variable
	;
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

			variable = replace_str(linea,"mkdir ","");
			/* variable = ltrim(variable);*/

			if(variable==""){
				printf("Falta argumento: nombre a designar al directorio.");}
			else {
				index++;
				crearDirectorio(directorios, index, variable);

			}
			// printf("Seleccionaste crear carpeta %s\n", linea);
		}
		else
		if(!strncmp(linea, "cpfrom", 6)) {
			log_trace(logFS,"Consola recibe ""cpfrom""");
			printf("Seleccionaste copiar desde\n");
			char ** parametros = string_split(linea, " ");
			guardarArchivoLocalEnFS(parametros[1],parametros[2]);


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

		/*	listarDirectorios(directorios, padre); */
			printf("Seleccionaste ver directorios y archivos\n");

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

void guardarArchivoLocalEnFS(char* path_archivo_origen, char* directorio_yamafs){


	void* buffer;
	size_t bytesRead = 0;
	buffer = malloc(sizeof(char)*4096);

	FILE* origen = fopen(path_archivo_origen, "rb");

	if (origen == NULL){
		fprintf(stderr, "Fallo al abrir el archivo %s %s\n",path_archivo_origen, strerror(errno));
		exit(EXIT_FAILURE);
	}

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

	nodo = malloc(sizeof(t_nodo));
	int bloque=0;

	while(!feof(origen)){

	  nodo = list_get(nodos_conectados,nodopos);
	  socketnodo = nodo->socket_nodo;
	  enviarInt(socketnodo,bloque);
	  while(!feof(origen) && bytesRead<=1024*1024){
		  int leido = fread(buffer, 1, sizeof(char)*4096, origen);
		  bytesRead += leido;
		  enviarInt(socketnodo,leido);
		  send(socketnodo,buffer,(size_t)leido,NULL);

	  }
		  bloque++; //TODO: buscoBloque, función que busque el bloque libre en el bitmap de este nodo  (/metadata//bitmap/<nombrenodo>.bin)
		  nodopos++;
		  if(nodopos >= list_size(nodos)){
			  nodopos = 0;
		  }
	}

	free(buffer);
	fclose(origen);


}
