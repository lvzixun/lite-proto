#ifndef  _LP_TABLE_H_
#define  _LP_TABLE_H_
#include "../lp_conf.h"
#include "lp_list.h"

typedef struct _lp_string{
	lp_list str;
}lp_string;

typedef struct _lp_table_one{
	int value;
	lp_string name;
	struct _lp_table_one* next;
}lp_table_one;

#define  TABLE_SIZE		128
typedef struct _lp_table{
	lp_table_one* table_list[TABLE_SIZE];
	size_t table_len;
}lp_table;


unsigned int _BKDRHash(char *str, int len);
int lp_table_new(lp_table* lp_t);
int lp_table_add(lp_table* lp_t, char* name, int value);
int* lp_table_query(lp_table* lp_t, char* name);
int lp_table_free(lp_table* lp_t);

lp_string lp_string_new(char* str);
lp_string* lp_string_cat(lp_string* lp_s, char at_char);
lp_string* lp_string_cats(lp_string* lp_s, char* str);
void lp_string_free(lp_string* lp_s);
void lp_string_clear(lp_string* lp_s);

#endif

