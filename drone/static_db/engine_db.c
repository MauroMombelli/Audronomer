#include "engine_db.h"

/* here we initalize the array of structure */
#define SEP ,
#define X(a,b) b
static uint8_t * const generic[] =
{
	#include "array_initialization.h"
};
#undef X
#undef SEP

static uint8_t update_number[sizeof(generic)/sizeof(generic[0])] = {0};

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
