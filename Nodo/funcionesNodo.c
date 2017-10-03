#include "funcionesNodo.h"
#include "../bibliotecas/sockets.h"
#include "../bibliotecas/protocolo.h"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>




t_nodo mapearDataBin(char* rutaBin, char* nombreNodo){

		t_nodo nodo;
		struct stat file_stat;
		unsigned char* map;
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


	   // int sz = getpagesize() * 256;

	    map = (char*) mmap(0, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, data, 0);

	    if (map == MAP_FAILED)
	        {
	            close(data);
	            perror("Error en el mapeo del data.bin.\n");
	            exit(EXIT_FAILURE);
	        }

	    nodo.mapa = map;
	    nodo.fsize = (int) file_stat.st_size;
	    nodo.nombre = nombreNodo;

	    return nodo;
}

void iniciarDataNode(){

	char* buffer;
	t_nodo nodo;
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

		socketConn = conectarseA(fsIP, fsPort);
		enviarInt(socketConn, PROCESO_NODO);

		nodo = mapearDataBin(rutaNodo, nombreNodo);

		enviarMensaje(socketConn, nodo.nombre);
		enviarInt(socketConn, nodo.fsize);



		while(1){
			esperarBloque(socketConn);
		}


		/*
		mapearDataBin(rutaNodo);
		esperarPeticiones(socketConn);
		 */
}

void esperarBloque(int socketConn){
	//esperarInstruccion(socketConn);
	char * bloqueArchivo = recibirMensaje(socketConn);
	printf("%s", bloqueArchivo);

}
