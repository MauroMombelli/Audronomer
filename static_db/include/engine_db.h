#ifndef ENGINE_DB_H_
#define ENGINE_DB_H_

#include "ch.h"
#include "hal.h"

#include "struct.h"

void init_static_generics(void);

void my_memcpy(void * destination, void * source, size_t size );

/* this macro will create the right X macro element, and also initiliaze the "anonymous" struct */
#define ADD_STRUCT_TO_ARRAY(xu) X(xu, &(struct xu){0})SEP

/* here we initialize the enum, where the type of the struct is the key, and the value its position in the array */
#define SEP ,
#define X(a,b) a
enum STRUCT {
	#include "array_initialization.h"
};
#undef X

/* here we initalize the array of structure */
#define X(a,b) b
static void * const generic[] =
{
	#include "array_initialization.h"
};
#undef X
#undef SEP

static uint8_t update_number[sizeof(generic)/sizeof(generic[0])] = {0};

/* here we create all the getter function. add here your array locking code */
#define SEP ;
#define X(a,b) extern uint8_t get_##a(struct a * dest);
#include "array_initialization.h"
#undef X
#undef SEP

/* here we create all the putter function. add here your array locking code */
#define SEP ;
#define X(a,b) extern void put_##a(struct a * source);
#include "array_initialization.h"
#undef X
#undef SEP 

#endif
