 /*
 *  Created on: 6/9/2017
 *      Author: utnso
 */
#include "sockets.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/sendfile.h>
#include "protocolo.h"

#define TAMBUFFER 1024
#define MAX_CLIENTES 10



int escuchar(int puerto) {
	int socketEscucha;
	struct sockaddr_in address;
	if ((socketEscucha = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		return -1;

	}
	int activador = 1;
	if (setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, (char *) &activador,
			sizeof(activador)) < 0) {
		return -1;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(puerto);

	if (bind(socketEscucha, (struct sockaddr *) &address, sizeof(address))
			< 0) {
		printf("Error al bindear\n");
		return -1;
	}
	if (listen(socketEscucha, MAX_CLIENTES) < 0) {
		printf("Error al escuchar\n");
		return -1;
	}
	return socketEscucha;
}

int aceptarConexion(int socketEscucha) {
	int nuevoSocket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	if ((nuevoSocket = accept(socketEscucha, (struct sockaddr *) &address,
			(socklen_t*) &addrlen)) < 0) {
		return 1;
	}
	return nuevoSocket;
}

int conectarseA(char *ip, int puerto) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);
	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor))
			!= 0) {
		perror("No se pudo conectar\n");
		return 0;
	}
	return cliente;
}


int enviarMensaje(int socketDestino, char* mensaje) {
	int totalEnviado=0, bytesRestantes, lenEnviado;

	int len = strlen(mensaje);
	bytesRestantes = len;
	enviarInt(socketDestino, len);
	while(totalEnviado < len) {
		lenEnviado = send(socketDestino, mensaje, len, 0);
		if(lenEnviado == -1){ perror("error al enviar\n"); return -1;}
		totalEnviado = totalEnviado + lenEnviado;
		bytesRestantes = bytesRestantes - lenEnviado;
	}
	return len;
}

int esperarConexionesSocket(fd_set *master, int socketEscucha) {
 	//dado un set y un socket de escucha, verifica mediante select, si hay alguna conexion nueva para aceptar
 	int nuevoSocket = -1;
 	fd_set readSet;
 	FD_ZERO(&readSet);
 	readSet = *(master);
 	if (select(socketEscucha + 1, &readSet, NULL, NULL, NULL) == -1) {
 		perror("select");
 		exit(4);
 	}
 	if (FD_ISSET(socketEscucha, &readSet)) {
 		// handle new connections
 		nuevoSocket = aceptarConexion(socketEscucha);
 	}
 	return nuevoSocket;
 }


char *recibirMensaje(int socketDestino) {
	//size es el tamaño que previamente me envio socketDestino, y mensaje tiene ese tamaño
	int largoLeido = -1, size, llegoTodo = 0;
	recibirInt(socketDestino, &size);
	char *mensaje = malloc(size+1);
	while(!llegoTodo){
		largoLeido = recv(socketDestino, mensaje, size, 0);
		if(largoLeido == 0){ //toda esta fumada es para cuando algun cliente se desconecta.
			printf("Socket dice: Cliente en socket N° %d se desconecto\n", socketDestino);
			mensaje = "-1";
			return (mensaje);
		}
		//si el largo leido es menor que el size, me quedan cosas por leer, sino llego el mensaje completo
		if(largoLeido < size) size= size - largoLeido;
		else llegoTodo = 1;
	}
	mensaje[largoLeido] = '\0';
	return mensaje;
}

int enviarInt(int socketDestino, int num){
	int status;
	void* bufferEnviarInt = malloc(sizeof(int));
	memcpy(bufferEnviarInt,&num,sizeof(int));
	status = send(socketDestino,bufferEnviarInt,sizeof(int),0);
	free(bufferEnviarInt);
	return status;
}

int recibirInt(int socketDestino, int* i) {
	int len_leida;
	len_leida = recv(socketDestino, i, sizeof(int), 0);
	return len_leida;
}

void selectRead(int descriptor, fd_set* bag, struct timeval* timeout) {
	select(descriptor, bag, NULL, NULL, timeout);
}

void selectWrite(int descriptor, fd_set* bag, struct timeval* timeout) {
	select(descriptor, NULL, bag, NULL, timeout);
}

void selectException(int descriptor, fd_set* bag, struct timeval* timeout) {
	select(descriptor, NULL, NULL, bag, timeout);
}



char * esperarMensajeSocket(fd_set *socketsClientes, int socketMaximo) {
	int i = -1;
	fd_set readSet;
	FD_ZERO(&readSet);
	readSet = *(socketsClientes);
	if (select(socketMaximo + 1, &readSet, NULL, NULL, NULL) == -1) {
		perror("select");
		exit(4);
	}
	for (i = 0; i <= socketMaximo; i++) {
		if (FD_ISSET(i, &readSet)) {
			char *mensaje = recibirMensaje(i);
			return mensaje;
		}
	}
}

int recibirHandShake (int unSocket) {
	int ret = 0, len;
	len = recibirInt(unSocket, &ret);
	if(len == -1) perror("error recibiendo handshake\n");
	return ret;
}

