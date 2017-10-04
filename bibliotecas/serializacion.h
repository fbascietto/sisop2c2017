/*
 * serializacion.h
 *
 *  Created on: 2/10/2017
 *      Author: utnso
 */

#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void serializar_desde_int(void* buffer, int value, int* offset);
void serializar_desde_string(void* buffer, char* value, int size, int* offset);

void deserializar_a_int(void* buffer, int* value, int* offset);
void deserializar_a_string(void* buffer, char* value, int size, int* offset);

#endif /* SERIALIZACION_H_ */
