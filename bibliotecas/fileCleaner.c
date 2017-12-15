/*
 * fileCleaner.c
 *
 *  Created on: 15/12/2017
 *      Author: utnso
 */

#include "fileCleaner.h"

void vaciarArchivo(char* ruta){

	FILE* fclean = fopen(ruta, "w");
	fclose(fclean);

}
