#ifndef  _LIB_STRINGPOOL_H_
#define  _LIB_STRINGPOOL_H_
#include "lib_table.h"

typedef struct _string_pool_slot{
	char*  str;
	size_t len;
	struct _string_pool_slot* next;
}string_pool_slot;

typedef struct _string_pool{
	string_pool_slot* pool;
	llp_map* string_table;
}string_pool;

string_pool* lib_stringpool_new();
void lib_stringpool_free(string_pool* sp);
char* lib_stringpool_add(string_pool* sp, const char* str);

#endif
