/*
 * serializacion.c
 *
 *  Created on: 2/10/2017
 *      Author: utnso
 */


void serializar_desde_int(void* buffer, int value, int* offset){
	int offset_loc = *offset;
	memcpy(buffer + offset_loc, &value, sizeof(int));
	*offset = offset_loc + sizeof(int);
}

void serializar_desde_string(void* buffer, char* value, int size, int* offset){
	int offset_loc = *offset;
	memcpy(buffer + offset_loc, value, size);
	*offset = offset_loc + size;
}


void deserializar_a_int(void* buffer, int* value, int* offset){
	int offset_loc = *offset;
	memcpy(value, buffer + offset_loc, sizeof(int));
	*offset = offset_loc + sizeof(int);
}

void deserializar_a_string(void* buffer, char* value, int size, int* offset){
	int offset_loc = *offset;
	memcpy(value, buffer + offset_loc, size+1);
	*offset = offset_loc + size + 1;
}