int envioArchivo(int peer_socket, char * archivo){

	ssize_t len;
	struct sockaddr_in      peer_addr;
	int fd;
	int sent_bytes = 0;
	char file_size[1024];
	struct stat file_stat;
	int offset;
	int remain_data;

    fd = open(archivo, O_RDONLY);
    if (fd == -1)
    {
            fprintf(stderr, "Error abriendo archivo --> %s", strerror(errno));

            exit(EXIT_FAILURE);
    }

    if (fstat(fd, &file_stat) < 0)
    {
            fprintf(stderr, "Error fstat --> %s", strerror(errno));

            exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Tamaño: \n%d bytes\n", file_stat.st_size);

    /* fprintf(stdout, "Acepto nodo --> %s\n", inet_ntoa(peer_addr.sin_addr));*/

    enviarInt(peer_socket, ENVIAR_ARCHIVO_TEXTO);
    /* Envio tamaño archivo */

    //len = send(peer_socket, file_size, sizeof(file_size), 0);
    enviarInt(peer_socket, file_stat.st_size);
    /* envio nombre archivo */
    // send(peer_socket, archivo, sizeof(archivo), 0);

    enviarMensaje(peer_socket, archivo);

    if (len < 0)
    {
          fprintf(stderr, "Error enviando datos preliminares --> %s", strerror(errno));

          exit(EXIT_FAILURE);
    }

    fprintf(stdout, "FS envia %d bytes de tamaño de archivo\n", len);

    offset = 0;
    remain_data = file_stat.st_size;

    /* Sending file data */
    while (((sent_bytes = sendfile(peer_socket, fd, &offset, TAMBUFFER)) > 0) && (remain_data > 0))
    {
            fprintf(stdout, "1. FS envio %d bytes de los datos de archivo, el offset es : %d y faltan = %d bytes\n", sent_bytes, offset, remain_data);
            remain_data -= sent_bytes;
            fprintf(stdout, "2. FS envio %d bytes de los datos de archivo, el offset es : %d y faltan = %d bytes\n", sent_bytes, offset, remain_data);
    }

    int fileClose = close(fd);

    if(fileClose == 0){
    	printf("Se cerro el archivo archivo %s", archivo);
    }
    close(peer_socket);
    return 0;

}
void *recibirArchivo(int client_socket){

			ssize_t len;
			char * nombre_archivo;
	        struct sockaddr_in remote_addr;
	        char buffer[TAMBUFFER];
	        int file_size;
	        FILE *received_file;


	        //recv(client_socket, buffer, TAMBUFFER, 0);
	        //file_size = atoi(buffer);

	        recibirInt(client_socket, &file_size);
	        int remain_data = file_size;
	        //recv(client_socket, nombre_archivo, TAMBUFFER, 0);
	        nombre_archivo = recibirMensaje(client_socket);
	        if(!strcmp(nombre_archivo,"-1")){
	        	printf("error al recibir el nombre del archivo");
	        }else{
	        	received_file = fopen(nombre_archivo, "w");

				if (received_file == NULL)
				{
						fprintf(stderr, "Fallo al abrir el archivo %s\n", strerror(errno));

						exit(EXIT_FAILURE);
				}
				while (((len = recv(client_socket, buffer, TAMBUFFER, 0)) > 0) && (remain_data > 0))
			   {
					   fwrite(buffer, sizeof(char), len, received_file);
					   remain_data -= len;
					   fprintf(stdout, "Recibidos %d bytes y se esperan :- %d bytes\n", len, remain_data);
			   }
			   fclose(received_file);

			   close(client_socket);
	        }

}

char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[255];
  char *p;

  if(!(p = strstr(str, orig)))
    return str;

  strncpy(buffer, str, p-str);
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}

/*
* Escribe dato en el socket cliente. Devuelve numero de bytes escritos,
* o -1 si hay error.
*/
int escribirSocketClient(int fd, char *datos, int longitud)
{
	int escrito = 0;
	int aux = 0;

	/*
	* Comprobacion de los parametros de entrada
	*/
	if ((fd == -1) || (datos == NULL) || (longitud < 1))
		return -1;

	/*
	* Bucle hasta que hayamos escrito todos los caracteres que nos han
	* indicado.
	*/
	while (escrito < longitud)
	{
		aux = write (fd, datos + escrito, longitud - escrito);
		if (aux > 0)
		{
			/*
			* Si hemos conseguido escribir caracteres, se actualiza la
			* variable escrito
			*/
			escrito = escrito + aux;
		}
		else
		{
			/*
			* Si se ha cerrado el socket, devolvemos el numero de caracteres
			* leidos.
			* Si ha habido error, devolvemos -1
			*/
			if (aux == 0)
				return escrito;
			else
				return -1;
		}
	}

	/*
	* Devolvemos el total de caracteres leidos
	*/
	return escrito;
}


