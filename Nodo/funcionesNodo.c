#include "funcionesNodo.h"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include "../bibliotecas/serializacion.c"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>




t_nodo* inicializoDataBin(char* rutaBin, char* nombreNodo){

		t_nodo* nodo;
		nodo = malloc(sizeof(t_nodo));
		struct stat file_stat;
		int data;
		data = open(rutaBin,O_RDWR);

		if (data == NULL)
				{
					printf("No se pudo abrir el archivo.\n");
					 exit(EXIT_FAILURE);
				}
		if (fstat(data, &file_stat) < 0)
		    {
		            fprintf(stderr, "Error fstat --> %s", strerror(errno),"\n");

		            exit(EXIT_FAILURE);
		    }

		/*
		map = (char*) mmap(0, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, data, 0);
	    if (map == MAP_FAILED)
	        {
	            close(data);
	            perror("Error en el mapeo del data.bin.\n");
	            exit(EXIT_FAILURE);
	        }
		*/

	    nodo->espacio_total = (int) file_stat.st_size;
	    strcpy(nodo->nombre,nombreNodo);
	    nodo->espacio_libre = 0;

	    return nodo;
}

void iniciarDataNode(){

	char* buffer;
	t_nodo *nodo;
	infoConfig = config_create("config.txt");

		if(config_has_property(infoConfig,"IP_FILESYSTEM")){
			fsIP = config_get_string_value(infoConfig,"IP_FILESYSTEM");
			printf("IP: %s\n", fsIP);
		}

		if(config_has_property(infoConfig,"PUERTO_FILESYSTEM")){
				fsPort = config_get_int_value(infoConfig,"PUERTO_FILESYSTEM");
				printf("Puerto: %d\n", fsPort);
		}

		if(config_has_property(infoConfig,"NOMBRE_NODO")){
				nombreNodo = config_get_string_value(infoConfig,"NOMBRE_NODO");
				nombreNodo[strlen(nombreNodo)+1]='\0';
		}

		if(config_has_property(infoConfig,"NOMBRE_NODO")){
				rutaNodo = config_get_string_value(infoConfig,"RUTA_DATABIN");
				rutaNodo[strlen(rutaNodo)+1]='\0';
		}

		if(config_has_property(infoConfig, "PUERTO_WORKER")){
				puerto = config_get_int_value(infoConfig, "PUERTO_WORKER");
		}

		socketConn = conectarseA(fsIP, fsPort);
		enviarInt(socketConn, PROCESO_NODO);

		nodo = inicializoDataBin(rutaNodo, nombreNodo);

		printf("tamanio total %d\n",nodo->espacio_total);

		void* buff = serializarNodo(nodo);
		enviarInt(socketConn,(int)(sizeof(t_nodo)));
		send(socketConn,buff,sizeof(t_nodo),NULL);

		//enviarMensaje(socketConn, (char*)serializarNodo(nodo));
		//enviarInt(socketConn, nodo.espacio_total);
		free(buff);

		int errorConexionFS = 0;
		while(1){
			esperarBloque(socketConn, nodo, rutaNodo);
			if(errorConexionFS<0){
				printf("Se desconecto el file system\n");
				break;
			}

		}

		/*
		mapearDataBin(rutaNodo);
		esperarPeticiones(socketConn);
		 */
}


void esperarBloque(int socketConn,t_nodo* nodo, char* rutaNodo){

	unsigned char* map;
	int bloque;
	recibirInt(socketConn, bloque);
	char * bloqueArchivo = recibirMensaje(socketConn);

	FILE* data = fopen(rutaNodo,"w+");
	map = (char*) mmap(0, nodo->espacio_total, PROT_READ | PROT_WRITE, MAP_SHARED, data, bloque*(1024*1024));
	if (map == MAP_FAILED)
	   {
	    close(data);
	    perror("Error en el mapeo del data.bin.\n");
	    exit(EXIT_FAILURE);
	   }
	int i=0;
	for (;i<(1024*1024);i++){
		map[i]=bloqueArchivo[i];
	}
	munmap(map,nodo->espacio_total);


}


void *serializarNodo(t_nodo* nodo){
	void* serializado = malloc(sizeof(t_nodo));
	int offset = 0;
	serializar_desde_int(serializado, nodo->espacio_total, &offset);
	serializar_desde_int(serializado, nodo->espacio_libre, &offset);
	serializar_desde_string(serializado, nodo->nombre, sizeof(char[10]),&offset);
	return serializado;

}
