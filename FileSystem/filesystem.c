/*
 * filesystem.c
 *
 *  Created on: 3/9/2017
 *      Author: utnso
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



/*

Funciones que ha de entender el fs

extern char *xmalloc PARAMS((size_t));

 Declaracion de las funciones del fs.
int com_format PARAMS((char *));
int com_rm PARAMS((char *));
int com_rename PARAMS((char *));
int com_mv PARAMS((char *));
int com_pwd PARAMS((char *));
int com_cat PARAMS((char *));
int com_list PARAMS((char *));
int com_cd PARAMS((char *));
int com_quit PARAMS((char *));
int com_mkdir PARAMS((char *));

/* La estructura que contiene la info de los comandos que este fs entenderá:

typedef struct {
  char *name;			/* Nombre del comando para el usuario.
  rl_icpfunc_t *func;		/* Función que llama el comando.
  char *doc;			/* Documentación para la función.
} COMMAND;

COMMAND commands[] = {
  { "format", com_format, "Formatear el File System." },
  { "rm ", com_rm , "Borrar un archivo." },
  { "rename ", com_rename , "Renombrar un archivo." },
  { "mv", com_mv, "Mover un archivo." },
  { "cat", com_cat, "Muestra el contenido como texto plano." },
  { "ls", com_list, "Lista los archivos de un directorio." },
  { "mkdir", com_mkdir, "Crea un directorio nuevo." },
  { "pwd", com_pwd, "Imprime el directorio actual." },
  { "exit", com_quit, "Salir." },
  { "rename", com_rename, "Rename FILE to NEWNAME" },
  { "cpfrom", com_cpfrom, "Copia de mi disco local al file system, siguiendo los lineamientos en la operacion Almacenar Archivo, de la Interfaz del FileSystem." },
  { "cpto ", com_view, "Copia de mi disco local al file system." },
  cpblock
  md5
  info
  { (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
};
*/

void main() {

	escucharConsola();

}