char* serializarMensaje(Package *package){

	char *serializedPackage = calloc(1,getLongitudPackage(package));

	int offset = 0;
	int size_to_send;

	size_to_send =  sizeof(package->msgCode);
	memcpy(serializedPackage + offset, &(package->msgCode), size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(package->message_long);
	memcpy(serializedPackage + offset, &(package->message_long), size_to_send);
	offset += size_to_send;

	size_to_send =  package->message_long;
	memcpy(serializedPackage + offset, package->message, size_to_send);

	return serializedPackage;
}

//recibe el paquete y lo deserializa
int recieve_and_deserialize(Package *package, int socketCliente){

	int leidos = 0;
	int aux = 0;

	int buffer_size;
	char *buffer = malloc(buffer_size = sizeof(uint32_t));

	aux = recv(socketCliente, buffer, sizeof(package->msgCode), 0);
	memcpy(&(package->msgCode), buffer, buffer_size);
	if (!aux) return 0;
	leidos+=aux;

	aux = recv(socketCliente, buffer, sizeof(package->message_long), 0);
	memcpy(&(package->message_long), buffer, buffer_size);
	if (!aux) return 0;
	leidos+=aux;

	package->message = malloc(sizeof(char)*package->message_long);
	aux = recv(socketCliente, package->message, package->message_long, 0);
	if (!aux) return 0;
	leidos+=aux;

	free(buffer);

	return leidos;
}

void destroyPackage(Package* package){
	if(package!=NULL){
		free(package->message);
	}
	free(package);
}

char* fileToChar(char* filename){
	int fd;                    /* file descriptor for file to send */
	struct stat stat_buf;      /* argument to fstat */
	off_t offset = 0;          /* file offset */
	/* open the file to be sent */
	fd = open(filename, O_RDONLY);
	if (fd == -1) {
	  fprintf(stderr, "unable to open '%s': %s\n", filename, strerror(errno));
	  exit(1);
	}

	/* get the size of the file to be sent */
	fstat(fd, &stat_buf);

	/* copy file using sendfile */
	offset = 0;

	size_t toRead = stat_buf.st_size;
	char* buffer = calloc(1, toRead + 1);

	int numRead = read(fd, buffer, toRead);
	if (numRead == -1)
		return -1;

	close(fd);

	buffer[strlen(buffer)+1] = '\0';

	return buffer;
}

int enviarMensajeSocketConLongitud(int socket, uint32_t accion, char* mensaje, uint32_t longitud){
	Package* package = fillPackage(accion,mensaje,longitud);
	char* serializedPkg = serializarMensaje(package);
	int resultado;

	resultado = escribirSocketClient(socket, (char *)serializedPkg, getLongitudPackage(package));

	free(serializedPkg);
	destroyPackage(package);
	return resultado;
}

int enviarMensajeSocket(int socket, uint32_t accion, char* mensaje){
	return enviarMensajeSocketConLongitud(socket,accion,mensaje,strlen(mensaje)+sizeof(char));
}


Package* fillPackage(uint32_t msgCode, char* message, uint32_t message_long){
	Package	*package = malloc(sizeof(Package));
	package->message = malloc(sizeof(char)*message_long);
	memcpy(package->message,message,message_long);
	package->message_long = message_long;
	package->msgCode = msgCode;
	return package;
}

Package* createPackage(){
	Package* tmp;

	tmp=malloc(sizeof(Package));
	tmp->message=NULL;

	return tmp;
}

int getLongitudPackage(Package *package){
	return sizeof(package->msgCode)+sizeof(package->message_long)+(sizeof(char)*package->message_long);
}


int leerSocketClient(int fd, char *datos, int longitud)
{
	int leido = 0;
	int aux = 0;

	/*
	* Comprobacion de que los parametros de entrada son correctos
	*/
	if ((fd == -1) || (datos == NULL) || (longitud < 1))
		return -1;

	/*
	* Mientras no hayamos leido todos los datos solicitados
	*/
	while (leido < longitud)
	{
		aux = read (fd, datos + leido, longitud - leido);
		if (aux > 0)
		{
			/*
			* Si hemos conseguido leer datos, incrementamos la variable
			* que contiene los datos leidos hasta el momento
			*/
			leido = leido + aux;
		}
		else
		{
			/*
			* Si read devuelve 0, es que se ha cerrado el socket. Devolvemos
			* los caracteres leidos hasta ese momento
			*/
			if (aux == 0)
				return leido;
			if (aux == -1)
			{
				/*
				* En caso de error, la variable errno nos indica el tipo
				* de error.
				* El error EINTR se produce si ha habido alguna
				* interrupcion del sistema antes de leer ningun dato. No
				* es un error realmente.
				* El error EGAIN significa que el socket no esta disponible
				* de momento, que lo intentemos dentro de un rato.
				* Ambos errores se tratan con una espera de 100 microsegundos
				* y se vuelve a intentar.
				* El resto de los posibles errores provocan que salgamos de
				* la funcion con error.
				*/
				switch (errno)
				{
					case EINTR:
					case EAGAIN:
						usleep (100);
						break;
					default:
						return -1;
				}
			}
		}
	}

	/*
	* Se devuelve el total de los caracteres leidos
	*/
	return leido;
}





