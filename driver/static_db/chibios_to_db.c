/*
 * custom_function.c
 *
 *  Created on: 16/mag/2015
 *      Author: mauro
 */

#include "ch.h"

static Mutex mtx;

void my_memcpy(void * destination, void * source, size_t size ){
	chMtxLock(&mtx);
	__builtin_memcpy(destination, source, size);
	chMtxUnlock();
}

void init_static_generics(void){
	chMtxInit(&mtx);
}
