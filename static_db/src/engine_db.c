#include "engine_db.h"

static Mutex mtx;

void init_static_generics(void){
	chMtxInit(&mtx);
}

void my_memcpy(void * destination, void * source, size_t size ){
	chMtxLock(&mtx);
	__builtin_memcpy(destination, source, size);
	chMtxUnlock();
}

/* here we create all the getter function. add here your array locking code */
#define SEP ;
#define X(a,b) uint8_t get_##a(struct a * dest){my_memcpy(dest, generic[a], sizeof(struct a) ); return update_number[a];}
#include "array_initialization.h"
#undef X
#undef SEP

/* here we create all the putter function. add here your array locking code */
#define SEP ;
#define X(a,b) void put_##a(struct a * source){update_number[a]++; my_memcpy(generic[a], source, sizeof(struct a) );}
#include "array_initialization.h"
#undef X
#undef SEP
