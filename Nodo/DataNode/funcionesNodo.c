#include "funcionesNodo.h"
#include "../../bibliotecas/sockets.h"
#include "../../bibliotecas/protocolo.h"
#include "../../bibliotecas/serializacion.c"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include "../../bibliotecas/fileCleaner.c"




t_nodo* inicializoDataBin(char* rutaBin, char* nombreNodo, int puerto){

		t_nodo* nodo;
		nodo = malloc(sizeof(t_nodo));
		struct stat file_stat;
		int data;
		data = open(rutaBin,O_RDWR);

		if (data == NULL){
					printf("No se pudo abrir el archivo.\n");
					 exit(EXIT_FAILURE);
		}
		if (fstat(data, &file_stat) < 0){
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
	    nodo->puerto = puerto;
	    nodo->espacio_libre = 0;
	    close(rutaBin);
	    return nodo;
}

void iniciarDataNode(){

	vaciarArchivo("log.txt");

	t_log_level LogL = LOG_LEVEL_TRACE;
	logNodo = log_create("log.txt","DataNode",1,LogL);


	t_nodo *nodo;
	infoConfig = config_create("../config.txt");

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

		if(config_has_property(infoConfig,"RUTA_DATABIN")){
				rutaNodo = config_get_string_value(infoConfig,"RUTA_DATABIN");
				rutaNodo[strlen(rutaNodo)+1]='\0';
		}

		if(config_has_property(infoConfig, "PUERTO_WORKER")){
				puerto = config_get_int_value(infoConfig, "PUERTO_WORKER");
		}

		socketConn = conectarseA(fsIP, fsPort);
		enviarInt(socketConn, PROCESO_NODO);

		nodo = inicializoDataBin(rutaNodo, nombreNodo, puerto);

		log_trace(logNodo,"tamanio total %d\n",nodo->espacio_total);

		void* buff = serializarNodo(nodo);
		enviarInt(socketConn,(int)(sizeof(t_nodo)));
		send(socketConn,buff,sizeof(t_nodo),NULL);

		//enviarMensaje(socketConn, (char*)serializarNodo(nodo));
		//enviarInt(socketConn, nodo.espacio_total);
		free(buff);

		int errorConexionFS = 0;
		int solicitudFs;
		while(1){
			errorConexionFS =  recibirInt(socketConn,&solicitudFs);
			if(errorConexionFS<=0){
				log_trace(logNodo,"Se desconecto el file system");

				break;
			}else{
				switch (solicitudFs){
					case ESCRIBIR_BLOQUE_NODO:
						esperarBloque(socketConn, nodo, rutaNodo);
						break;
					case LEER_BLOQUE_NODO:
						leerBloque(socketConn, nodo, rutaNodo);
						break;
					case ESTA_VIVO_NODO:
						enviarInt(socketConn,1);
						break;
				}

			}

		}
		log_destroy(logNodo);
		/*
		esperarPeticiones(socketConn);
		 */
}


int esperarBloque(int socketConn,t_nodo* nodo, char* rutaNodo){

	unsigned char* map;
	//map = malloc(sizeof(unsigned char)*(1024*1024));
	int bloque;
	int err = recibirInt(socketConn, &bloque);
	if(err<=0){
		return -1;

	}
		char * bloqueArchivo;
		bloqueArchivo = malloc((size_t)BUFBLOQ);

		int data = open(rutaNodo,O_RDWR);
		struct stat fileStat;
		if (fstat(data, &fileStat) < 0){
			log_error(logNodo, "Error fstat");
			exit(EXIT_FAILURE);
		}

		map = (unsigned char*) mmap(NULL, fileStat.st_size /*/(fileStat.st_size/(1024*1024))*/ , PROT_READ | PROT_WRITE, MAP_SHARED, data, sizeof(unsigned char)*bloque*(1024*1024));
		if (map == MAP_FAILED){
			close(data);
			log_error(logNodo,"Error en el mapeo del data.bin.\n");
			exit(EXIT_FAILURE);
		   }


		int i = 0;
		int j = 0;
		int largoMensaje = 0;
		int bytesRecibidos = 0;
		int recibido = 0;

				recibirInt(socketConn,&largoMensaje);

				while(recibido<largoMensaje){
					int bytesAleer = 0;
					recibirInt(socketConn,&bytesAleer);
					while(bytesRecibidos<bytesAleer){
						bytesRecibidos += recv(socketConn,bloqueArchivo,(size_t)bytesAleer-bytesRecibidos,NULL);
						for (;j<strlen(bloqueArchivo);j++){
								map[i]=bloqueArchivo[j];
								i++;
						}
						j=0;
					}
					recibido += bytesRecibidos;
					bytesRecibidos = 0;


				}
				//bloqueArchivo = recibirMensaje(socketConn);
				//paquetesRecibidos++;

					/*
					int fin_archivo = strchr(bloqueArchivo,"\0");
					if(fin_archivo){
						break;
					}*/

		log_trace(logNodo,"Se escribió con exito sobre bloque %d.", bloque);
		munmap(map,fileStat.st_size);
		// free(map);
		free(bloqueArchivo);
		close(data);
	return recibido;
}


void *serializarNodo(t_nodo* nodo){
	void* serializado = malloc(sizeof(t_nodo));
	int offset = 0;
	serializar_desde_int(serializado, nodo->espacio_total, &offset);
	serializar_desde_int(serializado, nodo->espacio_libre, &offset);
	serializar_desde_int(serializado, nodo->puerto, &offset);
	serializar_desde_string(serializado, nodo->nombre, sizeof(char[10]),&offset);
	return serializado;

}

int leerBloque(int socketConn,t_nodo* nodo, char* rutaNodo){
	unsigned char* map;
		map = malloc(sizeof(unsigned char)*(1024*1024));
		int bloque;
		int err = recibirInt(socketConn, &bloque);

		if(err<0){
			log_error(logNodo,"error de conexion con el fs");
			return -1;
		}
		int data = open(rutaNodo,O_RDWR);
		struct stat fileStat;
		if (fstat(data, &fileStat) < 0){
			log_error(logNodo,"Error fstat --> %s");
			  exit(EXIT_FAILURE);
		}

		map = (unsigned char*) mmap(NULL, fileStat.st_size /*/(fileStat.st_size/(1024*1024))*/ , PROT_READ, MAP_SHARED, data, sizeof(unsigned char)*bloque*(1024*1024));
		if (map == MAP_FAILED){
			close(data);
			log_error(logNodo,"Error en el mapeo del data.bin.\n");
			exit(EXIT_FAILURE);
		   }

		int bytesAleer;
		int err1 = recibirInt(socketConn, &bytesAleer);

		if(err1<0){
			log_error(logNodo,"Error de conexion con el fs.");
			return -1;
		}

		int bytes_totales_leidos = 0;
		int bytes_leidos = 0;
		//enviarInt(socketConn, LEER_BLOQUE_NODO);
		while(bytes_totales_leidos < bytesAleer){
			unsigned char * buffer;
			buffer = malloc((size_t)BUFBLOQ);

			for (;bytes_leidos<BUFBLOQ && bytes_totales_leidos< bytesAleer;bytes_totales_leidos++){
				buffer[bytes_leidos] = map[bytes_totales_leidos];
				bytes_leidos++;
			}
			enviarInt(socketConn,bytes_leidos);
			int bytes_leidos_eviados = 0;
			while(bytes_leidos_eviados<bytes_leidos){
				bytes_leidos_eviados += send(socketConn,buffer+bytes_leidos_eviados,(size_t) bytes_leidos-bytes_leidos_eviados,NULL);
			}
				/*free(buffer);
				printf("error de conexion con el fs");
				return -1;
			}*/
			//printf("%s\n",buffer); printea el buffer para ver que onda -- no more
			bytes_leidos=0;

			free(buffer);
		}


		log_trace(logNodo,"Se leyó con exito sobre bloque %d.", bloque);
		munmap(map,fileStat.st_size);
		// free(map);
		close(data);
		return 1;
}
